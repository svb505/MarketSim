#include "bot.h"
#include <cmath>
#include <iostream>
#include "variables.h"
#include "database.h"
#include "renders.h"
#include "database.h"

static bool InRadiusXZ(const svbmath::Vec3& a, const svbmath::Vec3& b, float radius){
    float dx = a.x - b.x;
    float dz = a.z - b.z;

    return dx * dx + dz * dz <= radius * radius;
}

bool Bot::isActive() const {
    return active;
}

bool Bot::findDrone(FPVDrone& drone) {

    auto& objects = drone.getPlacedEquip();

    for (const auto& obj : objects) {

        if (obj.name != "FPV Drone T1" &&
            obj.name != "FPV Drone T2")
            continue;

        targetShelf = obj.pos;
        droneModel = obj.name;

        return true;
    }

    return false;
}

void Bot::spawn(FPVDrone& drone, Building& building) {

    if (active) return;

    if (!findDrone(drone)) return;

    position = { 0.0f, 0.0f, 4.5f };

    cashTarget = building.getCashRegister().position;

    cashTarget.z += 0.8f;

    state = BotState::GoingToShelf;

    carryingDrone = false;
    active = true;

    std::cout << "BOT SPAWNED\n";
    std::cout << "Target drone: " << droneModel << "\n";
}

void Bot::moveTo(const svbmath::Vec3& target, float dt) {

    svbmath::Vec3 direction = { target.x - position.x, 0.0f, target.z - position.z};

    float distance = std::sqrt( direction.x * direction.x + direction.z * direction.z);

    if (distance < 0.05f)
        return;

    direction.x /= distance;
    direction.z /= distance;

    position.x += direction.x * speed * dt;
    position.z += direction.z * speed * dt;
}

void Bot::takeDrone(FPVDrone& drone) {

    auto& objects = drone.getPlacedEquip();

    bool taken = false;

    for (auto it = objects.begin(); it != objects.end(); ++it) {

        if (it->name != droneModel) continue;

        if (!InRadiusXZ(it->pos,targetShelf,0.05f)) continue;

        droneModel = it->name;

        objects.erase(it);

        taken = true;

        break;
    }

    if (!taken) {
        std::cout << "BOT FAILED TO TAKE DRONE\n";
        return;
    }

    auto& shelfPositions = drone.getShelfPositions();

    for (auto& shelf : shelfPositions) {

        if (InRadiusXZ(shelf.pos, targetShelf, 0.05f)) {
            shelf.busy = false;
            break;
        }
    }

    carryingDrone = true;

    std::cout << "BOT TOOK: " << droneModel << "\n";
}

void Bot::update( float dt, FPVDrone& drone, Building& building){
    if (!active)
        return;

    switch (state) {

    case BotState::GoingToShelf:

        moveTo(targetShelf, dt);

        if (InRadiusXZ(
            position,
            targetShelf,
            0.4f))
        {
            takeDrone(drone);

            state = BotState::GoingToCash;

            std::cout << "BOT GOING TO CASH\n";
        }

        break;


    case BotState::GoingToCash:
        moveTo(cashTarget, dt);

        if (InRadiusXZ(position, cashTarget, 0.5f)){
            float price;

            if (droneModel == "FPV Drone T1") price = priceForT1;
            else price = priceForT2;

            myBalance += price;

            saveDataForPlayer(price, "+");

            std::cout
                << "SOLD: "
                << droneModel
                << " for "
                << price
                << "\n";

            std::cout
                << "Balance: "
                << myBalance
                << "\n";

            state = BotState::Leaving;
        }

        break;


    case BotState::Leaving:
        active = false;

        std::cout << "BOT LEFT\n";

        break;
    }
}

void Bot::draw() {

    if (!active)
        return;

    glPushMatrix();

    glTranslatef(position.x,position.y + 0.8f,position.z);

    glColor3f(0.2f, 0.4f, 0.8f);

    glBegin(GL_QUADS);

    // Front
    glVertex3f(-0.3f, -0.8f, 0.2f);
    glVertex3f(0.3f, -0.8f, 0.2f);
    glVertex3f(0.3f, 0.8f, 0.2f);
    glVertex3f(-0.3f, 0.8f, 0.2f);

    // Back
    glVertex3f(0.3f, -0.8f, -0.2f);
    glVertex3f(-0.3f, -0.8f, -0.2f);
    glVertex3f(-0.3f, 0.8f, -0.2f);
    glVertex3f(0.3f, 0.8f, -0.2f);

    // Left
    glVertex3f(-0.3f, -0.8f, -0.2f);
    glVertex3f(-0.3f, -0.8f, 0.2f);
    glVertex3f(-0.3f, 0.8f, 0.2f);
    glVertex3f(-0.3f, 0.8f, -0.2f);

    // Right
    glVertex3f(0.3f, -0.8f, 0.2f);
    glVertex3f(0.3f, -0.8f, -0.2f);
    glVertex3f(0.3f, 0.8f, -0.2f);
    glVertex3f(0.3f, 0.8f, 0.2f);

    // Top
    glVertex3f(-0.3f, 0.8f, 0.2f);
    glVertex3f(0.3f, 0.8f, 0.2f);
    glVertex3f(0.3f, 0.8f, -0.2f);
    glVertex3f(-0.3f, 0.8f, -0.2f);

    // Bottom
    glVertex3f(-0.3f, -0.8f, -0.2f);
    glVertex3f(0.3f, -0.8f, -0.2f);
    glVertex3f(0.3f, -0.8f, 0.2f);
    glVertex3f(-0.3f, -0.8f, 0.2f);

    glEnd();

    glPopMatrix();
}