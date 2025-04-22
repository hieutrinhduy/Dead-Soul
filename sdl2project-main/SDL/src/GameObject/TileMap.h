#pragma once
#include <vector>
#include <string>
#include <map>
#include <SDL.h>

class TileMap
{
public:
    TileMap();
    ~TileMap();

    void LoadMap(const std::string& filePath);
    void LoadTiles(SDL_Renderer* renderer, const std::string& tilesFolder, int tileSize);
    void DrawMap(SDL_Renderer* renderer, float cameraX, float cameraY, int screenWidth, int screenHeight);
    int GetWidth();
    int GetHeight();
    int GetTileSize();

private:
    std::vector<std::vector<int>> m_mapData;
    std::map<int, SDL_Texture*> m_tileTextures;
    std::string m_tilesFolder;
    int m_tileSize;
};
