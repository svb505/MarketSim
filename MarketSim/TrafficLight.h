#pragma once

#include "svbmath.h"

enum class TrafficLightState {
    RED,
    RED_YELLOW,
    GREEN,
    YELLOW
};

class TrafficLight {
private:
    TrafficLightState state = TrafficLightState::RED;

    float timer = 0.0f;

public:
    void Update(float deltaTime);

    TrafficLightState GetState() const {
        return state;
    }

    void Reset() {
        state = TrafficLightState::RED;
        timer = 0.0f;
    }
};