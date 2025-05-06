#pragma once

#include <vector>
#include <string>
#include <map>
#include <SDL.h>

#include "Define.h"
#include "TextureManager.h"


/*
    Read binary file to get tile_id -> get texture based on tile_id
    After having enough tile_id and corresponding texture, draw tile map
*/


class TileMap
{
public:
    TileMap(const char* tileMapFile, const char* tilesetFile, int tileSize);
    ~TileMap();

    void GetTileIdFromTilemap();
    void LoadTileTexturesFromTileset();
    void DrawMap(SDL_Renderer* renderer);

    int GetWidth();
    int GetHeight();
    int GetTileSize();

private:
    const char* m_tilemapFile;
    const char* m_tilesetFile;

    std::vector<std::vector<int>> m_tileIds;
    std::map<int, std::shared_ptr<TextureManager>> m_tileTextures;
    std::string m_tilesFolder;
    int m_tileSize;
};