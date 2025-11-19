#include "BreakoutScene.h"
#include <raylib.h>
#include <format>
#include <algorithm>
#include "ecs/ImGuiSystem.h"
#include "ecs/Globals.h"


void BreakoutScene::setup() {
  // Paddle, Ball, Blocks
  spawnPaddle();
  spawnBall();
  spawnBlocks(/*rows*/5, /*cols*/10, /*bw*/60, /*bh*/20, /*gap*/6, /*margin*/40);
  // addSystem(std::make_unique<ImGuiSystem>());
  imgui.setScene(this);
  imgui.setup();
}

void BreakoutScene::update(float dt) {
  if (ended) return;

  auto& r = registry();

  // ===== INPUT (paddle) =====
  {
    auto viewPaddle = r.view<Transform2D, Size, Velocity, Paddle>();
    for (auto e : viewPaddle) {
      auto& t = viewPaddle.get<Transform2D>(e);
      auto& s = viewPaddle.get<Size>(e);
      auto& v = viewPaddle.get<Velocity>(e);

      v.vel.x = 0.0f;
      const float speed = 280.0f;
      if (IsKeyDown(KEY_LEFT))  v.vel.x = -speed;
      if (IsKeyDown(KEY_RIGHT)) v.vel.x =  speed;

      t.pos.x += v.vel.x * dt;

      // clamp en pantalla
      const float left  = 0.0f;
      const float right = static_cast<float>(width) - s.size.x;
      t.pos.x = std::clamp(t.pos.x, left, right);
    }
  }

  // ===== PHYSICS (ball) =====
  {
    auto viewBall = r.view<Transform2D, Size, Velocity, Ball, SpeedScale>();
    // substepping simple para reducir túneleo
    const int   substeps = std::max(1, (int)std::ceil(dt * 120.0f)); // hasta 120 Hz
    const float step     = (substeps > 0) ? (dt / substeps) : dt;

    for (auto e : viewBall) {
      auto& t     = viewBall.get<Transform2D>(e);
      auto& s     = viewBall.get<Size>(e);
      auto& v     = viewBall.get<Velocity>(e);
      auto& scale = viewBall.get<SpeedScale>(e);

      for (int it = 0; it < substeps; ++it) {
        // Integración
        t.pos.x += v.vel.x * step * scale.factor;
        t.pos.y += v.vel.y * step * scale.factor;

        // --- paredes ---
        // Izquierda
        if (t.pos.x <= 0.0f) {
          t.pos.x = 0.0f;
          v.vel.x *= -1.0f;
          scale.factor = std::min(scale.factor * 1.05f, scale.maxFactor);
        }
        // Derecha
        if (t.pos.x + s.size.x >= width) {
          t.pos.x = width - s.size.x;
          v.vel.x *= -1.0f;
          scale.factor = std::min(scale.factor * 1.05f, scale.maxFactor);
        }
        // Techo
        if (t.pos.y <= 0.0f) {
          t.pos.y = 0.0f;
          v.vel.y *= -1.0f;
          scale.factor = std::min(scale.factor * 1.05f, scale.maxFactor);
        }
        // Fondo
        if (t.pos.y + s.size.y >= height) {
          ended = true;
          endMsg = "Game Over";
          break; // salir del substepping
        }

        // Rectángulo de bola (actualizado)
        Rectangle ballRect{ t.pos.x, t.pos.y, s.size.x, s.size.y };

        // --- colisión con paddle ---
        {
          auto vp = r.view<Transform2D, Size, Velocity, Paddle>();
          for (auto p : vp) {
            auto& pt = vp.get<Transform2D>(p);
            auto& ps = vp.get<Size>(p);
            auto& pv = vp.get<Velocity>(p);
            Rectangle paddleRect{ pt.pos.x, pt.pos.y, ps.size.x, ps.size.y };

            if (CheckCollisionRecs(ballRect, paddleRect)) {
              // coloca la bola encima del paddle
              t.pos.y = pt.pos.y - s.size.y;
              // rebote hacia arriba
              v.vel.y = -std::abs(v.vel.y);

              // controla el ángulo según el punto de impacto
              const float ballCX = t.pos.x + s.size.x * 0.5f;
              const float padCX  = pt.pos.x + ps.size.x * 0.5f;
              float offset = (ballCX - padCX) / (ps.size.x * 0.5f); // [-1,1]
              offset = std::clamp(offset, -1.0f, 1.0f);

              // empuja la X según offset y añade un poco de "english" del paddle
              v.vel.x += offset * 160.0f + pv.vel.x * 0.35f;

              // asegura que la componente vertical no quede casi cero
              if (std::abs(v.vel.y) < 60.0f) v.vel.y = -60.0f;

              scale.factor = std::min(scale.factor * 1.07f, scale.maxFactor);

              // actualizar rect de bola tras corrección
              ballRect.x = t.pos.x; ballRect.y = t.pos.y;
              break;
            }
          }
        }

        // --- colisión con bloques
        {
          auto vb = r.view<Transform2D, Size, Block>();
          for (auto b : vb) {
            auto& bt = vb.get<Transform2D>(b);
            auto& bs = vb.get<Size>(b);
            Rectangle blockRect{ bt.pos.x, bt.pos.y, bs.size.x, bs.size.y };

            if (CheckCollisionRecs(ballRect, blockRect)) {
              // calcular solapes
              const float leftA   = ballRect.x;
              const float rightA  = ballRect.x + ballRect.width;
              const float topA    = ballRect.y;
              const float bottomA = ballRect.y + ballRect.height;

              const float leftB   = blockRect.x;
              const float rightB  = blockRect.x + blockRect.width;
              const float topB    = blockRect.y;
              const float bottomB = blockRect.y + blockRect.height;

              const float overlapX = std::min(rightA, rightB) - std::max(leftA, leftB);
              const float overlapY = std::min(bottomA, bottomB) - std::max(topA, topB);

              if (overlapX < overlapY) {
                // resolver lateral
                if (ballRect.x < blockRect.x) t.pos.x -= overlapX;
                else                           t.pos.x += overlapX;
                v.vel.x *= -1.0f;
              } else {
                // resolver vertical
                if (ballRect.y < blockRect.y) t.pos.y -= overlapY;
                else                           t.pos.y += overlapY;
                v.vel.y *= -1.0f;
              }

              // elimina bloque y acelera un poco
              r.destroy(b);
              scale.factor = std::min(scale.factor * 1.04f, scale.maxFactor);

              // actualiza rect tras corrección
              ballRect.x = t.pos.x; ballRect.y = t.pos.y;

              // una colisión por subpaso basta
              break;
            }
          }
        }

        // ¿win?
        if (r.view<Block>().empty()) {
          ended = true;
          endMsg = "You Win!";
          break;
        }
      } // substeps
    }   // bolas
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

  if (gPaused) {
    const char* txt = "PAUSED";
    int fs = 48;
    int tw = MeasureText(txt, fs);
    DrawText(txt, (GetScreenWidth()-tw)/2, 20, fs, YELLOW);
  }

  imgui.render();
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
