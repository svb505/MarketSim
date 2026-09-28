#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif
#include <windows.h>
#include <GL/gl.h>
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

Camera cam;
Building build;

void setMatrix(float left, float right, float bottom, float top, float nearP, float farP) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(left, right, bottom, top, nearP, farP);

    glMatrixMode(GL_MODELVIEW);
}

int main(){
    CameraParams& params = cam.getCamParams();

    if (!glfwInit()){
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 16);
    GLFWwindow* window = glfwCreateWindow(ECRAN_W, ECRAN_H, "3D Example", NULL, NULL);
    if (!window){
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, &cam);
    glfwMakeContextCurrent(window);

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
    //LoadAllTextures();

    build.CreateBuilding();

    while (!glfwWindowShouldClose(window)){
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        processInput(window, cam, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cam.setupCamera();

        drawSky();
        drawGround(cam.getCamPos().x, cam.getCamPos().z);
        
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2.0f, -2.0f);

        build.DrawBuilding();
        if (build.checkCollision(cam)) {
            cam.getCamPos() = cam.getOldCamPos();
        }
      

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

