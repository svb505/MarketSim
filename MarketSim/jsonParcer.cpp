#include "jsonParcer.h"
#include <string>
#include <fstream>
#include <nlohmann//json.hpp>

using json = nlohmann::json;

void changePlayerConfig(const std::string key, const std::string value) {
	std::ifstream file("droneNames.json");
	json j;

	file >> j;

	j[key] = value;

	std::ofstream("droneNames.json") << j.dump(4);
}
std::string getStringFromJson(std::string key, std::string path) {
	std::ifstream file(path);

	json j;

	file >> j;

	if (j.contains(key)) return j[key];
	else return "";
}
