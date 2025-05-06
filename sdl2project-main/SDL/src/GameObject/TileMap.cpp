#include "TileMap.h"

#include <stdio.h>
#include <stdint.h>

#include "ResourceManagers.h"
#include "Camera.h"



TileMap::TileMap(const char* tilemapFile, const char* tilesetFile, int tileSize) :
    m_tileSize(tileSize), m_tilemapFile(tilemapFile), m_tilesetFile(tilesetFile)
{
    GetTileIdFromTilemap();
    LoadTileTexturesFromTileset();
}


TileMap::~TileMap()
{
}


void TileMap::GetTileIdFromTilemap()
{
    FILE* file = NULL;
    errno_t err = fopen_s(&file, m_tilemapFile, "rb");

    if (err != 0 || file == NULL) {
        printf("Không thể mở tệp.\n");
        return;
    }


    int16_t rows, cols, id;
    fread(&rows, 2, 1, file); printf("%d\n", rows);
    fread(&cols, 2, 1, file); printf("%d\n", cols);

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


int TileMap::GetWidth() { return m_tileIds.empty() ? 0 : m_tileIds[0].size(); }
int TileMap::GetHeight() { return m_tileIds.size(); }
int TileMap::GetTileSize() { return m_tileSize; }