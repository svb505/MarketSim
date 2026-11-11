#include "bounds.h"
#include "decorations.h"
#include "model.h"
#include "svbmath.h"
#include "camera.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "logger.h"
#include <format>

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::map<DecType, float> scales = {
    { DecType::BUILD1, 1.0f },
    { DecType::BUILD2, 1.0f },
    { DecType::TREE,  1.25f },
    { DecType::LAMP,  1.25f },
    { DecType::STREET,  0.7f }
};

DecType Decorations::stringToDecType(const std::string& type) {
    static const std::map<std::string, DecType> types = {
        { "house", DecType::BUILD2 },
        { "tree", DecType::TREE },
        { "lamp", DecType::LAMP },
        { "street", DecType::STREET }
    };

    auto it = types.find(type);

    if (it == types.end()) throw std::runtime_error("Unknown decoration type: " + type);

    return it->second;
}

static float getModelScale(DecType type) {
    auto it = scales.find(type);

    if (it == scales.end()) return 1.0f;

    return it->second;
}
void Decorations::addDecoration(DecType type,svbmath::Vec3 pos, float rotationY) {
    DecorationInstance decoration;

    decoration.type = type;
    decoration.position = pos;
    decoration.rotationY = rotationY;

    decorations.push_back(decoration);
}
void Decorations::initPositions(const std::string& jsonPath) {

    decorations.clear();

    std::ifstream file(jsonPath);

    if (!file.is_open()) {
        LOG_ERROR(std::format("Failed to open decorations JSON : {}", jsonPath));
        return;
    }

    json data;

    try {
        file >> data;

        if (!data.contains("objects") || !data["objects"].is_array()) {
            LOG_ERROR("Invalid decorations JSON: objects array missing");
            return;
        }

        for (const auto& object : data["objects"]) {

            int id = object.at("id").get<int>();

            std::string typeName = object.at("type").get<std::string>();

            const auto& pos = object.at("position");

            if (!pos.is_array() || pos.size() != 3) {
                LOG_ERROR(std::format("Invalid position for decoration ID: {}", id));
                continue;
            }

            svbmath::Vec3 position = {pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>() };

            float yaw = object.value("yaw", 0.0f);

            try {
                DecType type;

                if (typeName == "house") type = DecType::STREET;
                else type = stringToDecType(typeName);

                DecorationInstance decoration;

                decoration.id = id;
                decoration.type = type;
                decoration.position = position;
                decoration.rotationY = yaw;

                decorations.push_back(decoration);

            }
            catch (const std::exception& e) {
                LOG_ERROR(std::format("Decoration ID {} skipped: {}",id,e.what()));
            }
        }

        LOG_INFO(std::format("Loaded {} decorations from JSON",decorations.size()));

    }
    catch (const std::exception& e) {
        LOG_ERROR(std::format( "Failed to parse decorations JSON: {}", e.what()));
    }
}

void Decorations::initDecoration(const std::string& modelPath, DecType decType,
    const std::string& texturePath) {

    float scale = getModelScale(decType);

    auto model = std::make_unique<OBJModel>();

    if (!model->LoadOBJ(modelPath, scale)) {
        LOG_ERROR(std::format("Failed to load decoration model: {}", modelPath));

        return;
    }

    if (!texturePath.empty())  model->LoadTexture(texturePath);

    model->CreateVBO();

    models[decType] = std::move(model);
}
void Decorations::drawAllDecorations(DecType decType) {
    auto modelIt = models.find(decType);

    if (modelIt == models.end()) return;

    OBJModel* model = modelIt->second.get();

    if (!model) return;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);


    for (const DecorationInstance& decoration : decorations) {
        if (decoration.type != decType) continue;

        model->Draw(decoration.position, decoration.rotationY, 0.0f);
    }


    glDisable(GL_CULL_FACE);
}

static Bounds transformBounds(const Bounds& local, float scale,
    const DecorationInstance& decoration) {
    Bounds worldBounds = local;

    const svbmath::Vec3& position = decoration.position;
    float rotation = decoration.rotationY;

    while (rotation < 0.0f) rotation += 360.0f;
    while (rotation >= 360.0f) rotation -= 360.0f;


    if (rotation == 90.0f) {
        worldBounds.min.x = -local.max.z * scale + position.x;
        worldBounds.max.x = -local.min.z * scale + position.x;

        worldBounds.min.z = local.min.x * scale + position.z;
        worldBounds.max.z = local.max.x * scale + position.z;
    }
    else {
        worldBounds.min.x = local.min.x * scale + position.x;
        worldBounds.max.x = local.max.x * scale + position.x;

        worldBounds.min.z = local.min.z * scale + position.z;
        worldBounds.max.z = local.max.z * scale + position.z;
    }

    worldBounds.min.y = local.min.y * scale + position.y;
    worldBounds.max.y = local.max.y * scale + position.y;

    return worldBounds;
}

// Какие части модели твёрдые. Всё, что здесь вернёт false, не блокирует игрока.
// rue_sol - пол улицы; magasin_* - интерьер магазина, чтобы можно было зайти внутрь
// (magasin_etages_toit начинается выше роста игрока, поэтому оставлен твёрдым).
static bool isSolidPart(const std::string& name) {
    if (name == "rue_sol") return false;

    if (name.rfind("magasin_", 0) == 0 && name != "magasin_etages_toit")
        return false;

    return true;
}

Bounds Decorations::getWorldBounds(const OBJModel& model, const DecorationInstance& decoration) {
    return transformBounds(model.bounds, model.scale, decoration);
}
bool Decorations::checkCollision(Camera& cam, DecType decType) {
    auto modelIt = models.find(decType);

    if (modelIt == models.end()) return false;

    OBJModel* model = modelIt->second.get();

    if (!model) return false;

    for (const DecorationInstance& decoration : decorations) {
        if (decoration.type != decType) continue;

        // Модель без объектов ("o") - как раньше, одна общая коробка.
        if (model->parts.empty()) {
            if (checkAABBCollision(getWorldBounds(*model, decoration), cam.getCamPos()))
                return true;

            continue;
        }

        // Иначе проверяем каждое здание отдельно - проходы между ними свободны.
        for (const ObjPart& part : model->parts) {
            if (!isSolidPart(part.name)) continue;

            Bounds worldBounds = transformBounds(part.bounds, model->scale, decoration);

            if (checkAABBCollision(worldBounds, cam.getCamPos())) return true;
        }
    }

    return false;
}

