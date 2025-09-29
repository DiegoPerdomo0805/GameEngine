#pragma once
#include <raylib.h>
#include <string>

struct Name { std::string tag; };

struct Transform { Vector2 pos; };
struct Velocity  { Vector2 vel; };
struct Size      { Vector2 size; };
struct Tint      { Color color; };
struct Collider  {};          // marcador
struct Paddle    {};          // marcador
struct Ball      {};          // marcador
struct Block     {};          // marcador

struct SpeedScale { float factor = 1.0f; float maxFactor = 2.5f; };
