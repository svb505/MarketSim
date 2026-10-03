#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include "svbmath.h"
#include "building.h"

struct Package {
    std::vector<std::string> equip;
    svbmath::Vec3 pos;
    bool render = false;
};

class FPVDrone {
private:
    std::vector<std::string> baseEquipment = {"Frame","Camera","Battery", "Motor",
        "Propeller","Controller" ,"Goggles",};
    std::map<std::string, float> equipmentPrice = {{"Frame",100.0f},{"Camera",300.0f},
        {"Battery",250.0f} ,{"Motor",350.0f} ,{"Propeller",150.0f}, {"Controller",100.0f},
        {"Goggles",250.0f}};

    std::vector<Package> boxesWithEquip = {};

    float deliveryTime = 10.0; //sec
    float textTime = 3.0f;

    bool showText = false;
    bool inDelivery = false;
public:
    const std::vector<std::string>& getBaseEquip();
    const std::map<std::string, float>& getEquipPrice();

    void sendPackage(std::vector<std::string>& equip, BuildingSettings& build);
    void delivery(float dt);
    void drawAllBoxes();

    void drawCube(float sx, float sy, float sz);
    void drawCylinder(float radius, float height, int segments = 16);
    void drawDroneFrame();
    void drawDroneCamera();
    void drawDroneBattery();
    void drawMotor();
    void drawPropeller(bool reversed = false);
    void drawFPVDrone();
    void drawRemoteController();
    void drawFPVGoggles();
    void drawFPVEquipment();
};