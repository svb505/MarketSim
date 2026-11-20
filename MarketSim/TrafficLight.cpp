#include "TrafficLight.h"

void TrafficLight::Update(float deltaTime) {
    timer += deltaTime;

    switch (state) {

    case TrafficLightState::RED:
        if (timer >= 5.0f) {
            state = TrafficLightState::RED_YELLOW;
            timer = 0.0f;
        }
        break;

    case TrafficLightState::RED_YELLOW:
        if (timer >= 2.0f) {
            state = TrafficLightState::GREEN;
            timer = 0.0f;
        }
        break;

    case TrafficLightState::GREEN:
        if (timer >= 5.0f) {
            state = TrafficLightState::YELLOW;
            timer = 0.0f;
        }
        break;

    case TrafficLightState::YELLOW:
        if (timer >= 2.0f) {
            state = TrafficLightState::RED;
            timer = 0.0f;
        }
        break;
    }
}