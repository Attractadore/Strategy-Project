#pragma once

#include "SRenderer.hpp"

#include <glm/vec2.hpp>

#include <memory>

class SNodeGraph;
class SCamera;
class SNode;

class SGameManager {
public:
  SGameManager();
  SGameManager(const SGameManager &other) = delete;
  SGameManager(SGameManager &&other) = delete;
  ~SGameManager();

  SGameManager &operator=(const SGameManager &other) = delete;
  SGameManager &operator=(SGameManager &&other) = delete;

  void run();

private:
  std::shared_ptr<SNodeGraph> m_tiles;
  SRenderer m_renderer;
  std::shared_ptr<SCamera> m_camera;

  glm::vec2 m_cameraMovementInput;
  int m_cameraZoomInput;

  int m_maxFPS;
  float m_maxFrameTime;
  float m_deltaTime;
  float m_aspectRatio;
  float m_realVirtualRatio;

  std::shared_ptr<SNode> m_selectedTile;
  std::shared_ptr<SNode> m_targetTile;

  void updateCamera();
  void endTurn();

  std::shared_ptr<SNode> getClickedTile(int x, int y);
};
