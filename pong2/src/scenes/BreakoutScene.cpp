#include "BreakoutScene.h"
#include <raylib.h>
#include <format>
#include <algorithm>

void BreakoutScene::setup() {
  // Paddle, Ball, Blocks
  spawnPaddle();
  spawnBall();
  spawnBlocks(/*rows*/5, /*cols*/10, /*bw*/60, /*bh*/20, /*gap*/6, /*margin*/30);
}

void BreakoutScene::update(float dt) {
  if (ended) return;

  auto& r = registry();

  // ===== INPUT (paddle) =====
  auto viewPaddle = r.view<Transform2D, Size, Velocity, Paddle>();
  for (auto e : viewPaddle) {
    auto& t = viewPaddle.get<Transform2D>(e);
    auto& s = viewPaddle.get<Size>(e);
    auto& v = viewPaddle.get<Velocity>(e);

    v.vel.x = 0.0f;
    float speed = 280.0f;
    if (IsKeyDown(KEY_LEFT))  v.vel.x = -speed;
    if (IsKeyDown(KEY_RIGHT)) v.vel.x =  speed;

    t.pos.x += v.vel.x * dt;

    // clamp dentro pantalla
    float left = 0.0f;
    float right = static_cast<float>(width) - s.size.x;
    t.pos.x = std::clamp(t.pos.x, left, right);
  }

  // ===== PHYSICS (ball) =====
  auto viewBall = r.view<Transform2D, Size, Velocity, Ball, SpeedScale>();
  for (auto e : viewBall) {
    auto& t = viewBall.get<Transform2D>(e);
    auto& s = viewBall.get<Size>(e);
    auto& v = viewBall.get<Velocity>(e);
    auto& scale = viewBall.get<SpeedScale>(e);

    t.pos.x += v.vel.x * dt * scale.factor;
    t.pos.y += v.vel.y * dt * scale.factor;

    // paredes
    bool hitTop    = (t.pos.y <= 0);
    bool hitBottom = (t.pos.y + s.size.y >= height);
    bool hitLeft   = (t.pos.x <= 0);
    bool hitRight  = (t.pos.x + s.size.x >= width);

    if (hitLeft || hitRight) {
      // GAME OVER al tocar pared izq/der?
      ended = true;
      endMsg = "Game Over (Left/Right Wall)";
    } else {
      if (hitTop) {
        t.pos.y = std::max(0.0f, t.pos.y);
        v.vel.y *= -1;
        scale.factor = std::min(scale.factor * 1.05f, scale.maxFactor);
      }
      if (hitBottom) {
        t.pos.y = height - s.size.y;
        v.vel.y *= -1;
        scale.factor = std::min(scale.factor * 1.05f, scale.maxFactor);
      }
    }

    // colisión con paddle
    Rectangle ballRect{t.pos.x, t.pos.y, s.size.x, s.size.y};
    auto vp = r.view<Transform2D, Size, Paddle>();
    for (auto p : vp) {
      auto& pt = vp.get<Transform2D>(p);
      auto& ps = vp.get<Size>(p);
      Rectangle paddleRect{pt.pos.x, pt.pos.y, ps.size.x, ps.size.y};
      if (CheckCollisionRecs(ballRect, paddleRect)) {
        t.pos.y = pt.pos.y - s.size.y; // sacar la bola encima del paddle
        v.vel.y *= -1;
        scale.factor = std::min(scale.factor * 1.07f, scale.maxFactor);
        break;
      }
    }

    // colisión con bloques (eliminar bloque y rebotar)
    auto vb = r.view<Transform2D, Size, Block>();
    for (auto b : vb) {
      auto& bt = vb.get<Transform2D>(b);
      auto& bs = vb.get<Size>(b);
      Rectangle blockRect{bt.pos.x, bt.pos.y, bs.size.x, bs.size.y};
      if (CheckCollisionRecs(ballRect, blockRect)) {
        r.destroy(b);
        v.vel.y *= -1;
        scale.factor = std::min(scale.factor * 1.04f, scale.maxFactor);
        break; // 1 bloque por frame
      }
    }

    // win? (sin bloques)
    if (r.view<Block>().empty()) {
      ended = true;
      endMsg = "You Win!";
    }
  }
}

