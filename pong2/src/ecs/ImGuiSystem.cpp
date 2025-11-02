#include "ecs/ImGuiSystem.h"
#include "ecs/Scene.h"
#include "ecs/components.h"
#include "ecs/Globals.h"

#include "rlImGui.h"
#include "imgui.h"
#include <algorithm>

// Helpers to show per-entity component editors
static void ShowComponentProperties(entt::registry& r, entt::entity e) {
  if (r.all_of<Transform2D>(e)) {
    auto &t = r.get<Transform2D>(e);
    ImGui::SeparatorText("Transform2D");
    ImGui::DragFloat2("pos", &t.pos.x, 1.0f, 0.0f, (float)GetScreenWidth());
  }

  if (r.all_of<Velocity>(e)) {
    auto &v = r.get<Velocity>(e);
    ImGui::SeparatorText("Velocity");
    ImGui::DragFloat2("vel", &v.vel.x, 1.0f, -1000.0f, 1000.0f);
  }

  if (r.all_of<Size>(e)) {
    auto &s = r.get<Size>(e);
    ImGui::SeparatorText("Size");
    ImGui::DragFloat2("size", &s.size.x, 1.0f, 0.0f, 2000.0f);
  }

  if (r.all_of<Tint>(e)) {
    auto &tint = r.get<Tint>(e);
    ImGui::SeparatorText("Tint");
    float col[4] = {
      tint.color.r / 255.0f,
      tint.color.g / 255.0f,
      tint.color.b / 255.0f,
      tint.color.a / 255.0f
    };
    if (ImGui::ColorEdit4("color", col)) {
      tint.color.r = (unsigned char)std::clamp(col[0]*255.0f, 0.0f, 255.0f);
      tint.color.g = (unsigned char)std::clamp(col[1]*255.0f, 0.0f, 255.0f);
      tint.color.b = (unsigned char)std::clamp(col[2]*255.0f, 0.0f, 255.0f);
      tint.color.a = (unsigned char)std::clamp(col[3]*255.0f, 0.0f, 255.0f);
    }
  }
}

ImGuiSystem::~ImGuiSystem() {
  rlImGuiShutdown();
}

void ImGuiSystem::setup() {
  rlImGuiSetup(true); // enable docking
}

void ImGuiSystem::render() {
  rlImGuiBegin();

  static bool first = true;
  if (first) {
    ImGui::SetNextWindowPos({10, 10}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({260, 120}, ImGuiCond_Always);
  }

  // ---- Game panel (Pause) ----
  ImGui::Begin("Game");
  ImGui::Checkbox("Paused", &gPaused);  // <-- pause toggle
  ImGui::Text("FPS: %d", GetFPS());
  ImGui::End();

  // ---- Components / Inspector ----
  ImGui::Begin("Entities");
  auto view = scene->registry().view<Name>();
  for (auto e : view) {
    auto &name = view.get<Name>(e);
    // Stable tree id per entity
    if (ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)e, ImGuiTreeNodeFlags_DefaultOpen, "%s", name.tag.c_str())) {
      ShowComponentProperties(scene->registry(), e);
      ImGui::TreePop();
    }
  }
  ImGui::End();

  rlImGuiEnd();
}
