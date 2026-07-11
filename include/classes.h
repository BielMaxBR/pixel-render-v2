#pragma once
#include "raylib.h"
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
