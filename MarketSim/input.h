#pragma once
#include "camera.h"

inline bool isDown(GLFWwindow* w, int key);
inline bool isPressed(GLFWwindow* w, int key, bool& prev);

void processInput(GLFWwindow* window, Camera& cam, float dt);