#include "Game.h"
#include <raylib.h>
#include <print>
#include <format>
#include "../include/ecs/Scene.h"

Game::Game(const char* title, int width, int height)
: screen_width(width), screen_height(height) {
  InitWindow(width, height, title);
  SetTargetFPS(60);
  std::println("Game Start!");
  isRunning = true;
  frameCount = 0;
  dT = 0.0f;
  FPS = 0.0f;
}

// Game::~Game() { clean(); }
Game::~Game() = default;

void Game::setup() {
  if (currentScene) currentScene->setup();
}

void Game::frameStart() {
  dT = GetFrameTime();
}

void Game::frameEnd() {
  frameCount++;
  FPS = (float)GetFPS();
}

void Game::handleEvents() {
  if (WindowShouldClose()) isRunning = false;
}

void Game::update() {
  if (currentScene) currentScene->update(dT);
}

void Game::render() {
  BeginDrawing();
  ClearBackground(BLACK);
  if (currentScene) currentScene->render();
  EndDrawing();
}

void Game::clean() {
  if (!WindowShouldClose()) CloseWindow();
  std::println("Game Over.");
}

bool Game::running() const { return isRunning; }

void Game::run() {
  setup();
  while (running()) {
    frameStart();
    handleEvents();
    update();
    render();
    frameEnd();
  }
  clean();
}

void Game::setScene(Scene* newScene) { currentScene.reset(newScene); }
Scene* Game::getCurrentScene() const { return currentScene.get(); }
