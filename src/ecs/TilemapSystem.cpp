#include "ecs/TilemapSystem.h"

TilemapSystem::TilemapSystem(int screenW, int screenH)
  : screenWidth(screenW), screenHeight(screenH) {}

TilemapSystem::~TilemapSystem() {
  if (tileset.id != 0) {
    UnloadTexture(tileset);
  }
}

void TilemapSystem::setup() {
  initMap();
  autotile();
  tileset = LoadTexture("tileset.png");
}

void TilemapSystem::initMap() {
  // Simple hardcoded map: solid border and a few filled islands.
  for (int y = 0; y < MAP_HEIGHT; ++y) {
    for (int x = 0; x < MAP_WIDTH; ++x) {
      bool border = (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1);
      map[y][x] = border ? 1 : 0;
    }
  }

  // Add a couple of blocks inside the map for variety.
  for (int y = 5; y <= 8; ++y) {
    for (int x = 5; x <= 10; ++x) {
      map[y][x] = 1;
    }
  }
}

void TilemapSystem::autotile() {
  // Use a 4-bit mask based on cardinal neighbors (N,E,S,W).
  // Bit 0: North, 1: East, 2: South, 3: West.
  int temp[MAP_HEIGHT][MAP_WIDTH]{};

  for (int y = 0; y < MAP_HEIGHT; ++y) {
    for (int x = 0; x < MAP_WIDTH; ++x) {
      if (map[y][x] == 0) {
        temp[y][x] = 0;
        continue;
      }

      int mask = 0;
      if (y > 0 && map[y - 1][x] != 0)            mask |= 1 << 0; // N
      if (x < MAP_WIDTH - 1 && map[y][x + 1] != 0) mask |= 1 << 1; // E
      if (y < MAP_HEIGHT - 1 && map[y + 1][x] != 0) mask |= 1 << 2; // S
      if (x > 0 && map[y][x - 1] != 0)            mask |= 1 << 3; // W

      // Base tile ID 1, variant determined by mask (1..16).
      temp[y][x] = 1 + mask;
    }
  }

  for (int y = 0; y < MAP_HEIGHT; ++y) {
    for (int x = 0; x < MAP_WIDTH; ++x) {
      map[y][x] = temp[y][x];
    }
  }
}

void TilemapSystem::render() {
  const int offsetX = (screenWidth  - MAP_WIDTH  * TILE_SIZE) / 2;
  const int offsetY = (screenHeight - MAP_HEIGHT * TILE_SIZE) / 2;

  for (int y = 0; y < MAP_HEIGHT; ++y) {
    for (int x = 0; x < MAP_WIDTH; ++x) {
      int tileId = map[y][x];
      if (tileId == 0) continue; // empty

      int drawX = offsetX + x * TILE_SIZE;
      int drawY = offsetY + y * TILE_SIZE;

      if (tileset.id != 0 && tileset.width >= TILE_SIZE && tileset.height >= TILE_SIZE) {
        int tilesPerRow = tileset.width / TILE_SIZE;
        if (tilesPerRow <= 0) tilesPerRow = 1;

        int variantIndex = tileId - 1;
        int sx = (variantIndex % tilesPerRow) * TILE_SIZE;
        int sy = (variantIndex / tilesPerRow) * TILE_SIZE;

        Rectangle src{ (float)sx, (float)sy, (float)TILE_SIZE, (float)TILE_SIZE };
        Vector2   dst{ (float)drawX, (float)drawY };
        DrawTextureRec(tileset, src, dst, WHITE);
      } else {
        // Fallback: draw colored rectangles if texture not available.
        Color col = { 100, (unsigned char)(150 + (tileId * 5) % 100), 200, 255 };
        DrawRectangle(drawX, drawY, TILE_SIZE, TILE_SIZE, col);
      }
    }
  }
}

