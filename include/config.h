#pragma once
#include "classes.h"

class SceneConfig : public DefaultWindow {
public:
  Vector3 cameraPosition = {0, 0, 0};
  Vector2 cameraRotation = {-45, 45};
  float cameraZoom = 5;
  int cameraProjection = CAMERA_PERSPECTIVE;
  float cameraFOV = 75;
  bool openFileDialogOpen = false;
  void Setup() override;
  void Show() override;
  void Vec3Menu(const char *label, Vector3 *vec, float speed = 0.1,
                float min = 0, float max = 1000, bool help = false);
  void Vec2Menu(const char *label, Vector2 *vec, float speed = 0.1,
                float min = 0, float max = 1000, bool help = false);
};