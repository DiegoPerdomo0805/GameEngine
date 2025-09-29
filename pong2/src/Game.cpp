#include <raylib.h>
#include <print>
#include <vector>

#include "Game.h"

std::vector<Rectangle> blocks;
int block_rows = 5;
int block_cols = 10;
float block_width = 60;
float block_height = 20;
float block_spacing = 5;


Rectangle ball;
Rectangle paddle;

int ball_sx = 150; 
int ball_sy = 150;
int paddle_sx = 200;

Game::Game(const char* title, int width, int height)
: screen_width(width), screen_height(height) {
  InitWindow(width, height, title);
  std::println("GAME STARTED");
  isRunning = true;
  counter = 0;
}

Game::~Game() {

}

void Game::setup() {
  SetTargetFPS(60);
  
  // ball and paddle
  ball = Rectangle{10, 10, 15, 15};
  paddle = Rectangle{(float)(screen_width/2) - ball.width*5, (float)screen_height - 15, ball.width*10, 15};

  //grid of rectangles

  blocks.clear();
  for (int row = 0; row < block_rows; ++row) {
    for (int col = 0; col < block_cols; ++col) {
      Rectangle block = {
        col * (block_width + block_spacing) + 30,
        row * (block_height + block_spacing) + 30,
        block_width,
        block_height
      };
      blocks.push_back(block);
    }
  }
  
}

void Game::frame_start() {
  std::println("==== FRAME {} START ====", counter);
  BeginDrawing();
}

void Game::frame_end() {
  std::println("==== FRAME END ====");
  EndDrawing();
  counter++;
}

void Game::handle_events() {
  float dT = GetFrameTime();  // seconds
  if (WindowShouldClose()) {
    isRunning = false;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    paddle.x += paddle_sx * dT;
  }

  if (IsKeyDown(KEY_LEFT)) {
    paddle.x -= paddle_sx * dT;
  }
}

void Game::update() {
  float dT = GetFrameTime();  // seconds
  
  // ball: no acceleration 
  if (ball.x >= screen_width) {
    ball_sx *= -1.00f;
  }
  if (ball.x < 0) {
    ball_sx *= -1.00f;
  } 
  
  
  if (ball.y >= screen_height) {
    std::println("YOU FAIL");
    exit(1);
  }

  // ball: acceleration
  if (CheckCollisionRecs(ball, paddle)) {
    ball_sy *= -1.05f;
    ball_sx = paddle_sx;
  }
  if (ball.y < 0) {
    ball_sy *= -1.05f;
  }

  for (size_t i = 0; i < blocks.size(); ++i) {
    if (CheckCollisionRecs(ball, blocks[i])) {
      ball_sy *= -1;
      blocks.erase(blocks.begin() + i);
      break;  // one block hit at a time
    }
  }
  
  // win condition
  if (blocks.empty()) {
    std::println("YOU WIN!");
    exit(0);
  }
  
  
  ball.x += ball_sx * dT;
  ball.y += ball_sy * dT;
}

void Game::render() {
  ClearBackground(GRAY);
  DrawText(
    std::format("FPS: {}", GetFPS()).c_str(),
    10,
    10,
    20,
    GRAY
  );
  
  DrawRectangleRec(ball, BLACK);
  DrawRectangleRec(paddle, BLACK);

  for (size_t i = 0; i < blocks.size(); ++i) {
    Color color = Fade(RED, 1.0f);
    int row = (blocks[i].y - 30) / (block_height + block_spacing);
    switch (row) {
      case 0: color = RED; break;
      case 1: color = ORANGE; break;
      case 2: color = YELLOW; break;
      case 3: color = GREEN; break;
      case 4: color = BLUE; break;
      default: color = PURPLE; break;
    }

    DrawRectangleRec(blocks[i], color);
  }
}

void Game::clean() {
  CloseWindow();
}

bool Game::running() {
  return isRunning;
}