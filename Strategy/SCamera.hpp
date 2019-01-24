#pragma once

#include <glm/vec2.hpp>

struct SCamera {
  glm::vec2 pos{0.0f, 0.0f};

  float defaultZoom = 2.0f;
  float zoomRate = 0.05f;
  float currentZoom = defaultZoom;

  float cameraSpeed = 500.0f;

  int viewportWidth = 1920;
  int viewportHeight = 1080;
};
