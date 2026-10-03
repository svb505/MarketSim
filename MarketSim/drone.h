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

    GLuint texture;

    bool render = false;
};
struct PlacedObject {
    std::string name;
    svbmath::Vec3 pos;
};
class FPVDrone {
private:
    std::vector<std::string> baseEquipment = {"Frame","Camera","Battery", "Motor",
        "Propeller","Controller" ,"Goggles",};
    std::map<std::string, float> equipmentPrice = {{"Frame",100.0f},{"Camera",300.0f},
        {"Battery",250.0f} ,{"Motor",350.0f} ,{"Propeller",150.0f}, {"Controller",100.0f},
        {"Goggles",250.0f}};

    std::vector<Package> boxesWithEquip = {};
    std::vector<PlacedObject> placedObjects;

    float deliveryTime = 10.0; //sec
    float textTime = 3.0f;

    bool showText = false;
    bool inDelivery = false;

    int currentIndex = 0;
public:
    bool getShowText();
    int& getCurrentIndex();
    std::vector<Package>& getOrderEquip();
    const std::vector<std::string>& getBaseEquip();
    const std::map<std::string, float>& getEquipPrice();
    bool isPlaced(const std::string& name) const;

    void drawAllPlacedObjects();
    void addPlacedObj(std::string name, svbmath::Vec3 pos);
    void sendPackage(std::vector<std::string>& equip, BuildingSettings& build);
    void delivery(float dt);
    void drawAllBoxes();
    void checkBoxesIntersection(Camera& cam);
    void drawEquipModel(std::string model, Camera& cam, bool placed);

    void drawCube(float sx, float sy, float sz);
    void drawCylinder(float radius, float height, int segments = 16);
    void drawDroneFrame(const svbmath::Vec3& pos);
    void drawDroneCamera(const svbmath::Vec3& pos);
    void drawDroneBattery(const svbmath::Vec3& pos);
    void drawMotor(const svbmath::Vec3& pos);
    void drawPropeller(const svbmath::Vec3& pos,bool reversed);
    void drawFPVDrone(const svbmath::Vec3& pos);
    void drawRemoteController(const svbmath::Vec3& pos);
    void drawFPVGoggles(const svbmath::Vec3& pos);
    void drawFPVEquipment();
};