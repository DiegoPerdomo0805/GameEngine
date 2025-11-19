#pragma once

#include "ecs/Scene.h"
#include "ecs/TilemapSystem.h"
#include "ecs/ImGuiSystem.h"

class GameScene : public Scene {
public:
  GameScene(int w, int h) : width(w), height(h) {}

  void setup() override;
  void update(float dt) override;
  void render() override;

private:
  int width;
  int height;

  ImGuiSystem imgui;
};

