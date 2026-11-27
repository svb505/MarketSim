#pragma once

struct Stat{
    int purchasesForDay = 0;
    int sallesForDay = 0;
    int losesForDay = 0;
    float sessionTime = 0.0f;
};

int getSessionNumber();
void saveStatistickToJSON(Stat& stat);
Stat getStatistick();