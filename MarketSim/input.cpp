#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "input.h"
#include <cmath>
#include "camera.h"
#include "variables.h"
#include "drone.h"
#include "svbmath.h"

inline bool isDown(GLFWwindow* w, int key) {
    return glfwGetKey(w, key) == GLFW_PRESS;
}
inline bool isPressed(GLFWwindow* w, int key, bool& prev) {
    bool curr = glfwGetKey(w, key) == GLFW_PRESS;
    bool result = curr && !prev;
    prev = curr;
    return result;
}

void processInput(GLFWwindow* window, Camera& cam, FPVDrone& drone,float dt,
    svbmath::Vec3 point) {
    cam.getOldCamPos() = cam.getCamPos();

    auto& params = cam.getCamParams();
    auto& pos = cam.getCamPos();

    const float moveSpeed = 10.0f;
    float currentSpeed = moveSpeed;
    const float rotateSpeed = 60.0f;

    float yawRad = params.cameraYaw * 3.1415926f / 180.0f;

    float forwardX = sin(yawRad);
    float forwardZ = cos(yawRad);

    float rightX = cos(yawRad);
    float rightZ = -sin(yawRad);

    static bool prevAlt = false;
    static bool prevE = false;

    bool pressE = isPressed(window, GLFW_KEY_E, prevE);

    if (pressE) {
        if (drawEquipModel && placeObj) {
            drone.addPlacedObj(objName, point);

            drawEquipModel = false;
            objName.clear();
            placeObj = false;
        }
        else if (!drawEquipModel && canTakeObj) {
            auto& boxes = drone.getOrderEquip();
            int& index = drone.getCurrentIndex();

            for (auto& box : boxes) {
                if (!box.render)
                    continue;

                if (index >= 0 &&
                    index < static_cast<int>(box.equip.size())) {
                    objName = box.equip[index];
                    ++index;

                    drawEquipModel = true;
                    break;
                }
            }
        }
    }
    if (isPressed(window, GLFW_KEY_LEFT_ALT, prevAlt)) hideCursor = !hideCursor;
    if (isDown(window, GLFW_KEY_LEFT_SHIFT)) currentSpeed *= 2;
    if (isDown(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
    if (isDown(window, GLFW_KEY_W)) {
        pos.x += forwardX * currentSpeed * dt;
        pos.z += forwardZ * currentSpeed * dt;
    }
    if (isDown(window, GLFW_KEY_S)) {
        pos.x -= forwardX * currentSpeed * dt;
        pos.z -= forwardZ * currentSpeed * dt;
    }
    if (isDown(window, GLFW_KEY_A)) {
        pos.x += rightX * currentSpeed * dt;
        pos.z += rightZ * currentSpeed * dt;
    }
    if (isDown(window, GLFW_KEY_D)) {
        pos.x -= rightX * currentSpeed * dt;
        pos.z -= rightZ * currentSpeed * dt;
    }
}