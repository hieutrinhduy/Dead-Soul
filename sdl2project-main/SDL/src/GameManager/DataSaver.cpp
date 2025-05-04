#include "DataSaver.h"
#include <fstream>

DataSaver::DataSaver() : m_saveFileName("save.dat") {

}


DataSaver::~DataSaver() {

}


void DataSaver::SetSaveData(SaveData saveData) {
    m_saveData = saveData;
}


void DataSaver::SaveGame(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);

    if (out.is_open()) {
        out.write((char*)&m_saveData, sizeof(SaveData));
        out.close();
    }
}


SaveData DataSaver::LoadGame(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);

    if (in.is_open()) {
        in.read((char*)&m_saveData, sizeof(SaveData));
        in.close();
    }

    return m_saveData;
}