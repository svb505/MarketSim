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

Camera cam;
Building build;

inline bool isDown(GLFWwindow* w, int key) {
    return glfwGetKey(w, key) == GLFW_PRESS;
}
inline bool isPressed(GLFWwindow* w, int key, bool& prev) {
    bool curr = glfwGetKey(w, key) == GLFW_PRESS;
    bool result = curr && !prev;
    prev = curr;
    return result;
}

void setMatrix(float left, float right, float bottom, float top, float nearP, float farP) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(left, right, bottom, top, nearP, farP);

    glMatrixMode(GL_MODELVIEW);
}

void processInput(GLFWwindow* window, float dt){
    if (isDown(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
    if (isDown(window, GLFW_KEY_W)) cam.getCamPos().z += 10 * dt;
    if (isDown(window, GLFW_KEY_A)) cam.getCamPos().x += 10 * dt;
    if (isDown(window, GLFW_KEY_S)) cam.getCamPos().z -= 10 * dt;
    if (isDown(window, GLFW_KEY_D)) cam.getCamPos().x -= 10 * dt;

    if (isDown(window, GLFW_KEY_Q)) cam.getCamPos().y -= 10 * dt;
    if (isDown(window, GLFW_KEY_E)) cam.getCamPos().y += 10 * dt;

    if (isDown(window, GLFW_KEY_KP_6)) cam.getCamParams().cameraYaw -= 30 * dt;
    if (isDown(window, GLFW_KEY_KP_4)) cam.getCamParams().cameraYaw += 30 * dt;
    if (isDown(window, GLFW_KEY_KP_8)) cam.getCamParams().cameraPitch += 30 * dt;
    if (isDown(window, GLFW_KEY_KP_2)) cam.getCamParams().cameraPitch -= 30 * dt;

}
int main(){
    CameraParams& params = cam.getCamParams();

    if (!glfwInit()){
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 16);
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Example", NULL, NULL);
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

        processInput(window, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cam.setupCamera();

        drawSky();
        drawGround(cam.getCamPos().x, cam.getCamPos().z);
        
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2.0f, -2.0f);

        build.DrawBuilding();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

