#pragma once

#include <vector>
#include <map>
#include <SDL.h>

#include "Define.h"
#include "TextureManager.h"
#include "PlayerController.h"


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

    // GETTER:
    int GetWidth();
    int GetHeight();
    int GetTileSize();

    // SETTER:
    void SetPlayer(std::shared_ptr<PlayerController> player);

    // checking MOVEMENT_ABILITY thread:
    void StartCheckingPlayerMovementAbility();
    void ClearThread();
    bool m_finishThread;



private:
    const char* m_tilemapFile;
    const char* m_tilesetFile;

    int m_mapWidth, m_mapHeight;

    std::vector<std::vector<int>> m_tileIds;
    std::map<int, std::shared_ptr<TextureManager>> m_tileTextures;
    std::string m_tilesFolder;
    int m_tileSize;


    bool m_isPlaying;
    std::shared_ptr<PlayerController> m_player;
};