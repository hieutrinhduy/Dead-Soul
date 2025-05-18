#include "TileMap.h"

#include <stdio.h>
#include <stdint.h>
#include <thread>

#include "ResourceManagers.h"
#include "Camera.h"
#include "GSPlay.h"


constexpr int CHECK_SCOPE = 2; // scope of the object-terrain collision check


#pragma region CONSTRUCTOR & DECONSTRUCTOR
TileMap::TileMap(const char* tilemapFile, const char* tilesFolder, int tileSize, std::shared_ptr<int> timeScale) :
    m_tileSize(tileSize), m_tilemapFile(tilemapFile), m_tilesFolder(tilesFolder), m_timeScale(timeScale),
    completeLoading(false),
    m_mapHeight(0), m_mapWidth(0), m_mapNumber(1),
    m_isPlaying(true)
{
    GetTileIdFromTilemap();
    LoadTileTexturesFromTileset();

    completeLoading = true;
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
        texture.second = ResourceManagers::GetInstance()->GetTexture(m_tilesFolder + std::to_string(texture.first) + ".png");
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


void TileMap::LoadNewMap() {
    m_mapNumber++;
    if (m_mapNumber == 2) {
        m_tilemapFile = "Data/Textures/Map/output2.bin";
        m_tilesFolder = "Map/map2_tileset/";
        m_tileIds.clear();
        m_tileTextures.clear();
        GetTileIdFromTilemap();
        LoadTileTexturesFromTileset();
        m_gsPlay->ChangeLevel(LevelID::Level2);
        m_player->ResetInput();
    }
    else if (m_mapNumber == 3) {
        m_tilemapFile = "Data/Textures/Map/output3.bin";
        m_tilesFolder = "Map/map1_tileset/";
        m_tileIds.clear();
        m_tileTextures.clear();
        GetTileIdFromTilemap();
        LoadTileTexturesFromTileset();
        m_gsPlay->ChangeLevel(LevelID::Level3);
        m_player->ResetInput();
        Camera::GetInstance()->SetLevelDimension(GetWidth()*32,GetHeight()*32);
    }
    else if (m_mapNumber == 4) {
        m_tilemapFile = "Data/Textures/Map/hidden_boss_map.bin";
        m_tilesFolder = "Map/map2_tileset/";
        m_tileIds.clear();
        m_tileTextures.clear();
        GetTileIdFromTilemap();
        LoadTileTexturesFromTileset();
        m_gsPlay->ChangeLevel(LevelID::SecretLevel);
        m_player->ResetInput();
        Camera::GetInstance()->SetLevelDimension(GetWidth() * 32, GetHeight() * 32);
    }

    if (m_mapNumber >= 3) {
        portalLeft = m_gsPlay->GetFinalGatePos().x;
        portalRight = portalLeft + m_gsPlay->GetGateSize().x;
        portalTop = m_gsPlay->GetFinalGatePos().y;
        portalBottom = portalTop + m_gsPlay->GetGateSize().y;

        if (m_mapNumber == 3) {
            secretPortalLeft = m_gsPlay->GetFinalSecretGatePos().x;
            secretPortalRight = secretPortalLeft + m_gsPlay->GetSecretGateSize().x;
            secretPortalTop = m_gsPlay->GetFinalSecretGatePos().y;
            secretPortalBottom = secretPortalTop + m_gsPlay->GetSecretGateSize().y;
        }
    }

    else {
        portalLeft = m_gsPlay->GetGatePos().x;
        portalRight = portalLeft + m_gsPlay->GetGateSize().x;
        portalTop = m_gsPlay->GetGatePos().y;
        portalBottom = portalTop + m_gsPlay->GetGateSize().y;
    }


    shopLeft = m_gsPlay->GetShopPos().x;
    shopRight = shopLeft + m_gsPlay->GetShopSize().x;
    shopTop = m_gsPlay->GetShopPos().y;
    shopBottom = shopTop + m_gsPlay->GetShopSize().y;

    shopIndicatorPositionX = shopLeft + m_gsPlay->GetShopSize().x * 24 / 59;
    shopIndicatorPositionY = shopTop + m_gsPlay->GetShopSize().y * 11 / 18;


    completeLoading = true;
    playerIsAppear = true;
}
#pragma endregion


#pragma region GETTER
int TileMap::GetWidth() { return m_tileIds.empty() ? 0 : m_tileIds[0].size(); }
int TileMap::GetHeight() { return m_tileIds.size(); }
int TileMap::GetTileSize() { return m_tileSize; }
Vector2 TileMap::GetShopIndicatorPosition() {
    return shopIndicatorPosition;
}
#pragma endregion


#pragma region SETTER
void TileMap::SetReferenceToGSPlay(GSPlay* gsPlay) { m_gsPlay = gsPlay; }
void TileMap::SetPlayer(std::shared_ptr<PlayerController> player) { m_player = player; }
#pragma endregion


#pragma region Thread that Check MOVEMENT_ABILITY
void TileMap::StartCheckingPlayerMovementAbility() {
    std::thread t([this]() {
        m_finishThread = false;
        bool canGoLeft = true, canGoRight = true, bumpTheHead = true, isGrounded = true;
        float leftLimit = 200, rightLimit = 200, topLimit = 200, bottomLimit = 200;

        portalLeft = m_gsPlay->GetGatePos().x;
        portalRight = portalLeft + m_gsPlay->GetGateSize().x;
        portalTop = m_gsPlay->GetGatePos().y;
        portalBottom = portalTop + m_gsPlay->GetGateSize().y;

        shopLeft = m_gsPlay->GetShopPos().x;
        shopRight = shopLeft + m_gsPlay->GetShopSize().x;
        shopTop = m_gsPlay->GetShopPos().y;
        shopBottom = shopTop + m_gsPlay->GetShopSize().y;

        shopIndicatorPositionX = shopLeft + m_gsPlay->GetShopSize().x * 24 / 59;
        shopIndicatorPositionY = shopTop + m_gsPlay->GetShopSize().y * 11 / 18;


        while (m_isPlaying) {
            m_gsPlay->playerIsAppear = playerIsAppear;
            shopIndicatorPosition.x = shopIndicatorPositionX - Camera::GetInstance()->GetPosition().x;
            shopIndicatorPosition.y = shopIndicatorPositionY - Camera::GetInstance()->GetPosition().y;

            SDL_Rect player = m_player->GetCollider();
            float playerRight = player.x + player.w;
            float playerBottom = player.y + player.h;

            // check if player REACHES PORTAL:
            if (playerRight >= portalLeft + 10 && player.x <= portalRight && player.y <= portalBottom && playerBottom >= portalTop) {
                if (m_mapNumber == 3) {
                    if (m_player->GetLevel3State()) victoryCode = 1;
                }
                else {
                    if (m_mapNumber == 4) { if (m_player->GetSecretLevelState()) victoryCode = 2; }
                    else *m_timeScale = -1;
                }
            }

            if (m_mapNumber == 3 && m_player->GetLevel3State()) {
                if (playerRight >= secretPortalLeft + 10 && player.x <= secretPortalRight &&
                    player.y <= secretPortalBottom && playerBottom >= secretPortalTop) {
                    *m_timeScale = -1;
                }
            }

            // check if player REACHES SHOP:
            if (playerRight >= shopLeft && player.x <= shopRight && player.y <= shopBottom && playerBottom >= shopTop)
                atShop = true;
            else atShop = false;

            #pragma region Check Movement limit
            if (completeLoading) {
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
                if (tilePosition == playerBottom) {
                    if (!m_player->IsJumping()) isGrounded = true;
                }
                else {
                    if (isGrounded && !m_player->IsJumping()) m_player->LoseTheirFooting();
                    isGrounded = false;
                }

                // update MOVEMENT_ABILITY:
                m_player->SetMovementLimit(leftLimit, rightLimit, topLimit, bottomLimit);
                m_player->SetMovementState(canGoLeft, canGoRight, bumpTheHead, isGrounded);
            }
            #pragma endregion

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



