#include "config.h"
#include "imgui.h"
#include "utils.h"
#include <cstdio>

void SceneConfig::Setup() { printf("fodase"); }

void SceneConfig::Show() {
  ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(20000, 20000));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  if (ImGui::Begin("Scene Config", &Open, 0)) {
    ImGui::SeparatorText("Camera");
    Vec3Menu("Position", &cameraPosition, 0.1, -10000, 10000, true);
    Vec2Menuf("Rotation", &cameraRotation, 0.1, -180, 180);

    ImGui::RadioButton("Perspective", &cameraProjection, CAMERA_PERSPECTIVE);
    ImGui::SameLine();
    ImGui::RadioButton("Orthographic", &cameraProjection,
                       CAMERA_ORTHOGRAPHIC);
    ImGui::DragFloat("Zoom", &cameraZoom, 0.1);

    if (cameraProjection == CAMERA_PERSPECTIVE) {
      ImGui::DragFloat("FOV", &cameraFOV);
    } else {
      ImGui::DragFloat("FOV", &cameraZoom);
    }
    ImGui::SeparatorText("Sprite");
    Vec2Menui("Sprite Size", &spriteSize,1,0,10000, false);
  }
  ImGui::End();
  ImGui::PopStyleVar();
}

void SceneConfig::Vec3Menu(const char *label, Vector3 *vec, float speed,
              float min, float max, bool help) {
  float list[3] = {vec->x, vec->y, vec->z};
  ImGui::DragFloat3(label, list, speed, min, max);
  if (help) {
    ImGui::SameLine();
    HelpMarker("Click and drag to edit value.\n"
               "Hold SHIFT/ALT for faster/slower edit.\n"
               "Double-click or CTRL+click to input value.");
  }
  vec->x = list[0];
  vec->y = list[1];
  vec->z = list[2];
}

void SceneConfig::Vec2Menuf(const char *label, Vector2 *vec, float speed,
              float min, float max, bool help) {
  float list[2] = {vec->x, vec->y};
  ImGui::DragFloat2(label, list, speed, min, max);
  if (help) {
    ImGui::SameLine();
    HelpMarker("Click and drag to edit value.\n"
               "Hold SHIFT/ALT for faster/slower edit.\n"
               "Double-click or CTRL+click to input value.");
  }
  vec->x = list[0];
  vec->y = list[1];
}

void SceneConfig::Vec2Menui(const char *label, Vector2 *vec,float speed,
              float min, float max, bool help) {
  int list[2] = {(int)vec->x, (int)vec->y};
  ImGui::DragInt2(label, list,speed,min,max);
  if (help) {
    ImGui::SameLine();
    HelpMarker("Click and drag to edit value.\n"
               "Hold SHIFT/ALT for faster/slower edit.\n"
               "Double-click or CTRL+click to input value.");
  }
  vec->x = list[0];
  vec->y = list[1];
}