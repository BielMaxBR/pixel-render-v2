#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "r3d.h"

class DocumentWindow {
public:
  bool Open = false;

  RenderTexture ViewTexture;

  virtual void Setup() = 0;
  virtual void Shutdown() = 0;
  virtual void Show() = 0;
  virtual void Update() = 0;

  bool Focused = false;

  Rectangle ContentRect = {0};
};

class Scene : public DocumentWindow {
public:
  Camera3D Camera = {0};
  float x = 0;
  void Setup() override {
    ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    Camera.fovy = 45;
    Camera.up.y = 1;
    Camera.position.y = 3;
    Camera.position.z = -25;

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  };
  void Update() override {
    if (!Open)
      return;
    x+=0.1;
    if (IsWindowResized()) {
      UnloadRenderTexture(ViewTexture);
      ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

    BeginTextureMode(ViewTexture);
    ClearBackground(LIGHTGRAY);
    BeginMode3D(Camera);

    DrawPlane(Vector3{ 0, 0, 0 }, Vector2{ 50, 50 }, BEIGE);
    DrawCube(Vector3{0, x, 0}, 5, 5, 5, Color{255, 255, 200});

    EndMode3D();

    EndTextureMode();
  };
  void Shutdown() override { UnloadRenderTexture(ViewTexture); }
  void Show() override {
    ImGui::SetNextWindowSizeConstraints(ImVec2(200,200),ImVec2(2000,2000));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("3D View", &Open, ImGuiWindowFlags_NoScrollbar)) {
      Focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
      // draw the view
      rlImGuiImageRenderTextureFit(&ViewTexture, true);
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }
};

void showDockSpace(){
    static bool dockingEnabled = true;
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("DockSpaceHost", nullptr, window_flags);

    ImGui::PopStyleVar(3);
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

    ImGui::End();

}
int main() {
  const int screenWidth = 1200;
  const int screenHeight = 700;
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Pixel Render");
  R3D_Init(screenWidth, screenHeight, 0);
  rlImGuiSetup(true);
  SetTargetFPS(60);

  Scene cena;
  bool open = true;

  cena.Setup();
  cena.Open = open;
  ImGui::LoadIniSettingsFromDisk("default.ini");
  while (!WindowShouldClose()) {
    cena.Update();

    BeginDrawing();
    ClearBackground(DARKGRAY);
    // start ImGui Conent
    rlImGuiBegin(); 
    // show ImGui Content
    showDockSpace();
    ImGui::ShowDemoWindow(&open);

    if (cena.Open) cena.Show();

    // salvador de padrão
    // if (ImGui::Button("save", ImVec2(4,4))) {
    //   ImGui::SaveIniSettingsToDisk("default.ini");
    // }

    // end ImGui Content
    rlImGuiEnd();
    EndDrawing();
  }
  cena.Shutdown();
  rlImGuiShutdown();
  R3D_Close();
  CloseWindow();
  return 0;
}