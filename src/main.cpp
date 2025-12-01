#include "ImGuiFileDialog.h"
#include "cstdio"
#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "rlImGui.h"

static void HelpMarker(const char *desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip()) {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

class DefaultWindow {
public:
  bool Open = false;
  bool Focused = false;

  virtual void Setup() = 0;
  virtual void Show() = 0;
};

class DocumentWindow : public DefaultWindow {
public:
  RenderTexture ViewTexture;

  virtual void Shutdown() = 0;
  virtual void Update() = 0;
};

class SceneConfig : public DefaultWindow {
public:
  Vector3 cameraPosition = {0, 0, 0};
  Vector2 cameraRotation = {-45, 45};
  float cameraZoom = 5;
  int cameraProjection = CAMERA_PERSPECTIVE;
  float cameraFOV = 75;
  bool openFileDialogOpen = false;
  void Setup() override { printf("fodase"); }

  void Show() override {
    ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(20000, 20000));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("Scene Config", &Open, 0)) {
      ImGui::SeparatorText("Camera");
      Vec3Menu("Position", &cameraPosition, 0.1, -10000, 10000, true);
      Vec2Menu("Rotation", &cameraRotation, 0.1, -180, 180);

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
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }

  void Vec3Menu(const char *label, Vector3 *vec, float speed = 0.1,
                float min = 0, float max = 1000, bool help = false) {
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

  void Vec2Menu(const char *label, Vector2 *vec, float speed = 0.1,
                float min = 0, float max = 1000, bool help = false) {
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
};

class Scene : public DocumentWindow {
public:
  Camera3D Camera = {0};
  Model model;
  Material material;
  SceneConfig *config;

  int sprite_width = 128;
  int sprite_height = 128;

  Vector2 lastCameraRotation = {0, 0};
  Vector3 lastCameraPosition = {0, 0, 0};

  float lastCameraZoom = 0;

  void Setup() override {
    ViewTexture = LoadRenderTexture(sprite_width, sprite_height);
    lastCameraZoom = config->cameraZoom;
    Camera = {.position = {lastCameraZoom, 0, 0},
              .target = {0, 0, 0},
              .up = {0, 1, 0},
              .fovy = 75.0f,
              .projection = CAMERA_PERSPECTIVE};

    model = LoadModel("neco.glb");
  };
  void UpdateMyCamera() {
    bool lockView = false;
    bool rotateAroundTarget = true;
    bool rotateUp = false;
    bool moveInWorldPlane = true;
    // rotação
    Camera.fovy = config->cameraFOV;
    if (Camera.projection == CAMERA_ORTHOGRAPHIC) {
      Camera.fovy = config->cameraZoom;
    }
    Camera.projection = config->cameraProjection;

    if (!Vector2Equals(config->cameraRotation, lastCameraRotation)) {
      Vector2 rotationResult = config->cameraRotation - lastCameraRotation;

      CameraYaw(&Camera, -rotationResult.x * DEG2RAD, rotateAroundTarget);
      CameraPitch(&Camera, -rotationResult.y * DEG2RAD, lockView,
                  rotateAroundTarget, rotateUp);

      lastCameraRotation = config->cameraRotation;
    }

    // posição
    if (!Vector3Equals(config->cameraPosition, lastCameraPosition)) {
      Vector3 positionResult = config->cameraPosition - lastCameraPosition;

      CameraMoveForward(&Camera, positionResult.z, moveInWorldPlane);
      CameraMoveUp(&Camera, positionResult.y);
      CameraMoveRight(&Camera, positionResult.x, moveInWorldPlane);

      lastCameraPosition = config->cameraPosition;
    }

    // zoom
    if (!FloatEquals(config->cameraZoom, lastCameraZoom)) {
      float zoomResult = config->cameraZoom - lastCameraZoom;
      CameraMoveToTarget(&Camera, zoomResult);
      lastCameraZoom = config->cameraZoom;
    }
  }
  void Update() override {
    if (!Open)
      return;
    if (IsWindowResized()) {
      UnloadRenderTexture(ViewTexture);
      ViewTexture = LoadRenderTexture(sprite_width, sprite_height);
    }

    UpdateMyCamera();

    BeginTextureMode(ViewTexture);
    ClearBackground(BLANK);
    BeginMode3D(Camera);

    DrawModel(model, Vector3{0, 0, 0}, 1, WHITE);
    DrawGrid(10, 1.0f);
    EndMode3D();
    EndTextureMode();
  };
  void Shutdown() override {
    UnloadModel(model);
    UnloadRenderTexture(ViewTexture);
  }
  void Show() override {
    ImGui::SetNextWindowSizeConstraints(ImVec2(sprite_width, sprite_height),
                                        ImVec2(2000, 2000));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("3D View", &Open, ImGuiWindowFlags_NoScrollbar)) {
      Focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
      // draw the view
      rlImGuiImageRenderTextureFit(&ViewTexture, true);

      if (ImGui::Button("save", ImVec2(4, 4))) {
        Image img = LoadImageFromTexture(ViewTexture.texture);
        ImageFlipVertical(&img);
        ExportImage(img, "final.png");
      }
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }
};

void MenuBar(SceneConfig *config) {
  if (ImGui::BeginMenuBar()) {

    if (ImGui::BeginMenu("File")) {
      config->openFileDialogOpen = ImGui::MenuItem("Open");

      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void updateDialogs(SceneConfig *config) {
  if (config->openFileDialogOpen) {
    IGFD::FileDialogConfig config;
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFile", "Choose a File",
                                            ".glb,.gltf", config);
  }

  // display
  if (ImGuiFileDialog::Instance()->Display("ChooseFile")) {
    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
      std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
      // action
      printf("%s\n", filePathName.c_str());
      config->openFileDialogOpen = false;
    }
    if (ImGuiFileDialog::Instance()->)
    // close
    ImGuiFileDialog::Instance()->Close();
  }
}

void showDockSpace(SceneConfig *config) {
  static bool dockingEnabled = true;

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
      ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

  ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

  ImGui::Begin("DockSpaceHost", nullptr, window_flags);
  MenuBar(config);

  ImGui::PopStyleVar(3);
  ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

  ImGui::End();
}
int main() {
  const int screenWidth = 1200;
  const int screenHeight = 700;
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "Pixel Render");
  rlImGuiSetup(true);
  SetTargetFPS(60);

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  bool open = true;

  Scene cena;
  SceneConfig config;
  config.Setup();
  cena.config = &config;
  cena.Setup();
  cena.Open = open;
  config.Open = open;
  ImGui::LoadIniSettingsFromDisk("default.ini");
  while (!WindowShouldClose()) {
    cena.Update();

    BeginDrawing();
    ClearBackground(DARKGRAY);
    
    // start ImGui Conent
    rlImGuiBegin();
    // show ImGui Content

    showDockSpace(&config);
    // ImGui::ShowDemoWindow(&open);
    updateDialogs(&config);
    if (cena.Open)
      cena.Show();
    if (config.Open)
      config.Show();

    // salvador de padrão
    if (ImGui::Button("save", ImVec2(10, 10))) {
      ImGui::SaveIniSettingsToDisk("default.ini");
    }

    // end ImGui Content

    rlImGuiEnd();
    EndDrawing();
  }
  cena.Shutdown();
  rlImGuiShutdown();
  CloseWindow();
  return 0;
}