#include "bounds.h"
#include "decorations.h"
#include "model.h"
#include "svbmath.h"
#include "camera.h"
#include <vector>
#include <iostream>

std::map<DecType, float> scales = { {DecType::BUILD1,1.0f},
                                    {DecType::BUILD2, 0.25},
                                    {DecType::TREE, 0.25},
                                    {DecType::LAMP, 1.25}
};

void Decorations::initPositions() {
    svbmath::Vec3 currentPos = buildBasePos;

    for (int i = 0; i < buildsCount; i++) {
        svbmath::Vec3 bufferPos = currentPos;

        if (i < 3) bufferPos.x += buildXStep * (i + 1);
        else  bufferPos.x -= buildXStep * (i - 2);

        allPositions.push_back(bufferPos);
    }

    for (int i = 0; i < buildsCount; i++) {
        svbmath::Vec3 bufferPos = currentPos;
        bufferPos.z = 25.0f;

        if (i < 3) bufferPos.x += buildXStep * (i + 1);
        else  bufferPos.x -= buildXStep * (i - 2);

        allPositions.push_back(bufferPos);
    }

    // Здания, закрывающие конец улицы
    svbmath::Vec3 endBuilding1 = currentPos;
    endBuilding1.x = -35.0f;
    endBuilding1.z = 15.0f;
    allPositions.push_back(endBuilding1);

    svbmath::Vec3 endBuilding2 = currentPos;
    endBuilding2.x = 80.0f;
    endBuilding2.z = 15.0f;
    allPositions.push_back(endBuilding2);

    for (int i = 0; i < treesCount; i++) {
        svbmath::Vec3 bufferPos = currentPos;

        bufferPos.z = 18.0f;

        if (i < 3) bufferPos.x += buildXStep * (i + 1);
        else  bufferPos.x -= buildXStep * (i - 2);

        allPositions.push_back(bufferPos);
    }

    for (int i = 0; i < lampsCount; i++) {
        svbmath::Vec3 bufferPos = currentPos;

        bufferPos.z = 6.5f;

        if (i < 3) bufferPos.x += buildXStep * (i + 1);
        else  bufferPos.x -= buildXStep * (i - 2);

        allPositions.push_back(bufferPos);
    }


}
void Decorations::initDecoration(std::string modelPath, DecType decType, 
	std::string texturePath) {

    float scale = scales[decType];

	models.emplace(decType, new OBJModel());

	models[decType]->LoadOBJ(modelPath, scale);
	if (texturePath != "") models[decType]->LoadTexture(texturePath);

	models[decType]->CreateVBO();
}
void Decorations::drawAllDecorations(DecType decType) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    float yaw = 0.0f;
    int treesStart = buildsCount * 2 + 2;
    int lampsStart = buildsCount * 2 + 2 + treesCount;

    switch (decType) {
    case DecType::BUILD1:
        yaw = 90.0f;
        break;

    case DecType::BUILD2:
        yaw = 0.0f;
        break;

    case DecType::TREE:
        yaw = 0.0f;
        break;
    case DecType::LAMP:
        yaw = 0.0f;
        break;
    }

    if (decType != DecType::TREE) {
        for (int i = 0; i < buildsCount * 2; i++) {
            auto pos = allPositions[i];

            if (decType == DecType::BUILD2) pos.y -= 2.0f;

            models[decType]->Draw(pos, yaw, 0.0f);
        }

        auto pos1 = allPositions[buildsCount * 2];
        pos1.y -= 2.0f;
        auto pos2 = allPositions[buildsCount * 2 + 1]; 
        pos2.y -= 2.0f;

        models[decType]->Draw(pos1, 90.0f, 0.0f);
        models[decType]->Draw(pos2, 90.0f, 0.0f);
    }
    if (decType == DecType::TREE) {
        for (int i = treesStart; i < treesStart + treesCount; i++) {
            auto pos = allPositions[i];

            models[decType]->Draw(pos, yaw, 0.0f);
        }
    }
    if (decType == DecType::LAMP) {
        for (int i = lampsStart; i < lampsStart + lampsCount; i++) {
            auto pos = allPositions[i];

            models[decType]->Draw(pos, yaw, 0.0f);
        }
    }

    glDisable(GL_CULL_FACE);
            
}
bool Decorations::checkCollision(Camera& cam, DecType decType) {
    auto mod = models[decType];

    float step = (decType == DecType::BUILD2) ? 2.0f : 0.0f;

    int startIndex = 0;
    int count = 0;

    if (decType == DecType::TREE) {
        startIndex = buildsCount * 2 + 2;
        count = treesCount;
    }
    if (decType == DecType::LAMP) {
        startIndex = buildsCount * 2 + 2 + treesCount;
        count = lampsCount;
    }
    else {
        startIndex = 0;
        count = buildsCount * 2 + 2;
    }



    for (int i = startIndex; i < startIndex + count; i++) {
        bool rotated = false;

        if (decType != DecType::TREE && i >= buildsCount * 2) rotated = true;

        svbmath::Vec3 pos = allPositions[i];

        Bounds newBounds = mod->bounds;

        if (rotated){
            // rotation 90 degres
            float minX = -mod->bounds.max.z * mod->scale;
            float maxX = -mod->bounds.min.z * mod->scale;

            float minZ = mod->bounds.min.x * mod->scale;
            float maxZ = mod->bounds.max.x * mod->scale;

            newBounds.min.x = minX + pos.x;
            newBounds.max.x = maxX + pos.x;

            newBounds.min.z = minZ + pos.z;
            newBounds.max.z = maxZ + pos.z;

            newBounds.min.y = mod->bounds.min.y * mod->scale + pos.y - step;
            newBounds.max.y = mod->bounds.max.y * mod->scale + pos.y - step;
        }
        else{
            newBounds.min.x = mod->bounds.min.x * mod->scale + pos.x;
            newBounds.max.x = mod->bounds.max.x * mod->scale + pos.x;

            newBounds.min.y = mod->bounds.min.y * mod->scale + pos.y - step;
            newBounds.max.y = mod->bounds.max.y * mod->scale + pos.y - step;

            newBounds.min.z = mod->bounds.min.z * mod->scale + pos.z;
            newBounds.max.z = mod->bounds.max.z * mod->scale + pos.z;
        }

        if (checkAABBCollision(newBounds, cam.getCamPos()))
            return true;
    }

    return false;
}