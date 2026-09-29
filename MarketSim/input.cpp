#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "input.h"
#include <cmath>
#include "camera.h"

inline bool isDown(GLFWwindow* w, int key) {
    return glfwGetKey(w, key) == GLFW_PRESS;
}
inline bool isPressed(GLFWwindow* w, int key, bool& prev) {
    bool curr = glfwGetKey(w, key) == GLFW_PRESS;
    bool result = curr && !prev;
    prev = curr;
    return result;
}

void processInput(GLFWwindow* window, Camera& cam, float dt) {
    if (isDown(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

    cam.getOldCamPos() = cam.getCamPos();

    auto& params = cam.getCamParams();
    auto& pos = cam.getCamPos();

    const float moveSpeed = 10.0f;
    const float rotateSpeed = 60.0f;

    float yawRad = params.cameraYaw * 3.1415926f / 180.0f;

    float forwardX = sin(yawRad);
    float forwardZ = cos(yawRad);

    float rightX = cos(yawRad);
    float rightZ = -sin(yawRad);

    if (isDown(window, GLFW_KEY_W)) {
        pos.x += forwardX * moveSpeed * dt;
        pos.z += forwardZ * moveSpeed * dt;
    }

    if (isDown(window, GLFW_KEY_S)) {
        pos.x -= forwardX * moveSpeed * dt;
        pos.z -= forwardZ * moveSpeed * dt;
    }

    if (isDown(window, GLFW_KEY_A)) {
        pos.x += rightX * moveSpeed * dt;
        pos.z += rightZ * moveSpeed * dt;
    }

    if (isDown(window, GLFW_KEY_D)) {
        pos.x -= rightX * moveSpeed * dt;
        pos.z -= rightZ * moveSpeed * dt;
    }

    if (isDown(window, GLFW_KEY_Q))
        pos.y -= moveSpeed * dt;

    if (isDown(window, GLFW_KEY_E))
        pos.y += moveSpeed * dt;

    if (isDown(window, GLFW_KEY_KP_6)) params.cameraYaw -= rotateSpeed * dt;
    if (isDown(window, GLFW_KEY_KP_4)) params.cameraYaw += rotateSpeed * dt;
    if (isDown(window, GLFW_KEY_KP_8)) params.cameraPitch += rotateSpeed * dt;
    if (isDown(window, GLFW_KEY_KP_2)) params.cameraPitch -= rotateSpeed * dt;


}