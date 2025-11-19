#include "GameScene.h"

void GameScene::setup() {
  addSystem(std::make_unique<TilemapSystem>(width, height));

  imgui.setScene(this);
  imgui.setup();
}

void GameScene::update(float dt) {
  Scene::update(dt);
}

void GameScene::render() {
  Scene::render();
  imgui.render();
}

