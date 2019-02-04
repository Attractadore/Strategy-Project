#pragma once

#include "SPlayer.hpp"
#include "SRenderer.hpp"
#include "SUnit.hpp"

#include <glm/vec2.hpp>

#include <memory>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class SNodeGraph;
struct SCamera;
class SNode;
class SBuilding;

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
  void handleLogic();
  void handleRendering();
  bool m_bQuit;
  std::shared_ptr<SNode> getClickedTile(int x, int y);
  void updateCamera();

  std::shared_ptr<SNodeGraph> m_tiles;
  int m_worldWidth;
  int m_worldHeight;
  int m_tileSize;
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

  std::unordered_set<std::shared_ptr<SUnit>> m_movingUnits;
  void performeUnitMovement(std::unordered_set<std::shared_ptr<SUnit>> units);

  void endTurn();

  std::unordered_map<std::string, SUnit> m_unitLookUpTable;

  std::unordered_map<std::shared_ptr<SNode>,
                     std::unordered_set<std::shared_ptr<SUnit>>>
      m_units;
  std::unordered_map<std::shared_ptr<SNode>, std::shared_ptr<SBuilding>>
      m_buildings;

  std::mt19937 m_gen;

  std::shared_ptr<SSprite> m_selectionSprite;
  std::shared_ptr<SSprite> m_endTurnButtonSprite;

  SPlayer defaultPlayer;
  std::vector<SPlayer *> m_players;
};
