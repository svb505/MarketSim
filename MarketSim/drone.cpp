#include "drone.h"
#include "text.h"
#include <glad/glad.h>
#include "variables.h"
#include "building.h"
#include "svbRandom.h"
#include "renders.h"

const std::vector<std::string>& FPVDrone::getBaseEquip() {
    return baseEquipment;
}
const std::map<std::string, float>& FPVDrone::getEquipPrice() {
    return equipmentPrice;
}

void FPVDrone::drawAllBoxes() {
    for (auto& box : boxesWithEquip) drawBox(4.0f, 4.0f, 4.0F, box.pos);
}
void FPVDrone::sendPackage(std::vector<std::string>& equip, BuildingSettings& build) {
    float step = 0.2f;

    svbmath::Vec3 boxPos = { 0.0f,0.0f,0.0f };

    float minX = build.warehouseCenter.x - build.warehouseWidth / 2.0f;
    float maxX = build.warehouseCenter.x + build.warehouseWidth / 2.0f;

    float minZ = build.warehouseCenter.z - build.warehouseLength / 2.0f;
    float maxZ = build.warehouseCenter.z + build.warehouseLength / 2.0f;

    boxPos.x = returnRandomNum(minX + step, maxX - step);
    boxPos.z = returnRandomNum(minZ + step, maxZ - step);
    
    boxesWithEquip.push_back({equip, boxPos, false});
    inDelivery = true;
}
void FPVDrone::delivery(float dt) {
    if (inDelivery) {
        if (deliveryTime > 0.0f) deliveryTime -= dt;
        else {
            deliveryTime = 30.0f;
            inDelivery = false;
            showText = true;
            drawAllBoxes();
        }
    }
    if (showText) {
        if (textTime > 0.0f) textTime -= dt;
        else {
            textTime = 3.0f;
            showText = false;
        }

        RenderTextHUD(ECRAN_W / 2, ECRAN_H / 2, 0.0f, 0.0f, 0.0f,
            "Your package has arrived", ECRAN_W, ECRAN_H);
    }
}

