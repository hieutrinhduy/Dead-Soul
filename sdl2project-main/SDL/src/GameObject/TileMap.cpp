#include "TileMap.h"

#include <stdio.h>
#include <stdint.h>
#include <thread>

#include "ResourceManagers.h"
#include "Camera.h"


#pragma region CONSTRUCTOR & DECONSTRUCTOR
TileMap::TileMap(const char* tilemapFile, const char* tilesetFile, int tileSize) :
    m_tileSize(tileSize), m_tilemapFile(tilemapFile), m_tilesetFile(tilesetFile),
    m_isPlaying(false)
{
    GetTileIdFromTilemap();
    LoadTileTexturesFromTileset();
}


TileMap::~TileMap()
{
}
#pragma endregion


#pragma region Implement TILE_MAP
void TileMap::GetTileIdFromTilemap()
{
    FILE* file = NULL;
    errno_t err = fopen_s(&file, m_tilemapFile, "rb");

    if (err != 0 || file == NULL) {
        printf("Không thể mở tệp.\n");
        return;
    }


    int16_t rows, cols, id;
    fread(&rows, 2, 1, file);
    fread(&cols, 2, 1, file);

    for (int i = 0; i < rows; i++) {
        std::vector<int> a;
        for (int j = 0; j < cols; j++) {
            fread(&id, 2, 1, file);
            a.push_back(id);
            if (id >= 0) {
                m_tileTextures.insert(std::pair<int, std::shared_ptr<TextureManager>>(id, nullptr));
            }
        }
        m_tileIds.push_back(a);
    }

    fclose(file);
}


void TileMap::LoadTileTexturesFromTileset()
{
    for (auto& texture : m_tileTextures) {
        texture.second = ResourceManagers::GetInstance()->GetTexture("Map/tiles/" + std::to_string(texture.first) + ".png");
    }
}


void TileMap::DrawMap(SDL_Renderer* renderer)
{
    float cameraX = Camera::GetInstance()->GetPosition().x;
    float cameraY = Camera::GetInstance()->GetPosition().y;
    int startCol = cameraX / m_tileSize;
    int startRow = cameraY / m_tileSize;
    int endCol = (cameraX + SCREEN_WIDTH) / m_tileSize + 1;
    int endRow = (cameraY + SCREEN_HEIDHT) / m_tileSize + 1;

    for (int row = startRow; row < endRow && row < m_tileIds.size(); ++row)
    {
        for (int col = startCol; col < endCol && col < m_tileIds[row].size(); ++col)
        {
            int tileID = m_tileIds[row][col];
            if (m_tileTextures.count(tileID) == 0) continue;

            SDL_Rect dstRect = {
                col * m_tileSize - cameraX,
                row * m_tileSize - cameraY,
                m_tileSize,
                m_tileSize
            };

            SDL_RenderCopy(renderer, m_tileTextures[tileID]->GetTextureObj(), nullptr, &dstRect);
        }
    }
}
#pragma endregion


#pragma region GETTER
int TileMap::GetWidth() { return m_tileIds.empty() ? 0 : m_tileIds[0].size(); }
int TileMap::GetHeight() { return m_tileIds.size(); }
int TileMap::GetTileSize() { return m_tileSize; }
#pragma endregion


#pragma region SETTER
void TileMap::SetPlayer(std::shared_ptr<PlayerController> player) { m_player = player; }
#pragma endregion


#pragma region Start a thread that Check MOVEMENT_ABILITY
void TileMap::StartCheckingPlayerMovementAbility() {
    std::thread t([this]() {
        while (m_isPlaying) {
            SDL_Rect player = m_player->GetCollider();
            int startColumn = player.x / m_tileSize;
            int endColumn = (player.x + player.w) / m_tileSize;
            int startRow = player.y / m_tileSize;
            int endRow = (player.y + player.h) / m_tileSize;
            // if the player's position is exactly on the edge of a tile, do not take the next tile
            if (endColumn * m_tileSize == player.x + player.w) endColumn -= 1;
            if (endRow * m_tileSize == player.y + player.h) endRow -= 1;

            // BOTTOM LIMIT:
            bool finishCheck = false;
            int bottomRowLimit = endRow + 1;
            for (int i = endRow + 1;;i++) {
                for (int j = startColumn; j <= endColumn; j++) {
                    if (m_tileIds[i][j] == 1) {
                        finishCheck = true;
                        break;
                    }
                }
                if (finishCheck) {
                    finishCheck = false;
                    break;
                }
            }
        }
    });
}
#pragma endregion
