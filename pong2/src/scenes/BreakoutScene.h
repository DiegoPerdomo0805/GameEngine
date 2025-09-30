#pragma once
#include "ecs/Scene.h"
#include "ecs/Entity.h"
#include "ecs/components.h"
#include "ecs/systems.h"
#include <vector>

class BreakoutScene : public Scene {
public:
  BreakoutScene(int w, int h): width(w), height(h) {}
  void setup() override;
  void update(float dt) override;
  void render() override;

private:
  int width, height;

  // util
  void spawnPaddle();
  void spawnBall();
  void spawnBlocks(int rows, int cols, float bw, float bh, float gap, float margin);

  // game state
  bool ended = false;
  const char* endMsg = nullptr;
};