void FPVDrone::drawCube(float sx, float sy, float sz) {
    sx /= 2.0f;
    sy /= 2.0f;
    sz /= 2.0f;

    glBegin(GL_QUADS);

    // Front (+Z)
    glNormal3f(0, 0, 1);
    glVertex3f(-sx, -sy, sz);
    glVertex3f(sx, -sy, sz);
    glVertex3f(sx, sy, sz);
    glVertex3f(-sx, sy, sz);

    // Back (-Z)
    glNormal3f(0, 0, -1);
    glVertex3f(sx, -sy, -sz);
    glVertex3f(-sx, -sy, -sz);
    glVertex3f(-sx, sy, -sz);
    glVertex3f(sx, sy, -sz);

    // Left (-X)
    glNormal3f(-1, 0, 0);
    glVertex3f(-sx, -sy, -sz);
    glVertex3f(-sx, -sy, sz);
    glVertex3f(-sx, sy, sz);
    glVertex3f(-sx, sy, -sz);

    // Right (+X)
    glNormal3f(1, 0, 0);
    glVertex3f(sx, -sy, sz);
    glVertex3f(sx, -sy, -sz);
    glVertex3f(sx, sy, -sz);
    glVertex3f(sx, sy, sz);

    // Top
    glNormal3f(0, 1, 0);
    glVertex3f(-sx, sy, sz);
    glVertex3f(sx, sy, sz);
    glVertex3f(sx, sy, -sz);
    glVertex3f(-sx, sy, -sz);

    // Bottom
    glNormal3f(0, -1, 0);
    glVertex3f(-sx, -sy, -sz);
    glVertex3f(sx, -sy, -sz);
    glVertex3f(sx, -sy, sz);
    glVertex3f(-sx, -sy, sz);

    glEnd();
}
void FPVDrone::drawCylinder(float radius, float height, int segments) {
    float half = height / 2.0f;

    // Боковая поверхность
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= segments; i++)
    {
        float a = 2.0f * 3.14159265f * i / segments;
        float x = cosf(a) * radius;
        float z = sinf(a) * radius;

        glNormal3f(x, 0, z);

        glVertex3f(x, -half, z);
        glVertex3f(x, half, z);
    }

    glEnd();

    // Верх
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f(0, half, 0);

    for (int i = 0; i <= segments; i++)
    {
        float a = 2.0f * 3.14159265f * i / segments;
        glVertex3f(
            cosf(a) * radius,
            half,
            sinf(a) * radius
        );
    }

    glEnd();

    // Низ
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glVertex3f(0, -half, 0);

    for (int i = 0; i <= segments; i++)
    {
        float a = -2.0f * 3.14159265f * i / segments;

        glVertex3f(
            cosf(a) * radius,
            -half,
            sinf(a) * radius
        );
    }

    glEnd();
}
void FPVDrone::drawDroneFrame() {
    glPushMatrix();

    // Нижняя и верхняя центральные пластины
    glColor3f(0.08f, 0.08f, 0.08f);

    glTranslatef(0.0f, 0.0f, 0.0f);
    drawCube(1.8f, 0.12f, 2.1f);

    glTranslatef(0.0f, 0.22f, 0.0f);
    drawCube(1.6f, 0.10f, 1.8f);

    // Четыре луча
    glColor3f(0.12f, 0.12f, 0.12f);

    // Передний левый
    glPushMatrix();
    glTranslatef(-0.9f, 0.0f, 0.95f);
    glRotatef(-45.0f, 0, 1, 0);
    drawCube(1.8f, 0.14f, 0.18f);
    glPopMatrix();

    // Передний правый
    glPushMatrix();
    glTranslatef(0.9f, 0.0f, 0.95f);
    glRotatef(45.0f, 0, 1, 0);
    drawCube(1.8f, 0.14f, 0.18f);
    glPopMatrix();

    // Задний левый
    glPushMatrix();
    glTranslatef(-0.9f, 0.0f, -0.95f);
    glRotatef(45.0f, 0, 1, 0);
    drawCube(1.8f, 0.14f, 0.18f);
    glPopMatrix();

    // Задний правый
    glPushMatrix();
    glTranslatef(0.9f, 0.0f, -0.95f);
    glRotatef(-45.0f, 0, 1, 0);
    drawCube(1.8f, 0.14f, 0.18f);
    glPopMatrix();

    glPopMatrix();
}
void FPVDrone::drawDroneCamera() {
    glPushMatrix();

    // Камера спереди
    glTranslatef(0.0f, 0.28f, 1.12f);

    // Корпус
    glColor3f(0.06f, 0.06f, 0.06f);
    drawCube(0.65f, 0.45f, 0.55f);

    // Немного наклоняем вниз
    glTranslatef(0.0f, -0.02f, 0.28f);

    // Объектив
    glColor3f(0.02f, 0.02f, 0.02f);

    glRotatef(90.0f, 1, 0, 0);
    drawCylinder(0.17f, 0.18f);

    glTranslatef(0.0f, 0.0f, 0.1f);

    glColor3f(0.02f, 0.08f, 0.12f);
    drawCylinder(0.13f, 0.05f);

    glPopMatrix();
}
void FPVDrone::drawDroneBattery() {
    glPushMatrix();

    // Обычно аккумулятор лежит сверху/в центре
    glTranslatef(0.0f, 0.42f, -0.05f);

    glColor3f(0.03f, 0.03f, 0.03f);

    drawCube(0.95f, 0.42f, 1.5f);

    // Ремешки аккумулятора
    glColor3f(0.15f, 0.15f, 0.15f);

    glPushMatrix();
    glTranslatef(0.0f, 0.23f, 0.0f);
    drawCube(1.05f, 0.05f, 0.12f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.23f, 0.5f);
    drawCube(1.05f, 0.05f, 0.12f);
    glPopMatrix();

    // Силовые провода
    glColor3f(0.08f, 0.08f, 0.08f);

    glPushMatrix();
    glTranslatef(0.25f, 0.55f, -0.78f);
    glRotatef(90.0f, 1, 0, 0);
    drawCylinder(0.025f, 0.4f);
    glPopMatrix();

    glPopMatrix();
}
void FPVDrone::drawMotor() {
    glPushMatrix();

    glColor3f(0.16f, 0.16f, 0.16f);

    drawCylinder(0.20f, 0.20f);

    glTranslatef(0.0f, 0.14f, 0.0f);

    glColor3f(0.25f, 0.25f, 0.25f);

    drawCylinder(0.14f, 0.08f);

    glPopMatrix();
}
void FPVDrone::drawPropeller(bool reversed) {
    glPushMatrix();

    glColor3f(0.02f, 0.02f, 0.02f);

    // Центральная втулка
    drawCylinder(0.06f, 0.05f);

    glTranslatef(0.0f, 0.04f, 0.0f);

    // Первый широкий лепесток
    glPushMatrix();

    if (reversed)
        glRotatef(180.0f, 0, 1, 0);

    glBegin(GL_QUADS);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.75f, 0.0f, 0.08f);
    glVertex3f(0.95f, 0.0f, 0.02f);
    glVertex3f(0.25f, 0.0f, -0.08f);

    glEnd();

    glPopMatrix();


    // Второй лепесток
    glPushMatrix();
    glRotatef(180.0f, 0, 1, 0);

    glBegin(GL_QUADS);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.75f, 0.0f, 0.08f);
    glVertex3f(0.95f, 0.0f, 0.02f);
    glVertex3f(0.25f, 0.0f, -0.08f);

    glEnd();

    glPopMatrix();

    glPopMatrix();
}
void FPVDrone::drawFPVDrone() {
    glPushMatrix();

    // Рама
    drawDroneFrame();

    // Камера
    drawDroneCamera();

    // Аккумулятор
    drawDroneBattery();


    // -------------------------
    // Моторы + пропеллеры
    // -------------------------

    // Front Left
    glPushMatrix();
    glTranslatef(-1.30f, 0.05f, 1.30f);
    drawMotor();

    glTranslatef(0.0f, 0.20f, 0.0f);
    drawPropeller(false);
    glPopMatrix();


    // Front Right
    glPushMatrix();
    glTranslatef(1.30f, 0.05f, 1.30f);
    drawMotor();

    glTranslatef(0.0f, 0.20f, 0.0f);
    glRotatef(30.0f, 0, 1, 0);
    drawPropeller(true);
    glPopMatrix();


    // Rear Left
    glPushMatrix();
    glTranslatef(-1.30f, 0.05f, -1.30f);
    drawMotor();

    glTranslatef(0.0f, 0.20f, 0.0f);
    glRotatef(30.0f, 0, 1, 0);
    drawPropeller(true);
    glPopMatrix();


    // Rear Right
    glPushMatrix();
    glTranslatef(1.30f, 0.05f, -1.30f);
    drawMotor();

    glTranslatef(0.0f, 0.20f, 0.0f);
    drawPropeller(false);
    glPopMatrix();


    // -------------------------
    // Радиоантенны
    // -------------------------

    glColor3f(0.05f, 0.05f, 0.05f);

    float antennaX[] = { -0.3f, 0.3f };

    for (float x : antennaX) {
        glPushMatrix();

        glTranslatef(x, 0.35f, -1.0f);
        glRotatef(-20.0f, 1, 0, 0);

        drawCylinder(0.025f, 0.7f);

        glPopMatrix();
    }


    // Радиоприёмник
    glPushMatrix();

    glTranslatef(0.0f, 0.4f, -0.8f);

    glColor3f(0.05f, 0.08f, 0.05f);

    drawCube(0.40f, 0.15f, 0.55f);

    glPopMatrix();


    glPopMatrix();
}
void FPVDrone::drawRemoteController() {
    glPushMatrix();

    glColor3f(0.08f, 0.08f, 0.08f);

    drawCube(2.2f, 0.35f, 1.3f);

    glPushMatrix();

    glTranslatef(-0.55f, 0.25f, 0.1f);

    glColor3f(0.15f, 0.15f, 0.15f);
    drawCylinder(0.13f, 0.12f);

    glTranslatef(0.0f, 0.12f, 0.0f);

    glColor3f(0.25f, 0.25f, 0.25f);
    drawCylinder(0.05f, 0.25f);

    glPopMatrix();


    glPushMatrix();

    glTranslatef(0.55f, 0.25f, 0.1f);

    glColor3f(0.15f, 0.15f, 0.15f);
    drawCylinder(0.13f, 0.12f);

    glTranslatef(0.0f, 0.12f, 0.0f);

    glColor3f(0.25f, 0.25f, 0.25f);
    drawCylinder(0.05f, 0.25f);

    glPopMatrix();


    glPushMatrix();

    glTranslatef(0.0f, 0.23f, -0.30f);

    glColor3f(0.02f, 0.07f, 0.08f);

    drawCube(0.65f, 0.08f, 0.35f);

    glPopMatrix();


    glColor3f(0.12f, 0.12f, 0.12f);
    float antenaX[] = { -0.45f, 0.45f };

    for (float x : antenaX) {
        glPushMatrix();

        glTranslatef(x, 0.45f, -0.45f);

        glRotatef(-20.0f, 1, 0, 0);

        drawCylinder(0.035f, 0.8f);

        glPopMatrix();
    }

    glPopMatrix();
}
void FPVDrone::drawFPVGoggles() {
    glPushMatrix();

    // Левая часть
    glPushMatrix();

    glTranslatef(-0.48f, 0.0f, 0.0f);

    glColor3f(0.06f, 0.06f, 0.06f);

    drawCube(0.9f, 0.55f, 0.55f);

    // Экран/линза
    glTranslatef(0.0f, 0.0f, 0.30f);

    glColor3f(0.03f, 0.10f, 0.14f);

    drawCube(0.6f, 0.32f, 0.03f);

    glPopMatrix();


    // Правая часть
    glPushMatrix();

    glTranslatef(0.48f, 0.0f, 0.0f);

    glColor3f(0.06f, 0.06f, 0.06f);

    drawCube(0.9f, 0.55f, 0.55f);

    glTranslatef(0.0f, 0.0f, 0.30f);

    glColor3f(0.03f, 0.10f, 0.14f);

    drawCube(0.6f, 0.32f, 0.03f);

    glPopMatrix();


    // Перемычка
    glColor3f(0.08f, 0.08f, 0.08f);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.05f);
    drawCube(0.45f, 0.25f, 0.5f);
    glPopMatrix();


    // Ремень
    glColor3f(0.03f, 0.03f, 0.03f);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.45f);

    glRotatef(90.0f, 1, 0, 0);

    drawCube(1.8f, 0.1f, 0.15f);

    glPopMatrix();

    glPopMatrix();
}
void FPVDrone::drawFPVEquipment() {
    glDisable(GL_TEXTURE_2D);

    // Дрон
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 7.0f);
    glScalef(0.4f, 0.4f, 0.4f);

    drawFPVDrone();

    glPopMatrix();


    // Пульт
    glPushMatrix();
    glTranslatef(5.0f, 0.5f, 7.0f);
    glRotatef(25.0f, 0, 1, 0);

    drawRemoteController();

    glPopMatrix();


    // FPV очки
    glPushMatrix();
    glTranslatef(-5.0f, 0.5f, 7.0f);
    glRotatef(-20.0f, 0, 1, 0);

    drawFPVGoggles();

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}