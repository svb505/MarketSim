#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif
#include <windows.h>
#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include "camera.h"
#include "text.h"
#include "texture.h"
#include "lighting.h"
#include "environnement.h"
#include "building.h"
#include "bounds.h"
#include "variables.h"
#include "input.h"
#include "model.h"
#include <algorithm>
#include "decorations.h"

Camera cam;
Building build;
OBJModel base;
Decorations decor;

void setMatrix(float left, float right, float bottom, float top, float nearP, float farP) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(left, right, bottom, top, nearP, farP);

    glMatrixMode(GL_MODELVIEW);
}
void countFps(double& deltaTime, double& lastTime, double& currentTime, int& frames, float& fps, float& fpsTimer) {
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    fpsTimer += deltaTime;
    frames++;

    if (fpsTimer >= 1.0f) {
        fps = frames / fpsTimer;
        frames = 0;
        fpsTimer = 0.0f;
    }
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    CameraParams& params = cam.getCamParams();

    static bool first = true;
    static double lastX = 0.0;
    static double lastY = 0.0;

    if (first) {
        lastX = xpos;
        lastY = ypos;
        first = false;
        return;
    }

    double xoffset = xpos - lastX;
    double yoffset = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    params.cameraYaw += -(float)xoffset;
    params.cameraPitch -= -(float)yoffset;
}
int main() {
    CameraParams& params = cam.getCamParams();

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 16);
    GLFWwindow* window = glfwCreateWindow(ECRAN_W, ECRAN_H, "FPV Market - V0.0.1", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, &cam);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD\n";

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);;
    BuildFont();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    setMatrix(params.left, params.right, params.bottom, params.top, params.nearPlane,
        params.farPlane);

    double lastTime = glfwGetTime();
    double deltaTime = 0.0;
    float fpsTimer = 0.0f, fps = 0.0f;
    int frames = 0;

    initLighting();
    LoadAllTextures();

    base.LoadOBJ("res/models/fpv/base.obj", 1.0f);
    base.LoadTexture("res/models/fpv/texture_diffuse.png");
    base.CreateVBO();

    build.CreateBuilding();

    decor.initDecoration("res/models/builds/build1.obj", DecType::BUILD1,
        "res/models/builds/tex.png");
    decor.initPositions();

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();

        countFps(deltaTime, lastTime, currentTime, frames, fps, fpsTimer);

        processInput(window, cam, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cam.setupCamera();

        drawSky();
        drawGround(cam.getCamPos().x, cam.getCamPos().z);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2.0f, -2.0f);

        build.DrawBuilding();
        if (build.checkCollision(cam)) cam.getCamPos() = cam.getOldCamPos();

        //base.Draw({0.0f, 0.5f,15.0f},0.0f,60.0f);

        decor.drawAllDecorations(DecType::BUILD1);
        if (decor.checkCollision(cam, DecType::BUILD1)) cam.getCamPos() = cam.getOldCamPos();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

