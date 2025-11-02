#pragma once
#include "ecs/systems.h"

class ImGuiSystem : public System {
public:
  ~ImGuiSystem() override;
  void setup() override;
  void render() override;
};
