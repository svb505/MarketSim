struct Stat{
    int purchasesForDay;
    int sallesForDay;
    int losesForDay;
    float sessionTime;
};

int getSessionNumber();
void saveStatistickToJSON(Stat& stat);
Stat getStatistick();