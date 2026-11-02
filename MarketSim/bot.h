#pragma once
#include "svbmath.h"
#include "camera.h"
#include "drone.h"
#include "building.h"
#include "sound.h"
#include "model.h"

enum class BotState {
    GoingToShelf,
    GoingToCash,
    Leaving
};

struct PurchasedItem {
    float basePrice;
    float sellPrice;
};

class Bot {
private:
    svbmath::Vec3 position;
    svbmath::Vec3 shelfApproachTarget;
    svbmath::Vec3 targetShelf;
    svbmath::Vec3 cashTarget;

    std::string droneModel;

    BotState state = BotState::GoingToShelf;

    OBJModel model;
    
    float yaw = 0.0f;
    float speed = 3.5f;

    bool carryingDrone = false;

    bool active = false;

    bool findDrone(FPVDrone& drone);
    bool takeDrone(FPVDrone& drone);

    void moveTo(const svbmath::Vec3& target,
        float dt,
        Building& building);

public:
    void init(GLFWwindow* win);

    bool checkCollision(const svbmath::Vec3& newPos, Building& building);
    void update(float dt, FPVDrone& drone, Building& building, Sound& sound);

    void draw();

    bool isActive() const;

    float calculateRating(const std::vector<PurchasedItem>& items);
    void spawn(FPVDrone& drone, Building& building);
};