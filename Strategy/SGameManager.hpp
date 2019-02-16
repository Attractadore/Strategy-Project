#pragma once

#include "SBuilding.hpp"
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

using UNIT_ID = std::string;
using BUILDING_ID = std::string;

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
  std::shared_ptr<SBuilding> m_selectedBuilding;
  //  std::shared_ptr<SNode> m_targetTile;

  void performUnitMovement();
  void performUnitMovement(std::unordered_set<std::shared_ptr<SUnit>> units);
  void generateMana();

  void endTurn();

  std::unordered_map<std::string, SUnit> m_unitLookUpTable;
  std::unordered_map<std::string, SBuilding> m_buildingLookUpTable;

  std::unordered_map<std::shared_ptr<SNode>,
                     std::unordered_set<std::shared_ptr<SUnit>>>
      m_units;
  std::unordered_map<std::shared_ptr<SUnit>, std::shared_ptr<SNode>>
      m_unitTargetTiles;
  std::unordered_map<std::shared_ptr<SNode>, std::shared_ptr<SBuilding>>
      m_buildings;

  std::mt19937 m_gen;

  std::shared_ptr<SSprite> m_selectionSprite;
  std::shared_ptr<SSprite> m_endTurnButtonSprite;
  std::shared_ptr<SSprite> m_buttonSelectionSprite;
  std::shared_ptr<SSprite> m_buttonSelectionLargeSprite;
  std::shared_ptr<SSprite> m_manaIconSprite;
  std::shared_ptr<SSprite> m_manaGeyserSprite;
  std::shared_ptr<SSprite> m_manaBallSprite;
  std::shared_ptr<SSprite> m_foundationSprite;
  std::shared_ptr<SSprite> m_uiIconBackgroundSprite;
  std::shared_ptr<SSprite> m_uiIconBackgroundBuildingSprite;
  std::shared_ptr<SSprite> m_buildingConstructionSprite;
  //  std::shared_ptr<SSprite> m_buildingBarracksIconSprite;
  //  std::shared_ptr<SSprite> m_buildingShrineIconSprite;

  SPlayer defaultPlayer;
  std::vector<SPlayer *> m_players;

  SGameInput m_inputStruct;

  std::uniform_int_distribution<> m_manaBallValue{150, 250};
  std::uniform_int_distribution<> m_manaGeyserValue{50, 75};

  std::uniform_real_distribution<float> m_coinTossDist{0, 1};

  float m_geyserChance = 0.005f;
  float m_manaBallChance = 0.01f;
  float m_foundationChance = 0.04f;

  int m_numManaBallRemaining;
  float m_wsmbRatio = 0.03f;

  bool bCoinToss(float chance);

  void tryAddManaBall(std::shared_ptr<SNode> tile);

  bool bCanConstructBuilding(std::shared_ptr<SNode> tile,
                             std::string buildingId, SPlayer &player);

  std::unordered_set<std::string>
  getConstructableBuidlings(std::shared_ptr<SNode> tile, SPlayer &player);
  std::shared_ptr<SBuilding>
  constructBuidlingForPlayer(std::shared_ptr<SNode> tile, BUILDING_ID building,
                             SPlayer &player);
  std::shared_ptr<SBuilding> spawnBuidlingForPlayer(std::shared_ptr<SNode> tile,
                                                    BUILDING_ID building,
                                                    SPlayer &player);

  std::unordered_set<std::shared_ptr<SUnit>>
  spawnUnitsForPlayer(std::shared_ptr<SNode> tile, UNIT_ID unit, int playerId,
                      int num);

  bool bCanTrainUnit(std::shared_ptr<SBuilding> building, std::string unit);
  std::unordered_set<std::string>
  getBuildableUnits(std::shared_ptr<SBuilding> building);

  std::shared_ptr<SNode> getUnitTile(std::shared_ptr<SUnit> unit);
  std::shared_ptr<SNode> getUnitTargetTile(std::shared_ptr<SUnit> unit);
  void setUnitTargetTile(std::shared_ptr<SUnit> unit,
                         std::shared_ptr<SNode> tile);
  void resetUnitTargetTile(std::shared_ptr<SUnit> unit);
  bool bUnitFinishedRoute(std::shared_ptr<SUnit> unit);
  void moveUnitToTile(std::shared_ptr<SUnit> unit, std::shared_ptr<SNode> tile);
  void startUnitMovement(std::shared_ptr<SUnit> unit,
                         std::shared_ptr<SNode> tile);
  void stopUnitMovement(std::shared_ptr<SUnit> unit);

  void addResourcesForPlayer(int playerId, int amount);
};
