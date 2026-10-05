#include "bot.h"
#include <cmath>
#include <iostream>
#include "variables.h"
#include "database.h"
#include "renders.h"
#include "database.h"

static bool CircleIntersectsAABB(const svbmath::Vec3& pos, float radius, float minX, float maxX, float minZ, float maxZ) {
    float closestX = std::max(minX, std::min(pos.x, maxX));
    float closestZ = std::max(minZ, std::min(pos.z, maxZ));

    float dx = pos.x - closestX;
    float dz = pos.z - closestZ;

    return dx * dx + dz * dz <= radius * radius;
}
bool Bot::checkCollision(const svbmath::Vec3& newPos, Building& building){
    const float botRadius = 0.35f;
    auto cash = building.getCashRegister();

    const float cashHalfX = 0.7f;
    const float cashHalfZ = 0.5f;

    if (CircleIntersectsAABB( newPos, botRadius, cash.position.x - cashHalfX, cash.position.x + cashHalfX, cash.position.z - cashHalfZ, cash.position.z + cashHalfZ)){
        return true;
    }

    auto& shelves = building.getShelves();

    for (const auto& shelf : shelves){
        const float shelfHalfX = 0.8f;
        const float shelfHalfZ = 0.35f;

        if (CircleIntersectsAABB(newPos,botRadius, shelf.position.x - shelfHalfX, shelf.position.x + shelfHalfX,shelf.position.z - shelfHalfZ,
            shelf.position.z + shelfHalfZ)){
            return true;
        }
    }

    return false;
}

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

        if (obj.name != getStringFromJson("DRONE_Achetee") &&
            obj.name != getStringFromJson("DRONE_Assemblee") && 
            obj.name != drone.getBaseEquip()[5] && obj.name != drone.getBaseEquip()[6])
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

    cashTarget.z += 1.3f;

    state = BotState::GoingToShelf;

    carryingDrone = false;
    active = true;

    std::cout << "BOT SPAWNED\n";
    std::cout << "Target drone: " << droneModel << "\n";
}

void Bot::moveTo(const svbmath::Vec3& target,float dt,Building& building){
    svbmath::Vec3 direction = {
        target.x - position.x,
        0.0f,
        target.z - position.z
    };

    float distance = std::sqrt(
        direction.x * direction.x +
        direction.z * direction.z
    );

    if (distance < 0.05f)
        return;

    direction.x /= distance;
    direction.z /= distance;

    float moveDistance = speed * dt;

    svbmath::Vec3 newPos = position;

    newPos.x += direction.x * moveDistance;
    newPos.z += direction.z * moveDistance;

    // Не разрешаем проходить сквозь препятствие
    if (!checkCollision(newPos, building))
    {
        position = newPos;
        return;
    }

    // Если вперед нельзя — пробуем двигаться только по X
    newPos = position;
    newPos.x += direction.x * moveDistance;

    if (!checkCollision(newPos, building))
    {
        position.x = newPos.x;
        return;
    }

    // Если по X нельзя — пробуем только по Z
    newPos = position;
    newPos.z += direction.z * moveDistance;

    if (!checkCollision(newPos, building))
    {
        position.z = newPos.z;
        return;
    }

    // Если оба направления заблокированы —
    // остаёмся на месте
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

        moveTo(targetShelf, dt, building);

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
        moveTo(cashTarget, dt, building);

        if (InRadiusXZ(position, cashTarget, 0.5f)){
            float price  = 0.0f;

            if (droneModel == getStringFromJson("DRONE_Achetee")) price = priceForT1;
            else if (droneModel == getStringFromJson("DRONE_Assemblee")) price = priceForT2;
            else if (droneModel == drone.getBaseEquip()[5]) price = priceForController;
            else if (droneModel == drone.getBaseEquip()[6]) price = priceForGoggles;

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