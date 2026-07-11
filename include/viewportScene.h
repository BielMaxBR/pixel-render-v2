#include "classes.h"
#include "config.h"

class ViewportScene : public DocumentWindow {
public:
  Camera3D Camera = {0};
  Model model;
  Material material;
  SceneConfig *config;

  int sprite_width = 64*2;
  int sprite_height = 64*2;

  Vector2 lastCameraRotation = {0, 0};
  Vector3 lastCameraPosition = {0, 0, 0};

  float lastCameraZoom = 0;

  bool withGrid = true;

  ModelAnimation *modelAnimations;
  int animCount = 0;

  void Setup() override;
  void UpdateMyCamera();
  void Update() override;
  void Shutdown() override;
  void Show() override;
  void ExportAnim();
};