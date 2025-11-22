#include "raylib.h"
#include "rlImGui.h"  
#include "imgui.h"

int main() {
  const int screenWidth = 800;
  const int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "raylib basic window");
  rlImGuiSetup(true);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // start ImGui Conent
      rlImGuiBegin();
        // show ImGui Content
        bool open = true;
        ImGui::ShowDemoWindow(&open);

        open = true;
        if (ImGui::Begin("Test Window", &open))
        {
          ImGui::TextUnformatted(ICON_FA_JEDI);

          // rlImGuiImage(&image);
        }
        ImGui::End();

        // end ImGui Content
      rlImGuiEnd();

    EndDrawing();
  }
  rlImGuiShutdown();
  CloseWindow();
  return 0;
}