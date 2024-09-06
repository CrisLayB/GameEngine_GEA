#pragma once
#include <string>
#include "Sprites.h"
#include "Engine/Entity.h"
#include "Engine/Systems.h"
#include <iostream>

enum class TileType {
  NONE,
  WALL,
};

struct Tile {
  int index;
  int tilemapIndex;
  TileType type;
};

struct TileComponent {
  Tile tile;
};

struct TilemapComponent {
  std::string filename;
  std::vector<Tile> tiles;
  int tileSize;
  int scale;
  int width;
  int height;
};

class TilemapSetupSystem : public SetupSystem {
public:
  void run() override {
    std::vector<int> initialMap = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
      0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
      0, 0, 0, 1, 0, 1, 0, 0, 1, 0,
      0, 0, 0, 1, 1, 1, 0, 0, 1, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    std::string filename = "assets/Tilesets/large.png";
    int tileSize = 8;
    int tileScale = 8;
    std::vector<Tile> tiles;
    for (int i = 0; i < initialMap.size(); i++) {
      TileType type = TileType::NONE;
      switch(initialMap[i]) {
        case 1:
          type = TileType::WALL;
          break;
        case 0:
          break;
      }
      Tile tile = Tile{i, 0, type};
      tiles.push_back(tile);
    }

    Entity* tilemapEntity = scene->createEntity("TILEMAP");
    tilemapEntity->addComponent<TilemapComponent>(
      filename,
      tiles,
      tileSize,
      tileScale,
      10,
      10
    );
    tilemapEntity->addComponent<TextureComponent>(filename);
  }
};

class AdvancedAutoTilingSetupSystem : public SetupSystem {
private:
  std::unordered_map<int, int> maskToTileIndex = {
    {2, 1}, {8, 2}, {10, 3}, {11, 4}, {16, 5}, {18, 6}, {22, 7}, {24, 8},
    {26, 9}, {27, 10}, {30, 11}, {31, 12}, {64, 13}, {66, 14}, {72, 15},
    {74, 16}, {75, 17}, {80, 18}, {82, 19}, {86, 20}, {88, 21}, {90, 22},
    {91, 23}, {94, 24}, {95, 25}, {104, 26}, {106, 27}, {107, 28}, {120, 29},
    {122, 30}, {123, 31}, {126, 32}, {127, 33}, {208, 34}, {210, 35},
    {214, 36}, {216, 37}, {218, 38}, {219, 39}, {222, 40}, {223, 41},
    {248, 42}, {250, 43}, {251, 44}, {254, 45}, {255, 46}, {0, 47}
  };


  bool isTile(const std::vector<Tile>& map, int x, int y, int w, int h) {
    return (x >= 0 && x < w && y >= 0 && y < h && map[y * w + x].type == TileType::WALL);
  }

public:
  void run() override {
    auto view = scene->r.view<TilemapComponent>();

    for (auto entity : view) {
      auto& tilemap = view.get<TilemapComponent>(entity);

      const int mapHeight = tilemap.width;
      const int mapWidth = tilemap.height;

      std::vector<Tile> newMap = tilemap.tiles;

      for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
          if (tilemap.tiles[y * mapWidth + x].type == TileType::WALL) {
            std::cout << "1";
            int mask = 0;

            bool north = isTile(tilemap.tiles, x, y-1, tilemap.width, tilemap.height);
            bool south = isTile(tilemap.tiles, x, y+1, tilemap.width, tilemap.height);
            bool west = isTile(tilemap.tiles, x-1, y, tilemap.width, tilemap.height);
            bool east = isTile(tilemap.tiles, x+1, y, tilemap.width, tilemap.height);

            // Cardinal directions
            if (north) mask |= 2;
            if (west) mask |= 8;
            if (east) mask |= 16;
            if (south) mask |= 64;

            // Corners (with redundancy check)
            if (north && west && isTile(tilemap.tiles, x-1, y-1, tilemap.width, tilemap.height)) mask |= 1;
            if (north && east && isTile(tilemap.tiles, x+1, y-1, tilemap.width, tilemap.height)) mask |= 4;
            if (south && west && isTile(tilemap.tiles, x-1, y+1, tilemap.width, tilemap.height)) mask |= 32;
            if (south && east && isTile(tilemap.tiles, x+1, y+1, tilemap.width, tilemap.height)) mask |= 128;

            // Map the mask to the tile index
            auto it = maskToTileIndex.find(mask);
            if (it != maskToTileIndex.end()) {
              newMap[y * mapWidth + x].tilemapIndex = it->second;
            } else {
              // If the mask doesn't have a mapping, use a default tile
              newMap[y * mapWidth + x].tilemapIndex = 47;
            }
          } else {
            std::cout << "0";
            newMap[y * mapWidth + x].tilemapIndex = -1;
          }
        }
        std::cout << std::endl;
      }

      tilemap.tiles = newMap;
    }
  }
};

class TilemapRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<TilemapComponent, TextureComponent>();
    for (auto e : view) {
      auto tmap = view.get<TilemapComponent>(e);
      auto tex = view.get<TextureComponent>(e);

      Texture* texture = TextureManager::GetTexture(tex.filename);

      int tileSize = tmap.tileSize * tmap.scale;
      int tilemapHeight = tmap.height;
      int tilemapWidth = tmap.width;

      for (int y = 0; y < tilemapHeight; y++) {
        for (int x = 0; x < tilemapWidth; x++) {
          int tileIndex = tmap.tiles[y * tilemapWidth + x].tilemapIndex;

          if (tileIndex >= 0) {
            int tileIndexX = tileIndex % 8;
            int tileIndexY = tileIndex / 8;

            SDL_Rect clip = {
              tileIndexX * tmap.tileSize,
              tileIndexY * tmap.tileSize,
              tmap.tileSize,
              tmap.tileSize,
            };

            texture->render(
              scene->renderer,
              x * tileSize,
              y * tileSize,
              tileSize,
              tileSize,
              &clip
            );
          }
        }
      }
    }
  }
}; 
