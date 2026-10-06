#include "bot.h"
#include <cmath>
#include <iostream>
#include <algorithm>

#include "variables.h"
#include "database.h"
#include "renders.h"


// ============================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================================

static bool CircleIntersectsAABB(
    const svbmath::Vec3& pos,
    float radius,
    float minX,
    float maxX,
    float minZ,
    float maxZ)
{
    float closestX =
        std::max(minX, std::min(pos.x, maxX));

    float closestZ =
        std::max(minZ, std::min(pos.z, maxZ));

    float dx = pos.x - closestX;
    float dz = pos.z - closestZ;

    return dx * dx + dz * dz <= radius * radius;
}


static bool InRadiusXZ(
    const svbmath::Vec3& a,
    const svbmath::Vec3& b,
    float radius)
{
    float dx = a.x - b.x;
    float dz = a.z - b.z;

    return dx * dx + dz * dz <= radius * radius;
}


// ============================================================
// ПОИСК ТОЧКИ ПОДХОДА К ПОЛКЕ
// ============================================================

static svbmath::Vec3 GetApproachPoint(
    const svbmath::Vec3& target,
    Building& building)
{
    const float margin = 0.55f;

    const auto& shelves = building.getShelves();

    for (const auto& shelf : shelves)
    {
        // Проверяем, находится ли дрон внутри AABB этой полки

        if (target.x >= shelf.min.x &&
            target.x <= shelf.max.x &&
            target.z >= shelf.min.z &&
            target.z <= shelf.max.z)
        {
            float leftDistance =
                std::abs(target.x - shelf.min.x);

            float rightDistance =
                std::abs(shelf.max.x - target.x);

            float frontDistance =
                std::abs(target.z - shelf.min.z);

            float backDistance =
                std::abs(shelf.max.z - target.z);

            float minDistance = std::min({
                leftDistance,
                rightDistance,
                frontDistance,
                backDistance
                });

            svbmath::Vec3 result = target;

            // Слева
            if (minDistance == leftDistance)
            {
                result.x =
                    shelf.min.x - margin;
            }

            // Справа
            else if (minDistance == rightDistance)
            {
                result.x =
                    shelf.max.x + margin;
            }

            // Спереди
            else if (minDistance == frontDistance)
            {
                result.z =
                    shelf.min.z - margin;
            }

            // Сзади
            else
            {
                result.z =
                    shelf.max.z + margin;
            }

            return result;
        }
    }

    // Если дрон почему-то не находится
    // внутри ни одной полки
    return target;
}


// ============================================================
// COLLISION
// ============================================================

bool Bot::checkCollision(
    const svbmath::Vec3& newPos,
    Building& building)
{
    const float botRadius = 0.35f;


    // ========================================================
    // КАССА
    // ========================================================

    const auto& cash =
        building.getCashRegister();

    if (CircleIntersectsAABB(
        newPos,
        botRadius,
        cash.min.x,
        cash.max.x,
        cash.min.z,
        cash.max.z))
    {
        return true;
    }


    // ========================================================
    // ПОЛКИ
    // ========================================================

    const auto& shelves =
        building.getShelves();

    for (const auto& shelf : shelves)
    {
        if (CircleIntersectsAABB(
            newPos,
            botRadius,
            shelf.min.x,
            shelf.max.x,
            shelf.min.z,
            shelf.max.z))
        {
            return true;
        }
    }


    return false;
}


// ============================================================
// ACTIVE
// ============================================================

bool Bot::isActive() const
{
    return active;
}


// ============================================================
// ПОИСК ДРОНА
// ============================================================

bool Bot::findDrone(FPVDrone& drone)
{
    auto& objects =
        drone.getPlacedEquip();

    for (const auto& obj : objects)
    {
        if (obj.name !=
            getStringFromJson("DRONE_Achetee") &&

            obj.name !=
            getStringFromJson("DRONE_Assemblee") &&

            obj.name !=
            drone.getBaseEquip()[5] &&

            obj.name !=
            drone.getBaseEquip()[6])
        {
            continue;
        }


        targetShelf = obj.pos;

        droneModel = obj.name;

        return true;
    }

    return false;
}


// ============================================================
// SPAWN
// ============================================================

