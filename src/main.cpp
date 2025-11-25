#include "imgui.h"
#include "r3d.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"

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

  R3D_Mesh meshes[10];
  R3D_Material material;
  R3D_Light light;
  float x = 0;
  void Setup() override {
    ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    Camera = {.position = {-3, 3, 3},
              .target = {0, 0, 0},
              .up = {0, 1, 0},
              .fovy = 90.0f,
              .projection = CAMERA_PERSPECTIVE};
    // Create scene objects

    meshes[0] = R3D_GenMeshSphere(1.0f, 16, 32, true);
    meshes[1] = R3D_GenMeshPlane(10.0f,10.0f,10,10,true);
    material = R3D_GetDefaultMaterial();

    // Setup lighting
    light = R3D_CreateLight(R3D_LIGHT_DIR);
    R3D_SetLightDirection(light, (Vector3){-1, -1, -1});
    R3D_SetLightActive(light, true);
    R3D_EnableShadow(light, 20);
    R3D_SetBackgroundColor(BLANK);
    R3D_SetAmbientColor(BLANK);

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  };
  void Update() override {
    if (!Open)
      return;
    x += 0.1;
    if (IsWindowResized()) {
      UnloadRenderTexture(ViewTexture);
      ViewTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    }

    R3D_SetRenderTarget(&ViewTexture);
    R3D_Begin(Camera);
    for (int i = 0; i < 2; i++) {
    R3D_DrawMesh(&meshes[i], &material, MatrixIdentity());
    }
    R3D_End();
  };
  void Shutdown() override {
    for (int i = 0; i < 2; i++) {
    R3D_UnloadMesh(&meshes[i]);
    }
    UnloadRenderTexture(ViewTexture);
  }
  void Show() override {
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(2000, 2000));
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

void showDockSpace() {
  static bool dockingEnabled = true;
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
      ImGuiWindowFlags_NoBackground;

  ImGuiViewport *viewport = ImGui::GetMainViewport();

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

    if (cena.Open)
      cena.Show();

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