#include "viewportScene.h"
#include "imgui.h"
#include "raymath.h"
#include "raylib.h"
#include "rcamera.h"
#include "rlImGui.h"
#include "utils.h"
#include <cstdio>

void ViewportScene::Setup() {
  ViewTexture = LoadRenderTexture(sprite_width, sprite_height);
  lastCameraZoom = config->cameraZoom;
  Camera = {.position = {lastCameraZoom, 0, 0},
            .target = {0, 0, 0},
            .up = {0, 1, 0},
            .fovy = 75.0f,
            .projection = CAMERA_PERSPECTIVE};

  model = LoadModel("pistola.gltf");
};

void ViewportScene::UpdateMyCamera() {
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
void ViewportScene::Update() {
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
  if (withGrid) {
    DrawGrid(10, 1.0f);
  }
  EndMode3D();
  EndTextureMode();
};
void ViewportScene::Shutdown() {
  UnloadModel(model);
  UnloadRenderTexture(ViewTexture);
}
void ViewportScene::Show() {
  ImGui::SetNextWindowSizeConstraints(ImVec2(sprite_width, sprite_height),
                                      ImVec2(2000, 2000));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  if (ImGui::Begin("3D View", &Open, ImGuiWindowFlags_NoScrollbar)) {
    Focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
    // draw the view
    rlImGuiImageRenderTextureFit(&ViewTexture, true);

    if (ImGui::Button("save", ImVec2(4, 4))) {
      ExportAnim();
    }
  }
  ImGui::End();
  ImGui::PopStyleVar();
}

void ViewportScene::ExportAnim() {
  int sides = 1;
  float actual_rotation = config->cameraRotation.x;
  withGrid = false;
  int animIndex = 0;
  int frameCount = 1;
  int frameRate = 12;
  int animCurrentFrame = 0;
  printf("%d\n",animCount);
  
  ModelAnimation anim;
  if (animCount > 0) {
    anim = modelAnimations[animIndex];
    frameCount = anim.frameCount;
  }
  // animCurrentFrame = (animCurrentFrame + 1)%anim.frameCount;
  
  // UpdateModelAnimation(model, anim, animCurrentFrame);
  float totalFrames = (float)frameCount/(60.0/(float)frameRate);
  Image sheet = GenImageColor(sprite_width * (int)ceil(totalFrames), sprite_height * sides, BLANK);

  for (int i = 0; i < sides; i++) {
    config->cameraRotation.x = (360.0/sides)*i + actual_rotation;
    for (int frame = 0; frame < totalFrames; frame++) {
      if (animCount > 0) {
        UpdateModelAnimation(model, anim, frameCutter(frame,frameRate));
      }
      Update();
      Image img = LoadImageFromTexture(ViewTexture.texture);
      ImageFlipVertical(&img);


      Rectangle srcRec = {0, 0, (float)sprite_width, (float)sprite_height};
      // Rectangle destPos = {0, 0, (float)sprite_width, (float)sprite_height};
      Rectangle destPos = {(float)(frame * sprite_width), (float)(i * sprite_height), (float)sprite_width, (float)sprite_height};
      ImageDraw(&sheet, img, srcRec, destPos, WHITE);

      UnloadImage(img); // libera o frame old
      // std::string text = "final";
      // text.insert(text.size(), std::to_string(i));
      // text.insert(text.size(), ".png");
      // ExportImage(img, text.c_str());
    }
  }
  ExportImage(sheet, "final.png");
  // UnloadImage(sheet);
  withGrid = true;
  config->cameraRotation.x = actual_rotation;
}
