#include "TileMap.h"

#include <stdio.h>
#include <stdint.h>
#include <thread>

#include "ResourceManagers.h"
#include "Camera.h"


constexpr int CHECK_SCOPE = 2; // scope of the object-terrain collision check


#pragma region CONSTRUCTOR & DECONSTRUCTOR
TileMap::TileMap(const char* tilemapFile, const char* tilesetFile, int tileSize) :
    m_tileSize(tileSize), m_tilemapFile(tilemapFile), m_tilesetFile(tilesetFile),
    m_mapHeight(0), m_mapWidth(0),
    m_isPlaying(true)
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
        return;
    }


    int16_t id;
    fread(&m_mapHeight, 2, 1, file);
    fread(&m_mapWidth, 2, 1, file);

    for (int i = 0; i < m_mapHeight; i++) {
        std::vector<int> a;
        for (int j = 0; j < m_mapWidth; j++) {
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
        texture.second = ResourceManagers::GetInstance()->GetTexture("Map/map1_tileset/" + std::to_string(texture.first) + ".png");
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


#pragma region Thread that Check MOVEMENT_ABILITY
void TileMap::StartCheckingPlayerMovementAbility() {
    std::thread t([this]() {
        m_finishThread = false;
        bool canGoLeft = true, canGoRight = true, bumpTheHead = true, isGrounded = true;
        float leftLimit = 200, rightLimit = 200, topLimit = 200, bottomLimit = 200;

        while (m_isPlaying) {
            SDL_Rect player = m_player->GetCollider();
            float playerRight = player.x + player.w;
            float playerBottom = player.y + player.h;

            int startColumn = player.x / m_tileSize;
            int endColumn = playerRight / m_tileSize;
            int startRow = player.y / m_tileSize;
            int endRow = playerBottom / m_tileSize;
            // if the player's position is exactly on the edge of a tile, do not take the next tile
            if (endColumn * m_tileSize == playerRight) endColumn -= 1;
            if (endRow * m_tileSize == playerBottom) endRow -= 1;

            // LEFT LIMIT:
            bool finishCheck = false;
            int leftColumnLimit = startColumn;
            if (m_player->IsJumping() && m_tileIds[startRow - 1][startColumn - 1] == 15 ||
                m_player->IsFalling() && m_tileIds[endRow + 1][startColumn - 1] == 3) {
                leftColumnLimit--;
            }
            else {
                for (int i = 1; i <= CHECK_SCOPE; i++) {
                    leftColumnLimit--;
                    if (leftColumnLimit == 2) break;
                    for (int j = startRow; j <= endRow; j++) {
                        if (m_tileIds[j][leftColumnLimit] == 3 || m_tileIds[j][leftColumnLimit] == 7 ||
                            m_tileIds[j][leftColumnLimit] == 11 || m_tileIds[j][leftColumnLimit] == 15) {
                            finishCheck = true;
                            break;
                        }
                    }
                    if (finishCheck) {
                        break;
                    }
                }
            }
            float tilePosition = (leftColumnLimit + 1) * m_tileSize;
            leftLimit = player.x - tilePosition;
            if (tilePosition == player.x) canGoLeft = false;
            else canGoLeft = true;

            // RIGHT LIMIT:
            finishCheck = false;
            int rightColumnLimit = endColumn;
            if (m_player->IsJumping() && m_tileIds[startRow - 1][startColumn + 1] == 12 ||
                m_player->IsFalling() && m_tileIds[endRow + 1][endColumn + 1] == 0) {
                rightColumnLimit++;
            }
            else {
                for (int i = 1; i <= CHECK_SCOPE; i++) {
                    rightColumnLimit++;
                    if (rightColumnLimit == m_mapWidth - 3) break;
                    for (int j = startRow; j <= endRow; j++) {
                        if (m_tileIds[j][rightColumnLimit] == 0 || m_tileIds[j][rightColumnLimit] == 4 ||
                            m_tileIds[j][rightColumnLimit] == 8 || m_tileIds[j][rightColumnLimit] == 12) {
                            finishCheck = true;
                            break;
                        }
                    }
                    if (finishCheck) {
                        break;
                    }
                }
            }
            tilePosition = rightColumnLimit * m_tileSize;
            rightLimit = tilePosition - playerRight;
            if (tilePosition == playerRight) canGoRight = false;
            else canGoRight = true;

            // TOP LIMIT:
            finishCheck = false;
            int topRowLimit = startRow;
            for (int i = 1; i <= CHECK_SCOPE; i++) {
                topRowLimit--;
                if (topRowLimit == -1) break;
                for (int j = startColumn; j <= endColumn; j++) {
                    if (m_tileIds[topRowLimit][j] >= 12 && m_tileIds[topRowLimit][j] <= 15) {
                        finishCheck = true;
                        break;
                    }
                }
                if (finishCheck) {
                    break;
                }
            }
            tilePosition = (topRowLimit + 1) * m_tileSize;
            topLimit = player.y - tilePosition;
            if (tilePosition == player.y) bumpTheHead = true;
            else bumpTheHead = false;

            // BOTTOM LIMIT:
            finishCheck = false;
            int bottomRowLimit = endRow;
            for (int i = 1; i <= CHECK_SCOPE; i++) {
                bottomRowLimit++;
                if (bottomRowLimit == m_mapHeight) break;
                for (int j = startColumn; j <= endColumn; j++) {
                    if (m_tileIds[bottomRowLimit][j] >= 0 && m_tileIds[bottomRowLimit][j] < 4) {
                        finishCheck = true;
                        break;
                    }
                }
                if (finishCheck) {
                    break;
                }
            }
            tilePosition = bottomRowLimit * m_tileSize;
            bottomLimit = tilePosition - playerBottom;
            if (tilePosition == playerBottom) isGrounded = true;
            else {
                if (isGrounded && !m_player->IsJumping()) m_player->LoseTheirFooting();
                isGrounded = false;
            }

            // update MOVEMENT_ABILITY:
            m_player->SetMovementLimit(leftLimit, rightLimit, topLimit, bottomLimit);
            m_player->SetMovementState(canGoLeft, canGoRight, bumpTheHead, isGrounded);

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        m_finishThread = true;
    });

    t.detach();
}

void TileMap::ClearThread() {
    m_isPlaying = false;
}
#pragma endregion
