#pragma once

#include <vector>
#include <map>
#include <SDL.h>

#include "Define.h"
#include "TextureManager.h"
#include "PlayerController.h"
class GSPlay;


/*
    Read binary file to get tile_id -> get texture based on tile_id
    After having enough tile_id and corresponding texture, draw tile map
*/


class TileMap
{
public:
    TileMap(const char* tileMapFile, const char* tilesetFile, int tileSize, std::shared_ptr<int> timeScale);
    ~TileMap();

    void GetTileIdFromTilemap();
    void LoadTileTexturesFromTileset();
    void DrawMap(SDL_Renderer* renderer);
    void LoadNewMap();

    // GETTER:
    int GetWidth();
    int GetHeight();
    int GetTileSize();
    Vector2 GetShopIndicatorPosition();

    // SETTER:
    void SetReferenceToGSPlay(GSPlay* gsPlay);
    void SetPlayer(std::shared_ptr<PlayerController> player);

    // checking MOVEMENT_ABILITY thread:
    void StartCheckingPlayerMovementAbility();
    void ClearThread();
    bool m_finishThread;

    void ExportFullMapImage(SDL_Renderer* renderer);

    bool completeLoading;
    bool atShop = false;
    int victoryCode = 0;


private:
    const char* m_tilemapFile;
    const char* m_tilesFolder;
    std::shared_ptr<int> m_timeScale;
    int m_mapNumber;

    int m_mapWidth, m_mapHeight;

    std::vector<std::vector<int>> m_tileIds;
    std::map<int, std::shared_ptr<TextureManager>> m_tileTextures;
    int m_tileSize;

    int shopLeft = 0, shopRight = 0, shopTop = -1000, shopBottom = 0;
    int portalLeft, portalRight, portalTop, portalBottom;
    Vector2 shopIndicatorPosition;
    float shopIndicatorPositionX, shopIndicatorPositionY;

    bool m_isPlaying;
    GSPlay* m_gsPlay;
    std::shared_ptr<PlayerController> m_player;
};