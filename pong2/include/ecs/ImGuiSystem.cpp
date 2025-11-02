#include "ecs/ImGuiSystem.h"
#include "ecs/Scene.h"
#include "ecs/components.h"  // Name, Transform2D, Velocity, Size, Tint...
#include "ecs/Globals.h"

#include "rlImGui.h"
#include "imgui.h"
#include <algorithm>

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
      tint.color.r/255.f, tint.color.g/255.f, tint.color.b/255.f, tint.color.a/255.f
    };
    if (ImGui::ColorEdit4("color", col)) {
      tint.color.r = (unsigned char)std::clamp(col[0]*255.f, 0.f, 255.f);
      tint.color.g = (unsigned char)std::clamp(col[1]*255.f, 0.f, 255.f);
      tint.color.b = (unsigned char)std::clamp(col[2]*255.f, 0.f, 255.f);
      tint.color.a = (unsigned char)std::clamp(col[3]*255.f, 0.f, 255.f);
    }
  }
}

ImGuiSystem::~ImGuiSystem() { rlImGuiShutdown(); }
void ImGuiSystem::setup()   { rlImGuiSetup(true); }

void ImGuiSystem::render() {
  rlImGuiBegin();

  // ---- Panel 1: Juego (pausa) ----
  {
    ImGui::SetNextWindowPos(ImVec2(10,10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(260,120), ImGuiCond_FirstUseEver);
    ImGui::Begin("Game");
    // Botón para pausar/reanudar (tal como pide la tarea)
    if (ImGui::Button(gPaused ? "Resume" : "Pause")) {
      gPaused = !gPaused;
    }
    ImGui::SameLine();
    ImGui::Checkbox("Paused", &gPaused); // extra, visible
    ImGui::Text("FPS: %d", GetFPS());
    ImGui::End();
  }

  // ---- Panel 2: Entidades + edición de componentes ----
  {
    ImGui::SetNextWindowPos(ImVec2(10,140), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(360,420), ImGuiCond_FirstUseEver);
    ImGui::Begin("Entities");

    auto view = scene->registry().view<Name>();
    for (auto e : view) {
      auto &name = view.get<Name>(e);
      if (ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)e, ImGuiTreeNodeFlags_DefaultOpen, "%s", name.tag.c_str())) {
        ShowComponentProperties(scene->registry(), e); // aquí se EDITAN valores
        ImGui::TreePop();
      }
    }

    ImGui::End();
  }

  rlImGuiEnd();
}
