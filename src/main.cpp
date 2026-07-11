#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "raylib.h"
#include "rcamera.h"
#include "rlImGui.h"
#include <string>

#include "config.h"
#include "viewportScene.h"


void MenuBar(SceneConfig *config) {
  if (ImGui::BeginMenuBar()) {

    if (ImGui::BeginMenu("File")) {
      config->openFileDialogOpen = ImGui::MenuItem("Open");
      ImGui::Separator();
      if (ImGui::MenuItem("Exit"))
        CloseWindow();
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void updateDialogs(SceneConfig *config, ViewportScene *scene) {
  if (config->openFileDialogOpen) {
    IGFD::FileDialogConfig config;
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFile", "Choose a File",
                                            ".glb,.gltf", config);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".glb",
                                              ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".gltf",
                                              ImVec4(0.8f, 1.0f, 0.0f, 0.9f));
  }

  // display
  if (ImGuiFileDialog::Instance()->Display(
          "ChooseFile", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400),
          ImVec2(2200, 1500))) {
    if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
      std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
      // action
      UnloadModel(scene->model);
      
      scene->model = LoadModel(filePathName.c_str());
      scene->modelAnimations = LoadModelAnimations(filePathName.c_str(), &scene->animCount);
      // printf("%s\n", filePathName.c_str());
      config->openFileDialogOpen = false;
    } else {
      config->openFileDialogOpen = false;
    }
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

  ViewportScene cena;
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
    updateDialogs(&config, &cena);
    if (cena.Open)
      cena.Show();
    if (config.Open)
      config.Show();

    // salvador de padrão
    if (ImGui::Button("save", ImVec2(10, 10))) {
      // ImGui::SaveIniSettingsToDisk("default.ini");
      cena.ExportAnim();
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