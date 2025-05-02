#pragma once

#include "Singleton.h"
#include <string>

struct GameState {
	bool isWatchedStory;
	int level;

	GameState() {
		isWatchedStory = false;
		level = 1;
	}
};

class DataSaver : public CSingleton<DataSaver>
{
public:
	DataSaver();
	~DataSaver();

	// SETTER:
	void SetSaveData(bool isWatchedStory, int level);

	void SaveGame(const std::string& filename);
	GameState LoadGame(const std::string& filename);


private:
	GameState gameState;
};
