#pragma once
#include "ecs/Scene.h"
#include <raylib.h>

class System {
protected:
  Scene* scene = nullptr;
public:
  virtual ~System() = default;
  virtual void setScene(Scene* s) { scene = s; }
  virtual void setup() {}
  virtual void update(float dt) {}
  virtual void render() {}
};
