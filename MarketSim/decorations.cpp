#include "bounds.h"
#include "decorations.h"
#include "model.h"
#include "svbmath.h"
#include "camera.h"
#include <vector>

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

        bufferPos.z = 15.0f;

        if (i < 3) bufferPos.x += buildXStep * (i + 1);
        else  bufferPos.x -= buildXStep * (i - 2);

        allPositions.push_back(bufferPos);
    }
}
void Decorations::initDecoration(std::string modelPath, DecType decType, 
	std::string texturePath) {
	models.emplace(decType, new OBJModel());

	models[decType]->LoadOBJ(modelPath, 0.5f);
	if (texturePath != "") models[decType]->LoadTexture(texturePath);

	models[decType]->CreateVBO();
}
void Decorations::drawAllDecorations(DecType decType) {
    float yaw;

    switch (decType) {
    case DecType::BUILD1:yaw = 90.0f;
    case DecType::BUILD2: yaw = 0.0f;
    }

    for (int i = 0; i < buildsCount * 2; i++) models[decType]->Draw(allPositions[i], yaw, 0.0f);
}

bool Decorations::checkCollision(Camera& cam, DecType decType){
    auto mod = models[decType];

    for (int i = 0; i < buildsCount * 2; i++){
        svbmath::Vec3 pos = allPositions[i];

        Bounds newBounds = mod->bounds;

        newBounds.min.x *= mod->scale;
        newBounds.min.y *= mod->scale;
        newBounds.min.z *= mod->scale;

        newBounds.max.x *= mod->scale;
        newBounds.max.y *= mod->scale;
        newBounds.max.z *= mod->scale;

        newBounds.min.x += pos.x;
        newBounds.min.y += pos.y;
        newBounds.min.z += pos.z;

        newBounds.max.x += pos.x;
        newBounds.max.y += pos.y;
        newBounds.max.z += pos.z;

        if (checkAABBCollision(newBounds, cam.getCamPos())) return true;
    }

    return false;
}