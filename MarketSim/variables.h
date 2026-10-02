#pragma once

struct FPS {
    double deltaTime = 0.0;
    float fpsTimer = 0.0f, fps = 0.0f, minFps = 0.0f, maxFps = 0.0f;
    int frames = 0;
};

extern int ECRAN_W;
extern int ECRAN_H;
extern float sensitivity;