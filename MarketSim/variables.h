#pragma once
#include <string>

struct FPS {
    double deltaTime = 0.0;
    float fpsTimer = 0.0f, fps = 0.0f, minFps = 0.0f, maxFps = 0.0f;
    int frames = 0;
};

extern std::string objName;
extern float myBalance;
extern int ECRAN_W;
extern int ECRAN_H;
extern float sensitivity;
extern bool hideCursor;
extern bool canTakeObj;
extern bool canDrawEquipModel;
extern bool placeObj;
extern bool canCreateDrone;
extern float priceForT1;
extern float priceForT2;
extern float priceForController;
extern float priceForGoggles;
extern bool isOpen;