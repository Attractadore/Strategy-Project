#pragma once

#include "SPlayer.hpp"
#include "SRenderer.hpp"
#include "SUnit.hpp"

#include <glm/vec2.hpp>

#include <list>
#include <memory>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class SNodeGraph;
struct SCamera;
class SNode;
class SBuilding;

enum class MouseButton { BUTTON_NONE, BUTTON_LEFT, BUTTON_RIGHT };

enum class KeyboardKey {
  KEY_ESCAPE,
  KEY_ENTER,
  KEY_RIGHT_SHIFT,
  KEY_LEFT_SHIFT
};

struct SGameInput {
  void reset();

  void activateKeyboardKeys(std::unordered_set<KeyboardKey> keys);
  void releaseKeyboardKeys(std::unordered_set<KeyboardKey> keys);

  bool bShiftDown();

  void pressKeyboardKey(KeyboardKey key);
  void releaseKeyboardKey(KeyboardKey key);

  void clickMouseButton(MouseButton button);

  std::unordered_set<MouseButton> mouseClickedButtons = {};
  std::unordered_set<KeyboardKey> keyboardPressedKeys = {};
  std::unordered_set<KeyboardKey> keyboardReleasedKeys = {};
  std::unordered_set<KeyboardKey> keyboardActiveKeys = {};

  bool bRightShiftDown = false;
  bool bLeftShiftDown = false;

  int mouseWheelScroll = 0;

  int mouseX = -1;
  int mouseY = -1;
};

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
  void handleRightClick();
  void handleLeftClick();
  void updateCamera();
  std::shared_ptr<SUnit> strongestUnit(std::shared_ptr<SNode> tile);

  std::shared_ptr<SNodeGraph> m_tiles;
  int m_worldWidth;
  int m_worldHeight;
  int m_tileSize;
  SRenderer m_renderer;
  std::shared_ptr<SCamera> m_camera;

  int m_maxFPS;
  float m_maxFrameTime;
  float m_deltaTime;
  float m_aspectRatio;
  float m_realVirtualRatio;
  int m_virtualWidth;
  int m_virtualHeight;

  std::shared_ptr<SNode> m_selectedTile;
  std::unordered_set<std::shared_ptr<SUnit>> m_selectedUnits;
  std::shared_ptr<SNode> m_targetTile;

  std::unordered_set<std::shared_ptr<SUnit>> m_movingUnits;
  void performUnitMovement(std::unordered_set<std::shared_ptr<SUnit>> units);

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
  std::shared_ptr<SSprite> m_buttonSelectionSprite;

  SPlayer defaultPlayer;
  std::vector<SPlayer *> m_players;

  SGameInput m_inputStruct;
};
