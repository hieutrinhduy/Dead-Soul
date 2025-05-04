#pragma once

#include "Singleton.h"
#include <string>

typedef struct SaveData {
	bool isWatchedStory;
	int level;
	int musicVolume; // from 0 - 128
	int sfxVolume;   // from 0 - 128

	SaveData() {
		isWatchedStory = false;
		level = 1;
		musicVolume = 128;
		sfxVolume = 128;
	}
} SaveData;

class DataSaver : public CSingleton<DataSaver>
{
public:
	DataSaver();
	~DataSaver();

	// SETTER:
	void SetSaveData(SaveData saveData);

	void SaveGame(const std::string& filename);
	SaveData LoadGame(const std::string& filename);


private:
	SaveData m_saveData;
	std::string m_saveFileName;
};