void Bot::spawn(
    FPVDrone& drone,
    Building& building)
{
    if (active || !isOpen)
        return;


    // Ищем дрон
    if (!findDrone(drone))
        return;


    // Начальная позиция бота
    position = {
        0.0f,
        0.0f,
        4.5f
    };


    // Находим точку,
    // с которой нужно подойти к полке

    shelfApproachTarget =
        GetApproachPoint(
            targetShelf,
            building);


    // Касса

    const auto& cash =
        building.getCashRegister();

    cashTarget = cash.position;

    cashTarget.z =
        cash.min.z - 0.4f;


    state =
        BotState::GoingToShelf;


    carryingDrone = false;

    active = true;


    std::cout
        << "BOT SPAWNED\n";

    std::cout
        << "Target drone: "
        << droneModel
        << "\n";

    std::cout
        << "Drone position: "
        << targetShelf.x
        << " "
        << targetShelf.z
        << "\n";

    std::cout
        << "Approach position: "
        << shelfApproachTarget.x
        << " "
        << shelfApproachTarget.z
        << "\n";
}


// ============================================================
// MOVEMENT
// ============================================================

void Bot::moveTo(
    const svbmath::Vec3& target,
    float dt,
    Building& building)
{
    svbmath::Vec3 direction = {
        target.x - position.x,
        0.0f,
        target.z - position.z
    };


    float distance =
        std::sqrt(
            direction.x * direction.x +
            direction.z * direction.z
        );


    if (distance < 0.01f)
        return;


    direction.x /= distance;
    direction.z /= distance;


    float moveDistance =
        speed * dt;


    // Максимальный размер одного шага.
    // Это предотвращает прохождение через тонкую коллизию.

    const float maxStep = 0.04f;


    int steps =
        static_cast<int>(
            std::ceil(
                moveDistance / maxStep
            )
            );


    if (steps < 1)
        steps = 1;


    float stepDistance =
        moveDistance / steps;


    for (int i = 0; i < steps; ++i)
    {
        bool moved = false;


        // ====================================================
        // 1. ПРЯМО
        // ====================================================

        svbmath::Vec3 newPos =
            position;

        newPos.x +=
            direction.x * stepDistance;

        newPos.z +=
            direction.z * stepDistance;


        if (!checkCollision(
            newPos,
            building))
        {
            position = newPos;

            moved = true;
        }


        if (moved)
            continue;


        // ====================================================
        // 2. ТОЛЬКО X
        // ====================================================

        newPos = position;

        newPos.x +=
            direction.x * stepDistance;


        if (!checkCollision(
            newPos,
            building))
        {
            position.x =
                newPos.x;

            moved = true;
        }


        if (moved)
            continue;


        // ====================================================
        // 3. ТОЛЬКО Z
        // ====================================================

        newPos = position;

        newPos.z +=
            direction.z * stepDistance;


        if (!checkCollision(
            newPos,
            building))
        {
            position.z =
                newPos.z;

            moved = true;
        }


        if (moved)
            continue;


        // ====================================================
        // 4. ВЛЕВО ОТ НАПРАВЛЕНИЯ
        // ====================================================

        newPos = position;

        newPos.x +=
            -direction.z * stepDistance;

        newPos.z +=
            direction.x * stepDistance;


        if (!checkCollision(
            newPos,
            building))
        {
            position = newPos;

            moved = true;
        }


        if (moved)
            continue;


        // ====================================================
        // 5. ВПРАВО ОТ НАПРАВЛЕНИЯ
        // ====================================================

        newPos = position;

        newPos.x +=
            direction.z * stepDistance;

        newPos.z +=
            -direction.x * stepDistance;


        if (!checkCollision(
            newPos,
            building))
        {
            position = newPos;

            moved = true;
        }


        // Если все варианты заблокированы,
        // этот маленький шаг пропускаем.
    }
}


// ============================================================
// TAKE DRONE
// ============================================================

bool Bot::takeDrone(
    FPVDrone& drone)
{
    auto& objects =
        drone.getPlacedEquip();


    // Бот должен находиться рядом с дроном,
    // но не обязан стоять ровно в его координате.

    const float takeRadius = 1.3f;


    for (auto it = objects.begin();
        it != objects.end();
        ++it)
    {
        if (it->name != droneModel)
            continue;


        if (!InRadiusXZ(
            it->pos,
            position,
            takeRadius))
        {
            continue;
        }


        droneModel =
            it->name;


        // Удаляем дрон из размещённого оборудования

        objects.erase(it);


        // Освобождаем место на полке

        auto& shelfPositions =
            drone.getShelfPositions();


        for (auto& shelf :
            shelfPositions)
        {
            if (InRadiusXZ(
                shelf.pos,
                targetShelf,
                0.2f))
            {
                shelf.busy = false;

                break;
            }
        }


        carryingDrone = true;


        std::cout
            << "BOT TOOK: "
            << droneModel
            << "\n";


        return true;
    }


    std::cout
        << "BOT FAILED TO TAKE DRONE\n";


    return false;
}


