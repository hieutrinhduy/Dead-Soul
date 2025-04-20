#include "TileMap.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <SDL_image.h>
#include <iostream>


TileMap::TileMap()
{
    m_tileSize = 32;
}

TileMap::~TileMap()
{
    for (auto& pair : m_tileTextures)
    {
        SDL_DestroyTexture(pair.second);
    }
}

void TileMap::LoadMap(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Không mở được file map: " << filePath << "\n";
        return;
    }
    else {
        std::cout << "Đang load map từ: " << filePath << "\n";
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << "Dòng đọc được: " << line << "\n";

        std::stringstream ss(line);
        int tile;
        std::vector<int> row;

        while (ss >> tile) {
            row.push_back(tile);
        }

        if (!row.empty()) {
            std::cout << "➡️ Thêm dòng vào map: ";
            for (int t : row) std::cout << t << " ";
            std::cout << "\n";

            m_mapData.push_back(row);
        }
    }

    std::cout << "Tổng dòng map: " << m_mapData.size() << "\n";
}


void TileMap::LoadTiles(SDL_Renderer* renderer, const std::string& tilesFolder, int tileSize)
{
    m_tilesFolder = tilesFolder;
    m_tileSize = tileSize;

    for (int i = 0; i < 256; ++i)
    {
        std::string path = tilesFolder + "/" + std::to_string(i) + ".png";

        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface)
        {
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (texture)
        {
            m_tileTextures[i] = texture;
            printf("LoadTile Success\n");
        }
    }
}

void TileMap::DrawMap(SDL_Renderer* renderer)
{
    for (int row = 0; row < m_mapData.size(); ++row)
    {
        for (int col = 0; col < m_mapData[row].size(); ++col)
        {
            int tileID = m_mapData[row][col];

            if (m_tileTextures.count(tileID) == 0) continue;

            SDL_Rect dstRect = {
                col * m_tileSize,
                row * m_tileSize,
                m_tileSize,
                m_tileSize
            };

            SDL_RenderCopy(renderer, m_tileTextures[tileID], nullptr, &dstRect);
            std::cout << "Rendering tileID: " << tileID << " at (" << col * m_tileSize << ", " << row * m_tileSize << ")\n";
        }
    }
}
