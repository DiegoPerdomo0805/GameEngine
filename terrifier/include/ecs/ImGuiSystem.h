// #pragma once
// #include "ecs/systems.h"

// class ImGuiSystem : public System {
// public:
//   ~ImGuiSystem() override;
//   void setup() override;
//   void render() override;
// };

#pragma once
#include "ecs/systems.h"

class Scene;

struct ImGuiSystem {
    void setup();
    void render();
    ~ImGuiSystem();
    void setScene(Scene* s) { scene = s; }
private:
    Scene* scene = nullptr;
};