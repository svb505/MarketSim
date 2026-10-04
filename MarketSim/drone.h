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
#include "model.h"

struct ShelfSetting {
    svbmath::Vec3 pos;
    bool busy = false;
};
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
struct Components {
    int frame = 0, camera = 0, battery = 0, motor = 0, propeller = 0, controller = 0, goggles = 0;
};

class FPVDrone {
private:
    std::vector<std::string> baseEquipment = {"Cadre","Camera","Batterie", "Moteur",
        "Hélice","Controlleur" ,"Lunettes","FPV Drone T1"};
    std::map<std::string, float> equipmentPrice = { {"Cadre",100.0f},{"Camera",300.0f},
        {"Batterie",250.0f} ,{"Moteur",350.0f} ,{"Hélice",150.0f}, {"Controlleur",100.0f},
        {"Lunettes",250.0f},{"FPV Drone T1",1000.0f} };

    std::vector<Package> boxesWithEquip = {};
    std::vector<PlacedObject> placedObjects;
    std::vector<ShelfSetting> positionsOnShelves;

    float deliveryTime = 10.0; //sec
    float textTime = 3.0f;

    bool showText = false;
    bool inDelivery = false;

    int currentIndex = 0;
public:
    bool getShowText();
    int& getCurrentIndex();
    std::vector<Package>& getOrderEquip();
    std::vector<PlacedObject>& getPlacedEquip();
    const std::vector<std::string>& getBaseEquip();
    const std::map<std::string, float>& getEquipPrice();
    bool isPlaced(const std::string& name) const;
    std::vector<svbmath::Vec3> generateShelfPoints(const Shelf& shelf);

    void drawAvaibleShelfPos();
    void checkIfCanPlaceInShelf(Camera& cam);
    void drawAllPlacedObjects(OBJModel& objmodel);
    void addPlacedObj(std::string name, svbmath::Vec3 pos);
    void sendPackage(std::vector<std::string>& equip, BuildingSettings& build);
    void delivery(float dt);
    void drawAllBoxes();
    void checkBoxesIntersection(Camera& cam);
    void drawEquipModel(std::string model, Camera& cam, bool placed, OBJModel& objmodel);
    void checkIfCanCreateDrone(Camera& cam, CashRegister& build);
    void initCoordinatesForShelves(Building& build);

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