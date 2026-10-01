#include "bounds.h"
#include "decorations.h"
#include "model.h"
#include "svbmath.h"
#include "camera.h"
#include <vector>

std::map<DecType, float> scales = { {DecType::BUILD1,1.0f},
                                   {DecType::BUILD2, 0.25},
                                    {DecType::TREE, 0.25} 
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

    for (int i = 0; i < treesCount; i++) {
        svbmath::Vec3 bufferPos = currentPos;

        bufferPos.z = 18.0f;

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

    float yaw;

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
    }
    if (decType != DecType::TREE) {
        for (int i = 0; i < buildsCount * 2; i++) {
            auto pos = allPositions[i];

            if (decType == DecType::BUILD2) pos.y -= 2.0f;

            models[decType]->Draw(pos, yaw, 0.0f);
        }
    }
    else {
        for (int i = buildsCount * 2; i < buildsCount * 2 + treesCount; i++) {
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
        startIndex = buildsCount * 2;
        count = treesCount;
    }
    else {
        startIndex = 0;
        count = buildsCount * 2;
    }

    for (int i = startIndex; i < startIndex + count; i++) {

        svbmath::Vec3 pos = allPositions[i];

        Bounds newBounds = mod->bounds;

        newBounds.min.x *= mod->scale;
        newBounds.min.y *= mod->scale;
        newBounds.min.z *= mod->scale;

        newBounds.max.x *= mod->scale;
        newBounds.max.y *= mod->scale;
        newBounds.max.z *= mod->scale;

        newBounds.min.x += pos.x;
        newBounds.min.y += pos.y - step;
        newBounds.min.z += pos.z;

        newBounds.max.x += pos.x;
        newBounds.max.y += pos.y - step;
        newBounds.max.z += pos.z;

        if (checkAABBCollision(newBounds, cam.getCamPos()))
            return true;
    }

    return false;
}