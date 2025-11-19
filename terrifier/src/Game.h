// Game.h
#pragma once
#include <memory>
#include <raylib.h>

class Scene;  // forward declaration

class Game {
public:
  Game(const char* title, int width, int height);
  ~Game();

  void run();

  void setup();
  void frameStart();
  void handleEvents();
  void update();
  void render();
  void frameEnd();
  void clean();

  bool running() const;

  void setScene(Scene* newScene);   // takes ownership
  Scene* getCurrentScene() const;

  int width()  const { return screen_width; }
  int height() const { return screen_height; }

private:
  int   screen_width;
  int   screen_height;

  bool  isRunning{false};
  int   frameCount{0};
  float dT{0.0f};
  float FPS{0.0f};

  std::unique_ptr<Scene> currentScene; // forward-declared type is OK in header
};
