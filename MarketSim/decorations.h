#pragma once
#include "svbmath.h"
#include <vector>
#include "model.h"
#include "camera.h"
#include <map>

enum class DecType {
	BUILD1,
	BUILD2
};

class Decorations {
private:
	int buildsCount = 6;
	float buildXStep = 20.0f;

	std::vector<svbmath::Vec3> allPositions;

	svbmath::Vec3 buildBasePos = {0.0f,0.1f,0.0f};

	std::map<DecType, OBJModel*> models;
public:
	void initPositions();
	void initDecoration(std::string modelPath, DecType decType, std::string texturePath = "");
	void drawAllDecorations(DecType decType);
	bool checkCollision(Camera& cam, DecType decType);
};