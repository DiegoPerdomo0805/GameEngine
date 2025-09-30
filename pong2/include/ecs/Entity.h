#pragma once
#include <entt/entt.hpp>
#include "ecs/Scene.h"
#include <utility>

class Entity {
public:
  Entity() = default;
  Entity(entt::entity h, Scene* s): handle(h), scene(s) {}

  template<typename T, typename... Args>
  // validate empty types
  void add(Args&&... args) {
    scene->registry().emplace_or_replace<T>(handle, std::forward<Args>(args)...);
  }

  template<typename T>
  bool has() const {
    return scene->registry().all_of<T>(handle);
  }

  template<typename T>
  T& get() {
    return scene->registry().get<T>(handle);
  }

  entt::entity id() const { return handle; }

private:
  entt::entity handle{entt::null};
  Scene* scene{nullptr};
};
