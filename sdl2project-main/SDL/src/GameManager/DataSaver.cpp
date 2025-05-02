#include "DataSaver.h"
#include <fstream>

DataSaver::DataSaver() {

}


DataSaver::~DataSaver() {

}


void DataSaver::SetSaveData(bool isWatchedStory, int level) {
	gameState.isWatchedStory = isWatchedStory;
	gameState.level = level;
}


void DataSaver::SaveGame(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);

    if (out.is_open()) {
        out.write((char*)&gameState, sizeof(GameState));
        out.close();
    }
}


GameState DataSaver::LoadGame(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);

    if (in.is_open()) {
        in.read((char*)&gameState, sizeof(GameState));
        in.close();
    }

    return gameState;
}