#pragma once
#include "camera.h"
#include "drone.h"
#include "svbmath.h"

inline bool isDown(GLFWwindow* w, int key);
inline bool isPressed(GLFWwindow* w, int key, bool& prev);

void processInput(GLFWwindow* window, Camera& cam, FPVDrone& drone, float dt,
	svbmath::Vec3 point);