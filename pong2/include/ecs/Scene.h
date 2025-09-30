#pragma once
#include <entt/entt.hpp>
#include <vector>
#include <memory>
#include "ecs/systems.h"

class System; // fwd

class Scene {
public:
  virtual ~Scene() = default;

  virtual void setup() {}
  virtual void update(float dt) { for (auto& s : systems_) s->update(dt); }
  virtual void render() { for (auto& s : systems_) s->render(); }

  entt::registry& registry() { return r; }

  void addSystem(std::unique_ptr<System> sys) {
    sys->setScene(this);
    systems_.push_back(std::move(sys));
  }

protected:
  entt::registry r;

private:
  std::vector<std::unique_ptr<System>> systems_;
};