// ============================================================
// UPDATE
// ============================================================

void Bot::update(
    float dt,
    FPVDrone& drone,
    Building& building)
{
    if (!active || !isOpen)
        return;


    switch (state)
    {
        // ====================================================
        // ИДЁМ К ПОЛКЕ
        // ====================================================

    case BotState::GoingToShelf:
    {
        moveTo(
            shelfApproachTarget,
            dt,
            building
        );


        if (InRadiusXZ(
            position,
            shelfApproachTarget,
            0.45f))
        {
            if (takeDrone(drone))
            {
                state =
                    BotState::GoingToCash;


                std::cout
                    << "BOT GOING TO CASH\n";
            }
        }

        break;
    }


    // ====================================================
    // ИДЁМ К КАССЕ
    // ====================================================

    case BotState::GoingToCash:
    {
        moveTo(
            cashTarget,
            dt,
            building
        );


        if (InRadiusXZ(
            position,
            cashTarget,
            0.5f))
        {
            float price = 0.0f;


            if (droneModel ==
                getStringFromJson(
                    "DRONE_Achetee"))
            {
                price =
                    priceForT1;
            }

            else if (
                droneModel ==
                getStringFromJson(
                    "DRONE_Assemblee"))
            {
                price =
                    priceForT2;
            }

            else if (
                droneModel ==
                drone.getBaseEquip()[5])
            {
                price =
                    priceForController;
            }

            else if (
                droneModel ==
                drone.getBaseEquip()[6])
            {
                price =
                    priceForGoggles;
            }


            myBalance +=
                price;


            saveDataForPlayer(
                price,
                "+"
            );


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


            state =
                BotState::Leaving;
        }

        break;
    }


    // ====================================================
    // УХОДИМ
    // ====================================================

    case BotState::Leaving:
    {
        active = false;

        std::cout
            << "BOT LEFT\n";

        break;
    }
    }
}


// ============================================================
// DRAW
// ============================================================

void Bot::draw()
{
    if (!active || !isOpen)
        return;


    glPushMatrix();


    glTranslatef(
        position.x,
        position.y + 0.8f,
        position.z
    );


    glColor3f(
        0.2f,
        0.4f,
        0.8f
    );


    glBegin(GL_QUADS);


    // ========================================================
    // FRONT
    // ========================================================

    glVertex3f(
        -0.3f,
        -0.8f,
        0.2f
    );

    glVertex3f(
        0.3f,
        -0.8f,
        0.2f
    );

    glVertex3f(
        0.3f,
        0.8f,
        0.2f
    );

    glVertex3f(
        -0.3f,
        0.8f,
        0.2f
    );


    // ========================================================
    // BACK
    // ========================================================

    glVertex3f(
        0.3f,
        -0.8f,
        -0.2f
    );

    glVertex3f(
        -0.3f,
        -0.8f,
        -0.2f
    );

    glVertex3f(
        -0.3f,
        0.8f,
        -0.2f
    );

    glVertex3f(
        0.3f,
        0.8f,
        -0.2f
    );


    // ========================================================
    // LEFT
    // ========================================================

    glVertex3f(
        -0.3f,
        -0.8f,
        -0.2f
    );

    glVertex3f(
        -0.3f,
        -0.8f,
        0.2f
    );

    glVertex3f(
        -0.3f,
        0.8f,
        0.2f
    );

    glVertex3f(
        -0.3f,
        0.8f,
        -0.2f
    );


    // ========================================================
    // RIGHT
    // ========================================================

    glVertex3f(
        0.3f,
        -0.8f,
        0.2f
    );

    glVertex3f(
        0.3f,
        -0.8f,
        -0.2f
    );

    glVertex3f(
        0.3f,
        0.8f,
        -0.2f
    );

    glVertex3f(
        0.3f,
        0.8f,
        0.2f
    );


    // ========================================================
    // TOP
    // ========================================================

    glVertex3f(
        -0.3f,
        0.8f,
        0.2f
    );

    glVertex3f(
        0.3f,
        0.8f,
        0.2f
    );

    glVertex3f(
        0.3f,
        0.8f,
        -0.2f
    );

    glVertex3f(
        -0.3f,
        0.8f,
        -0.2f
    );


    // ========================================================
    // BOTTOM
    // ========================================================

    glVertex3f(
        -0.3f,
        -0.8f,
        -0.2f
    );

    glVertex3f(
        0.3f,
        -0.8f,
        -0.2f
    );

    glVertex3f(
        0.3f,
        -0.8f,
        0.2f
    );

    glVertex3f(
        -0.3f,
        -0.8f,
        0.2f
    );


    glEnd();


    glPopMatrix();
}