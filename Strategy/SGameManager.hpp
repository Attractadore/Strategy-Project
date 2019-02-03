#pragma once

#include "SRenderer.hpp"

#include <glm/vec2.hpp>

#include <memory>
#include <random>
#include <unordered_map>
#include <vector>

class SNodeGraph;
struct SCamera;
class SNode;
class SUnit;

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
  void handleInput();
  bool m_bQuit;
  std::shared_ptr<SNode> getClickedTile(int x, int y);
  void updateCamera();

  std::shared_ptr<SNodeGraph> m_tiles;
  int m_worldWidth;
  int m_worldHeight;
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

  void endTurn();

  std::unordered_map<std::shared_ptr<SNode>,
                     std::vector<std::shared_ptr<SUnit>>>
      m_units;

  std::mt19937 m_gen;

  std::shared_ptr<SSprite> m_selectionSprite;
};
