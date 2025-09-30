#include "Game.h"
#include "scenes/BreakoutScene.h"

int main() {
  const int W = 800, H = 600;
  Game game("Breakout ECS", W, H);
  game.setScene(new BreakoutScene(W, H));
  game.run();
  return 0;
}
