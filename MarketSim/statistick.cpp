#include <nlohmann//json.hpp>
#include "statistick.h"
#include "variables.h"
#include <format>
#include <vector>

using json = nlohmann::json;

int getSessionNumber(){
    std::ifstream file("stat.json");
	json j;

    if (!file.is_open()) return 0;

    file >> j;

    return j["sessionN"];
}

void saveStatistickToJSON(Stat& stat){
    std::ifstream file("stat.json");
	json j;

    file >> j;

    std::string nextSessionStr = std::format("session{}",sessionNumber);

    j["statistick"][nextSessionStr] = {
        {"purchases":stat.purchasesForDay},
        {"sessionTime":stat.sessionTime}, 
        {"salles":stat.sallesForDay}, 
        {"loses": stat.losesForDay}
    };

    ++sessionNumber;
    j["sessionN"] = sessionNumber

    std::ofstream outFile("stat.json");
    outFile << j.dump(4);
    outFile.close();
}
Stat getStatistick(){
    std::ifstream file("stat.json");

    json j;  
    file >> j;

    std::vector<Stat> bufStat;
    Stat finalStat;

    int sessionNumbers = (j["sessionN"] == 0) : 1 ? j["sessionN"];

    for (auto& [key, element] : j["statistick"].items()){
        Stat buf;

        buf.sallesForDay = element.value("salles",0)
        buf.sessionTime = element.value("sessionTime",0.0f)
        buf.losesForDay = element.value("loses",0)
        buf.purchasesForDay = element.value("purchases",0)

        bufStat.push_back(buf);
    }

    for (auto& e : bufStat){
        finalStat.sallesForDay += e.sallesForDay;
        finalStat.sessionTime +=e.sessionTime;
        finalStat.losesForDay += e.losesForDay;
        finalStat.purchasesForDay += e.purchasesForDay;
    }

    finalStat.sallesForDay /= sessionNumbers;
    finalStat.sessionTime /= sessionNumbers;
    finalStat.losesForDay /= sessionNumbers;
    finalStat.purchasesForDay /= sessionNumbers;

    return finalStat;


}