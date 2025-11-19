#include "Game.h"
#include "scenes/GameScene.h"

int main() {
  const int W = 800, H = 600;
  Game game("Tilemap ECS", W, H);
  game.setScene(new GameScene(W, H));
  game.run();
  return 0;
}