void BreakoutScene::render() {
  auto& r = registry();

  // draw blocks
  auto vb = r.view<Transform2D, Size, Tint, Block>();
  for (auto e : vb) {
    auto& t = vb.get<Transform2D>(e);
    auto& s = vb.get<Size>(e);
    auto& c = vb.get<Tint>(e);
    DrawRectangleV(t.pos, s.size, c.color);
  }

  // draw paddle
  auto vp = r.view<Transform2D, Size, Tint, Paddle>();
  for (auto e : vp) {
    auto& t = vp.get<Transform2D>(e);
    auto& s = vp.get<Size>(e);
    auto& c = vp.get<Tint>(e);
    DrawRectangleV(t.pos, s.size, c.color);
  }

  // draw ball
  auto vball = r.view<Transform2D, Size, Tint, Ball>();
  for (auto e : vball) {
    auto& t = vball.get<Transform2D>(e);
    auto& s = vball.get<Size>(e);
    auto& c = vball.get<Tint>(e);
    DrawRectangleV(t.pos, s.size, c.color);
  }

  if (ended && endMsg) {
    const int fs = 32;
    const int tw = MeasureText(endMsg, fs);
    DrawText(endMsg, (GetScreenWidth()-tw)/2, GetScreenHeight()/2 - fs, fs, YELLOW);
  }

  // overlay
  DrawText(std::format("FPS: {}", GetFPS()).c_str(), 10, 10, 20, DARKGRAY);
}

void BreakoutScene::spawnPaddle() {
  auto& r = registry();
  auto e = r.create();
  Entity ent(e, this);

  float w = 110, h = 18;
  ent.add<Name>(Name{"paddle"});
  ent.add<Transform2D>(Transform2D{ Vector2{ (float)width/2 - w/2, (float)height - 40 } });
  ent.add<Size>(Size{ Vector2{ w, h } });
  ent.add<Tint>(Tint{ LIGHTGRAY });
  ent.add<Velocity>(Velocity{ Vector2{ 0, 0 } });
  ent.add<Collider>();
  ent.add<Paddle>();
}

void BreakoutScene::spawnBall() {
  auto& r = registry();
  auto e = r.create();
  Entity ent(e, this);

  Vector2 sz{14, 14};
  Vector2 start{ (float)width/2 - sz.x/2, (float)height/2 };
  Vector2 v{ 40, -200 };

  ent.add<Name>(Name{"ball"});
  ent.add<Transform2D>(Transform2D{ start });
  ent.add<Size>(Size{ sz });
  ent.add<Tint>(Tint{ WHITE });
  ent.add<Velocity>(Velocity{ v });
  ent.add<Collider>();
  ent.add<Ball>();
  ent.add<SpeedScale>(SpeedScale{ 1.0f, 2.6f });
}

void BreakoutScene::spawnBlocks(int rows, int cols, float bw, float bh, float gap, float margin) {
  auto& r = registry();
  Color rowColors[6] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };

  float totalW = cols * bw + (cols-1) * gap;
  float startX = (float)width/2 - totalW/2;
  float y = margin;

  for (int i=0;i<rows;i++) {
    float x = startX;
    for (int j=0;j<cols;j++) {
      auto e = r.create();
      Entity ent(e, this);
      ent.add<Name>(Name{std::format("block_{}_{}", i, j)});
      ent.add<Transform2D>(Transform2D{ Vector2{ x, y } });
      ent.add<Size>(Size{ Vector2{ bw, bh } });
      ent.add<Tint>(Tint{ rowColors[i % 6] });
      ent.add<Collider>();
      ent.add<Block>();
      x += bw + gap;
    }
    y += bh + gap;
  }
}
