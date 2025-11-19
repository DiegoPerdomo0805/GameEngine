#pragma once

#include "ecs/systems.h"
#include <raylib.h>

class TilemapSystem : public System {
public:
  TilemapSystem(int screenW, int screenH);
  ~TilemapSystem() override;

  void setup() override;
  void render() override;

private:
  static constexpr int TILE_SIZE   = 32;
  static constexpr int MAP_WIDTH   = 25;
  static constexpr int MAP_HEIGHT  = 19;

  int  screenWidth;
  int  screenHeight;
  int  map[MAP_HEIGHT][MAP_WIDTH]{};
  Texture2D tileset{};

  void initMap();
  void autotile();
};

