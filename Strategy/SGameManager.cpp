#include "SGameManager.hpp"

#include "SBuilding.hpp"
#include "SCamera.hpp"
#include "SNode.hpp"
#include "SNodeGraph.hpp"
#include "SSprite.hpp"
#include "SUnit.hpp"

#include <glm/glm.hpp>

#include <SDL2/SDL.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <set>
#include <thread>

#include <cmath>

bool compareUnits(const std::shared_ptr<SUnit>& lhs, const std::shared_ptr<SUnit>& rhs)
{
  return lhs->getHealth() * lhs->m_damage * lhs->m_accuracy + lhs->getMoves() < rhs->getHealth() * rhs->m_damage * rhs->m_accuracy + rhs->getMoves();
}

bool compareUnitsUI(const std::shared_ptr<SUnit>& lhs, const std::shared_ptr<SUnit>& rhs)
{
  return lhs->getHealth() * lhs->m_damage * lhs->m_accuracy * lhs->getMoves() < rhs->getHealth() * rhs->m_damage * rhs->m_accuracy * rhs->getMoves();
}

SGameManager::SGameManager()
{
  //  m_worldWidth = 32;
  //  m_worldHeight = 16;
  m_worldWidth = 32;
  m_worldHeight = 32;
  m_numManaBallRemaining = int(m_worldWidth * m_worldHeight * m_wsmbRatio);

  m_tiles = std::make_shared<SNodeGraph>(m_worldWidth, m_worldHeight);
  m_camera = std::make_shared<SCamera>();

  m_camera->pos = { 0, 0 };
  m_camera->zoomRate = 0.9f;
  m_camera->cameraSpeed = 800.0f;
  m_camera->defaultZoom = 1.0f;
  m_camera->currentZoom = m_camera->defaultZoom;
  m_camera->viewportWidth = 1280;
  m_camera->viewportHeight = 720;

  m_renderer.setRenderCamera(m_camera);

  m_aspectRatio = float(m_camera->viewportWidth) / m_camera->viewportHeight;
  m_virtualHeight = 1000;
  m_virtualWidth = int(m_virtualHeight * m_aspectRatio);
  m_realVirtualRatio = float(m_camera->viewportHeight) / m_virtualHeight;
  m_maxFPS = 120;
  m_maxFrameTime = 1.0f / m_maxFPS;

  auto spearmanSprite = std::make_shared<SSprite>("./assets/art/spearman.png", 1, 20, 3);
  auto spearmanTCSprite = std::make_shared<SSprite>("./assets/art/spearmanTC.png", 1, 20, 3.1);
  auto archerSprite = std::make_shared<SSprite>("./assets/art/archer.png", 1, 20, 3);
  auto archerTCSprite = std::make_shared<SSprite>("./assets/art/archerTC.png", 1, 20, 3.1);
  auto horsemanSprite = std::make_shared<SSprite>("./assets/art/horseman.png", 1, 20, 3);
  auto horsemanTCSprite = std::make_shared<SSprite>("./assets/art/horsemanTC.png", 1, 20, 3.1);
  auto catapultSprite = std::make_shared<SSprite>("./assets/art/catapult.png", 1, 20, 3);
  auto catapultTCSprite = std::make_shared<SSprite>("./assets/art/catapultTC.png", 1, 20, 3.1);

  auto swordsmanSprite = std::make_shared<SSprite>("./assets/art/swordsman.png", 1, 20, 3);
  auto swordsmanTCSprite = std::make_shared<SSprite>("./assets/art/swordsmanTC.png", 1, 20, 3.1);
  auto crossbowmanSprite = std::make_shared<SSprite>("./assets/art/crossbowman.png", 1, 20, 3);
  auto crossbowmanTCSprite = std::make_shared<SSprite>("./assets/art/crossbowmanTC.png", 1, 20, 3.1);
  auto knightSprite = std::make_shared<SSprite>("./assets/art/knight.png", 1, 20, 3);
  auto knightTCSprite = std::make_shared<SSprite>("./assets/art/knightTC.png", 1, 20, 3.1);

  m_selectionSprite = std::make_shared<SSprite>("./assets/art/selection.png", 1, 60, 4);
  m_endTurnButtonSprite = std::make_shared<SSprite>("./assets/art/ui/endTurn.png", 1, 150, 5);
  m_endTurnButtonTCSprite = std::make_shared<SSprite>("./assets/art/ui/endTurnTC.png", 1, 150, 4.9);
  m_buttonSelectionSprite = std::make_shared<SSprite>("./assets/art/ui/buttonSelection.png", 1, 20, 5);
  m_buttonSelectionHasNotMovedSprite = std::make_shared<SSprite>("./assets/art/ui/buttonSelectionHasNotMoved.png", 1, 20, 5);
  m_buttonSelectionNoMovesSprite = std::make_shared<SSprite>("./assets/art/ui/buttonSelectionNoMoves.png", 1, 20, 5);
  m_buttonSelectionLargeSprite = std::make_shared<SSprite>("./assets/art/ui/buttonSelection.png", 1, 30, 5);
  m_manaIconSprite = std::make_shared<SSprite>("./assets/art/ui/manaIcon.png", 1, 15, 5);
  m_manaGeyserSprite = std::make_shared<SSprite>("./assets/art/manaGeyser.png", 1, 50, 2);
  m_manaBallSprite = std::make_shared<SSprite>("./assets/art/manaBall.png", 1, 30, 2.5);
  m_foundationSprite = std::make_shared<SSprite>("./assets/art/ancientFoundation.png", 1, 60, 1.5);
  m_uiIconBackgroundSprite = std::make_shared<SSprite>("./assets/art/ui/uiBackground.png", 1, 20, 2.9);
  m_uiIconBackgroundBuildingSprite = std::make_shared<SSprite>("./assets/art/ui/uiBackground.png", 1, 30, 4.9);
  m_buildingConstructionSprite = std::make_shared<SSprite>("./assets/art/buildingConstruction.png", 1, 60, 2);
  m_buildingConstructionTCSprite = std::make_shared<SSprite>("./assets/art/buildingConstructionTC.png", 1, 60, 2.1);
  auto buildingBarracksIconSprite = std::make_shared<SSprite>("./assets/art/building.png", 1, 30, 5);
  auto buildingShrineIconSprite = std::make_shared<SSprite>("./assets/art/shrine.png", 1, 30, 5);
  auto productionBuildingSprite = std::make_shared<SSprite>("./assets/art/building.png", 1, 60, 2);
  auto shrineSprite = std::make_shared<SSprite>("./assets/art/shrine.png", 1, 60, 2);
  auto productionBuildingTCSprite = std::make_shared<SSprite>("./assets/art/buildingTC.png", 1, 60, 2.1);
  auto shrineTCSprite = std::make_shared<SSprite>("./assets/art/shrineTC.png", 1, 60, 2.1);

  SUnit spearman{ "UNIT_SPEARMAN" };
  spearman.setSprite(spearmanSprite);
  spearman.setTCSprite(spearmanTCSprite);
  spearman.m_health = 75;
  spearman.m_armor = 1;
  spearman.m_damage = 10;
  spearman.m_accuracy = 1;
  spearman.m_attackRange = 0;
  spearman.m_moves = 2;
  spearman.m_buildTime = 4;
  spearman.m_resourceCost = 300;

  SUnit archer{ "UNIT_ARCHER" };
  archer.setSprite(archerSprite);
  archer.setTCSprite(archerTCSprite);
  archer.m_health = 50;
  archer.m_armor = 0;
  archer.m_damage = 13;
  archer.m_accuracy = 0.7f;
  archer.m_attackRange = 2;
  archer.m_moves = 2;
  archer.m_buildTime = 5;
  archer.m_resourceCost = 400;

  SUnit horseman{ "UNIT_HORSEMAN" };
  horseman.setSprite(horsemanSprite);
  horseman.setTCSprite(horsemanTCSprite);
  horseman.m_health = 65;
  horseman.m_armor = 0;
  horseman.m_damage = 12;
  horseman.m_accuracy = 1;
  horseman.m_attackRange = 0;
  horseman.m_moves = 4;
  horseman.m_buildTime = 5;
  horseman.m_resourceCost = 500;

  SUnit catapult{ "UNIT_CATAPULT" };
  catapult.setSprite(catapultSprite);
  catapult.setTCSprite(catapultTCSprite);
  catapult.m_health = 60;
  catapult.m_armor = 2;
  catapult.m_damage = 20;
  catapult.m_accuracy = 0.6f;
  catapult.m_attackRange = 3;
  catapult.m_moves = 1;
  catapult.m_buildTime = 7;
  catapult.m_resourceCost = 800;

  SUnit swordsman{ "UNIT_SWORDSMAN" };
  swordsman.setSprite(swordsmanSprite);
  swordsman.setTCSprite(swordsmanTCSprite);
  swordsman.m_health = 90;
  swordsman.m_armor = 2;
  swordsman.m_damage = 12;
  swordsman.m_accuracy = 1;
  swordsman.m_attackRange = 0;
  swordsman.m_moves = 2;

  SUnit crossbowman{ "UNIT_CROSSBOWMAN" };
  crossbowman.setSprite(archerSprite);
  crossbowman.setTCSprite(archerTCSprite);
  crossbowman.m_health = 60;
  crossbowman.m_armor = 0;
  crossbowman.m_damage = 16;
  crossbowman.m_accuracy = 0.8f;
  crossbowman.m_attackRange = 2;
  crossbowman.m_moves = 2;

  SUnit knight{ "UNIT_KNIGHT" };
  knight.setSprite(knightSprite);
  knight.setTCSprite(knightTCSprite);
  knight.m_health = 75;
  knight.m_armor = 1;
  knight.m_damage = 14;
  knight.m_accuracy = 1;
  knight.m_attackRange = 0;
  knight.m_moves = 5;

  m_unitLookUpTable[swordsman.m_ID] = swordsman;
  m_unitLookUpTable[crossbowman.m_ID] = crossbowman;
  m_unitLookUpTable[knight.m_ID] = knight;

  spearman.setPromotionUnit(&m_unitLookUpTable[swordsman.m_ID]);
  archer.setPromotionUnit(&m_unitLookUpTable[crossbowman.m_ID]);
  horseman.setPromotionUnit(&m_unitLookUpTable[knight.m_ID]);

  m_unitLookUpTable[spearman.m_ID] = spearman;
  m_unitLookUpTable[archer.m_ID] = archer;
  m_unitLookUpTable[horseman.m_ID] = horseman;
  m_unitLookUpTable[catapult.m_ID] = catapult;


  SBuilding productionBuilding("BUILDING_BARRACKS");
  productionBuilding.setUnitLookUpTable(&m_unitLookUpTable);
  productionBuilding.setSprite(productionBuildingSprite);
  productionBuilding.setUISprite(buildingBarracksIconSprite);
  productionBuilding.setTCSprite(productionBuildingTCSprite);
  productionBuilding.m_resourceGatherRate = 0;
  productionBuilding.m_buildTime = 10;
  productionBuilding.m_armor = 0;
  productionBuilding.m_health = 400;
  productionBuilding.m_resourceCost = 800;
  productionBuilding.m_buildableUnits = { spearman.m_ID, archer.m_ID, horseman.m_ID, catapult.m_ID };

  SBuilding resourceBuilding("BUILDING_SHRINE");
  resourceBuilding.setUnitLookUpTable(&m_unitLookUpTable);
  resourceBuilding.setSprite(shrineSprite);
  resourceBuilding.setUISprite(buildingShrineIconSprite);
  resourceBuilding.setTCSprite(shrineTCSprite);
  resourceBuilding.m_resourceGatherRate = 30;
  resourceBuilding.m_buildTime = 8;
  resourceBuilding.m_armor = 2;
  resourceBuilding.m_health = 200;
  resourceBuilding.m_resourceCost = 600;

  m_buildingLookUpTable[productionBuilding.m_ID] = productionBuilding;
  m_buildingLookUpTable[resourceBuilding.m_ID] = resourceBuilding;

  m_gen.seed(std::random_device()());

  std::unordered_set<std::shared_ptr<SNode>> foundationTiles;

  for (auto& t : m_tiles->getTiles())
  {
    if (!(t->bPassable))
    {
      continue;
    }
    if (bCoinToss(m_geyserChance) and m_buildings.count(t) == 0)
    {
      t->addGeyser();
      t->addMana(m_manaGeyserValue(m_gen));
    }
    tryAddManaBall(t);
    if (!t->bHasGeyser() and bCoinToss(m_foundationChance) and t->m_movementCost != 3)
    {
      t->addFoundation();
      foundationTiles.insert(t);
    }
  }

  for (int i = 0; i < m_numPlayers; i++)
  {
    m_players[i] = std::make_shared<SPlayer>(i);

    std::uniform_int_distribution<> dist(0, foundationTiles.size() - 1);
    auto spawnTile = *std::next(foundationTiles.begin(), dist(m_gen));
    spawnBuildingForPlayer(spawnTile, productionBuilding.m_ID, i);
    spawnUnitsForPlayer(spawnTile, spearman.m_ID, i, 1);
    if (spawnTile->getCurrentMana() > 0)
    {
      spawnTile->getAndRemoveMana();
      m_numManaBallRemaining++;
    }
    foundationTiles.erase(spawnTile);
  }

  m_currentPlayerId = 0;

  m_bQuit = false;
}

SGameManager::~SGameManager()
{
}

void SGameManager::run()
{
  auto time = std::chrono::steady_clock::now();

  while (!m_bQuit)
  {
    auto newTime = std::chrono::steady_clock::now();
    m_deltaTime = m_deltaTime =
    std::chrono::duration_cast<std::chrono::microseconds>(newTime - time).count() / 1000000.0f;
    time = newTime;

    if (m_maxFPS != 0)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(std::max(0, int(1000000 * (m_maxFrameTime - m_deltaTime)))));
    }

    handleInput();
    handleLogic();
    handleRendering();
  }
}

void SGameManager::handleInput()
{
  SDL_Event e;
  int x, y;
  SDL_GetMouseState(&x, &y);
  x /= m_realVirtualRatio;
  y /= m_realVirtualRatio;

  m_inputStruct.mouseX = x;
  m_inputStruct.mouseY = y;

  while (SDL_PollEvent(&e) != 0)
  {
    if (e.type == SDL_QUIT)
    {
      m_bQuit = true;
    }

    else if (e.type == SDL_KEYDOWN and !e.key.repeat)
    {
      auto ks = e.key.keysym.scancode;
      if (ks == SDL_SCANCODE_ESCAPE)
      {
        m_inputStruct.pressKeyboardKey(KeyboardKey::KEY_ESCAPE);
      }
      else if (ks == SDL_SCANCODE_RETURN)
      {
        m_inputStruct.pressKeyboardKey(KeyboardKey::KEY_ENTER);
      }
      else if (ks == SDL_SCANCODE_RSHIFT)
      {
        m_inputStruct.pressKeyboardKey(KeyboardKey::KEY_RIGHT_SHIFT);
        m_inputStruct.bRightShiftDown = true;
      }
      else if (ks == SDL_SCANCODE_LSHIFT)
      {
        m_inputStruct.pressKeyboardKey(KeyboardKey::KEY_LEFT_SHIFT);
        m_inputStruct.bLeftShiftDown = true;
      }
    }

    else if (e.type == SDL_KEYUP and !e.key.repeat)
    {
      auto ks = e.key.keysym.scancode;
      if (ks == SDL_SCANCODE_ESCAPE)
      {
        m_inputStruct.releaseKeyboardKey(KeyboardKey::KEY_ESCAPE);
      }
      else if (ks == SDL_SCANCODE_RETURN)
      {
        m_inputStruct.releaseKeyboardKey(KeyboardKey::KEY_ENTER);
      }
      else if (ks == SDL_SCANCODE_RSHIFT)
      {
        m_inputStruct.releaseKeyboardKey(KeyboardKey::KEY_RIGHT_SHIFT);
        m_inputStruct.bRightShiftDown = false;
      }
      else if (ks == SDL_SCANCODE_LSHIFT)
      {
        m_inputStruct.releaseKeyboardKey(KeyboardKey::KEY_LEFT_SHIFT);
        m_inputStruct.bLeftShiftDown = false;
      }
    }

    else if (e.type == SDL_MOUSEWHEEL)
    {
      m_inputStruct.mouseWheelScroll += e.wheel.y;
    }

    else if (e.type == SDL_MOUSEBUTTONDOWN)
    {

      if (e.button.button == SDL_BUTTON_LEFT)
      {
        m_inputStruct.clickMouseButton(MouseButton::BUTTON_LEFT);
      }

      else if (e.button.button == SDL_BUTTON_RIGHT)
      {
        m_inputStruct.clickMouseButton(MouseButton::BUTTON_RIGHT);
      }
    }
  }
}

void SGameManager::handleLogic()
{
  for (auto& k : m_inputStruct.keyboardPressedKeys)
  {
    if (k == KeyboardKey::KEY_ESCAPE)
    {
      m_bQuit = true;
    }
    else if (k == KeyboardKey::KEY_ENTER)
    {
      endTurn();
      //      checkAndRemovePlayers();
    }
  }
  for (auto& m : m_inputStruct.mouseClickedButtons)
  {
    if (m == MouseButton::BUTTON_LEFT)
    {
      handleLeftClick();
      //      checkAndRemovePlayers();
    }
    else if (m == MouseButton::BUTTON_RIGHT)
    {
      handleRightClick();
      //      checkAndRemovePlayers();
    }
  }

  updateCamera();

  if (m_selectedUnits.size() == 0 and m_selectedBuilding == nullptr)
  {
    m_selectedTile = nullptr;
  }

  m_inputStruct.reset();
}

void SGameManager::handleRendering()
{

  for (auto& tile : m_tiles->getTiles())
  {
    auto sprite = tile->getSprite();
    float x, y;
    std::tie(x, y) = tile->getPos();
    x *= sprite->m_size;
    y *= sprite->m_size;
    m_renderer.submitRenderRequest(tile->getSprite(), x, y, 0, RenderLocation::RENDER_CENTER);
    if (m_buildings.count(tile) > 0)
    {
      auto tileBuilding = m_buildings[tile];
      if (tileBuilding->bUnderConstruction())
      {
        m_renderer.submitRenderRequest(m_buildingConstructionSprite, x, y, 0, RenderLocation::RENDER_CENTER);
        m_renderer.submitRenderRequest(m_buildingConstructionTCSprite, x, y, 0, RenderLocation::RENDER_CENTER, true, m_playerColors[tileBuilding->getOwner()]);
      }
      else
      {
        m_renderer.submitRenderRequest(tileBuilding->getSprite(), x, y, 0, RenderLocation::RENDER_CENTER);
        m_renderer.submitRenderRequest(tileBuilding->getTCSprite(), x, y, 0, RenderLocation::RENDER_CENTER, true, m_playerColors[tileBuilding->getOwner()]);
      }
    }
    if (tile->bHasGeyser())
    {
      m_renderer.submitRenderRequest(m_manaGeyserSprite, x, y, 0, RenderLocation::RENDER_CENTER);
    }
    if (tile->bHasMana())
    {
      m_renderer.submitRenderRequest(m_manaBallSprite, x, y, 0, RenderLocation::RENDER_CENTER);
    }
    if (tile->bHasFoundation())
    {
      m_renderer.submitRenderRequest(m_foundationSprite, x, y, 0, RenderLocation::RENDER_CENTER);
    }
  }

  if (m_selectedTile != nullptr)
  {
    auto tileSprite = m_selectedTile->getSprite();
    float x, y;
    std::tie(x, y) = m_selectedTile->getPos();
    x *= tileSprite->m_size;
    y *= tileSprite->m_size;

    m_renderer.submitRenderRequest(m_selectionSprite, x, y, 0, RenderLocation::RENDER_CENTER, true, m_playerColors[m_currentPlayerId]);
    if (!m_selectedUnits.empty())
    {
      auto constructableBuildings = getConstructableBuildings(m_selectedTile, m_currentPlayerId);
      int dx = 5;
      int dy = 5;
      for (auto& cb : constructableBuildings)
      {
        m_renderer.submitRenderRequest(m_uiIconBackgroundBuildingSprite,
                                       m_virtualWidth - dx, m_virtualHeight - dy, 0,
                                       RenderLocation::RENDER_BOTTOM_RIGHT, false);
        m_renderer.submitRenderRequest(m_buildingLookUpTable[cb].getUISprite(),
                                       m_virtualWidth - dx, m_virtualHeight - dy, 0,
                                       RenderLocation::RENDER_BOTTOM_RIGHT, false);
        dx += m_buildingLookUpTable[cb].getUISprite()->m_size + 5;
      }
    }
    int dx = 5;
    int dy = -5;
    auto unitSet = m_units[m_selectedTile];
    std::vector<std::shared_ptr<SUnit>> tileUnits(unitSet.begin(), unitSet.end());
    std::copy(unitSet.begin(), unitSet.end(), tileUnits.begin());
    std::sort(tileUnits.rbegin(), tileUnits.rend(), compareUnitsUI);
    for (auto& unit : tileUnits)
    {
      m_renderer.submitRenderRequest(m_uiIconBackgroundSprite, dx, m_virtualHeight + dy, 0,
                                     RenderLocation::RENDER_BOTTOM_LEFT, false);
      m_renderer.submitRenderRequest(unit->getSprite(), dx, m_virtualHeight + dy, 0,
                                     RenderLocation::RENDER_BOTTOM_LEFT, false);
      if (m_selectedUnits.count(unit) > 0)
      {
        if (unit->getMoves() == unit->m_moves)
        {
          m_renderer.submitRenderRequest(m_buttonSelectionHasNotMovedSprite, dx, m_virtualHeight + dy, 0,
                                         RenderLocation::RENDER_BOTTOM_LEFT, false);
        }
        else if (!unit->bCanMove())
        {
          m_renderer.submitRenderRequest(m_buttonSelectionNoMovesSprite, dx, m_virtualHeight + dy, 0,
                                         RenderLocation::RENDER_BOTTOM_LEFT, false);
        }
        else
        {
          m_renderer.submitRenderRequest(m_buttonSelectionSprite, dx, m_virtualHeight + dy, 0,
                                         RenderLocation::RENDER_BOTTOM_LEFT, false);
        }
      }
      dx += unit->getSprite()->m_size + 4;
    }

    if (m_buildings.count(m_selectedTile) > 0)
    {
      m_renderer.submitRenderRequest(m_buildings[m_selectedTile]->getUISprite(), dx, m_virtualHeight + dy,
                                     0, RenderLocation::RENDER_BOTTOM_LEFT, false);
      m_renderer.submitRenderRequest(m_uiIconBackgroundBuildingSprite, dx, m_virtualHeight + dy,
                                     0, RenderLocation::RENDER_BOTTOM_LEFT, false);
      if (m_selectedBuilding == m_buildings[m_selectedTile])
      {
        m_renderer.submitRenderRequest(m_buttonSelectionLargeSprite, dx, m_virtualHeight + dy,
                                       0, RenderLocation::RENDER_BOTTOM_LEFT, false);
        auto bu = getBuildableUnits(m_selectedBuilding);
        dx = 5;
        dy = 5;
        for (const auto& u : bu)
        {
          m_renderer.submitRenderRequest(m_unitLookUpTable[u].getSprite(),
                                         m_virtualWidth - dx, m_virtualHeight - dy, 0,
                                         RenderLocation::RENDER_BOTTOM_RIGHT, false);
          m_renderer.submitRenderRequest(m_uiIconBackgroundSprite, m_virtualWidth - dx,
                                         m_virtualHeight - dy, 0,
                                         RenderLocation::RENDER_BOTTOM_RIGHT, false);
          dx += 25;
        }
        dx = 5;
        dy = 30;
        for (const auto& u : m_buildings[m_selectedTile]->getBuildQueue())
        {
          m_renderer.submitRenderRequest(m_unitLookUpTable[u].getSprite(),
                                         m_virtualWidth - dx, m_virtualHeight - dy, 0,
                                         RenderLocation::RENDER_BOTTOM_RIGHT, false);
          m_renderer.submitRenderRequest(m_uiIconBackgroundSprite, m_virtualWidth - dx,
                                         m_virtualHeight - dy, 0,
                                         RenderLocation::RENDER_BOTTOM_RIGHT, false);
          dx += 25;
        }
      }
    }
  }

  for (auto& p : m_units)
  {
    auto tile = p.first;
    auto tileSprite = tile->getSprite();
    float x, y;
    std::tie(x, y) = tile->getPos();
    x *= tileSprite->m_size;
    y *= tileSprite->m_size;

    std::shared_ptr<SUnit> su = nullptr;
    if (tile != m_selectedTile)
    {
      su = strongestUnit(tile);
    }
    else
    {
      su = strongestUnit(m_selectedUnits);
    }
    if (su == nullptr)
    {
      continue;
    }
    auto unitSprite = su->getSprite();
    auto unitTCSprite = su->getTCSprite();
    m_renderer.submitRenderRequest(unitSprite, x, y, 0, RenderLocation::RENDER_CENTER);
    auto tc = m_playerColors[su->getOwner()];
    m_renderer.submitRenderRequest(unitTCSprite, x, y, 0, RenderLocation::RENDER_CENTER, true, m_playerColors[su->getOwner()]);
  }

  m_renderer.submitRenderRequest(m_manaIconSprite, m_virtualWidth - 10, 10, 0,
                                 RenderLocation::RENDER_TOP_RIGHT, false);

  m_renderer.submitRenderRequest(m_endTurnButtonSprite, 0, 0, 0, RenderLocation::RENDER_TOP_LEFT, false);
  m_renderer.submitRenderRequest(m_endTurnButtonTCSprite, 0, 0, 0, RenderLocation::RENDER_TOP_LEFT, false, m_playerColors[m_currentPlayerId]);

  m_renderer.submitTextRenderRequest(std::to_string(m_players[m_currentPlayerId]->getCurrentManaAmount()), m_virtualWidth - 35, 10, m_manaIconSprite->m_size, 5, RenderLocation::RENDER_TOP_RIGHT);

  m_renderer.render();
}

void SGameManager::performUnitMovement(std::unordered_set<std::shared_ptr<SUnit>> units)
{
  for (const auto& unit : units)
  {
    if (m_unitTargetTiles.count(unit) == 0)
    {
      continue;
    }
    auto targetTile = getUnitTargetTile(unit);
    int tx, ty, cx, cy, d;
    std::tie(tx, ty) = targetTile->getPos();
    while (unit->bCanMove())
    {
      auto currentTile = getUnitTile(unit);
      std::tie(cx, cy) = currentTile->getPos();
      d = std::max(std::abs(tx - cx), std::abs(ty - cy));
      if ((d <= unit->m_attackRange or d == 1) and bTileHasEnemiesForPlayer(targetTile, m_currentPlayerId))
      {
        attackTile(unit, targetTile);
        if (!unit->bCanMove())
        {
          m_unitTargetTiles.erase(unit);
        }
        continue;
      }
      auto route = m_tiles->shortestPath(currentTile, targetTile);
      if (route.size() == 0)
      {
        stopUnitMovement(unit);
        break;
      }


      auto nextTile = route.front();
      moveUnitToTile(unit, nextTile);
    }
  }
}

void SGameManager::updateCamera()
{
  glm::vec2 cameraMovementInput{ 0, 0 };
  if (m_inputStruct.mouseX <= 5)
  {
    cameraMovementInput.x -= 1;
  }
  else if (m_inputStruct.mouseX >= (m_virtualWidth - 5))
  {
    cameraMovementInput.x += 1;
  }
  if (m_inputStruct.mouseY <= 5)
  {
    cameraMovementInput.y -= 1;
  }
  else if (m_inputStruct.mouseY >= m_virtualHeight - 5)
  {
    cameraMovementInput.y += 1;
  }
  if (glm::length(cameraMovementInput) != 0)
  {
    m_camera->pos += cameraMovementInput * m_camera->cameraSpeed * m_deltaTime;
    cameraMovementInput = { 0, 0 };
  }
  if (m_inputStruct.mouseWheelScroll != 0)
  {
    m_camera->currentZoom *= std::pow<float, float>(m_camera->zoomRate, m_inputStruct.mouseWheelScroll);
  }
}

std::shared_ptr<SNode> SGameManager::getClickedTile(int x, int y)
{
  auto dr = m_camera->pos - glm::vec2{ m_virtualWidth, m_virtualHeight } / 2.0f * m_camera->currentZoom;
  dr += glm::vec2{ x, y } * m_camera->currentZoom;
  dr += glm::vec2{ 30, 30 };

  return m_tiles->getTileAt(std::floor(dr.x / 60), std::floor(dr.y / 60));
}

void SGameManager::endTurn()
{
  m_selectedUnits.clear();
  m_selectedBuilding = nullptr;

  std::unordered_set<std::shared_ptr<SUnit>> movingUnits = {};
  for (const auto& [unit, tile] : m_unitTargetTiles)
  {
    if (unit->getOwner() == m_currentPlayerId)
    {
      movingUnits.insert(unit);
    }
  }

  performUnitMovement(movingUnits);
  if (m_currentPlayerId == m_numPlayers - 1)
  {
    generateMana();
  }

  for (const auto& [tile, tileUnits] : m_units)
  {
    for (auto& unit : tileUnits)
    {
      if (unit->getOwner() == m_currentPlayerId)
      {
        unit->refresh();
      }
    }
    auto su = strongestUnit(tile);
    if (su != nullptr)
    {
      m_players[su->getOwner()]->addResources(tile->getAndRemoveMana());
    }
  }

  for (const auto& [tile, building] : m_buildings)
  {
    if (building->getOwner() != m_currentPlayerId)
    {
      continue;
    }
    if (!building->bUnderConstruction())
    {
      addResourcesForPlayer(m_currentPlayerId, building->m_resourceGatherRate);

      if (building->finisingBuilding())
      {
        spawnUnitsForPlayer(tile, building->unitUnderConstruction(), m_currentPlayerId, 1);
      }
    }

    building->refresh();
  }

  m_currentPlayerId++;
  m_currentPlayerId %= m_numPlayers;
}

void SGameManager::handleRightClick()
{
  int x = m_inputStruct.mouseX;
  int y = m_inputStruct.mouseY;

  auto clickedTile = getClickedTile(x, y);

  if (m_selectedTile == nullptr or clickedTile == nullptr or clickedTile == m_selectedTile)
  {
    return;
  }

  for (auto& unit : m_selectedUnits)
  {
    startUnitMovement(unit, clickedTile);
  }

  performUnitMovement(m_selectedUnits);

  m_selectedUnits.clear();
}

void SGameManager::handleLeftClick()
{
  int x = m_inputStruct.mouseX;
  int y = m_inputStruct.mouseY;
  if (glm::length(glm::vec2{ x, y }) <= m_endTurnButtonSprite->m_size)
  {
    endTurn();
    return;
  }
  if (m_selectedTile != nullptr and y <= m_virtualHeight - 5 and y >= m_virtualHeight - 25)
  {
    int range = 5 + (m_units[m_selectedTile].size() - 1) * 24 + 20;
    bool bClickedUnitIcon = (x - 5) % 24 <= 20 and x >= 5 and x <= range;
    if (bClickedUnitIcon)
    {
      int index = (x - 5) / 24;
      auto unitSet = m_units[m_selectedTile];
      if (index < unitSet.size())
      {
        std::vector<std::shared_ptr<SUnit>> tileUnits(unitSet.begin(), unitSet.end());
        std::copy(unitSet.begin(), unitSet.end(), tileUnits.begin());
        std::sort(tileUnits.rbegin(), tileUnits.rend(), compareUnitsUI);
        auto targetUnit = tileUnits[index];
        if (targetUnit->getOwner() != m_currentPlayerId)
        {
          return;
        }
        bool bTargetUnitSelected = m_selectedUnits.count(targetUnit) > 0;
        m_selectedBuilding = nullptr;
        if (bTargetUnitSelected)
        {
          m_selectedUnits.erase(targetUnit);
        }
        else
        {
          if (m_inputStruct.bShiftDown())
          {
            m_selectedUnits.insert(targetUnit);
          }
          else
          {
            m_selectedUnits = { targetUnit };
          }
        }
        return;
      }
    }
    if (m_selectedBuilding != nullptr)
    {
      auto tu = getBuildableUnits(m_selectedBuilding);
      if (m_selectedBuilding != nullptr and tu.size() > 0)
      {
        int range = 5 + (tu.size() - 1) * 25 + 20;
        bool bClickedTrainingIcon = (m_virtualWidth - x - 5) % 25 <= 20 and
                                    x <= m_virtualWidth - 5 and x >= m_virtualWidth - range;
        if (bClickedTrainingIcon)
        {
          int index = (m_virtualWidth - x - 5) / 25;
          if (index < tu.size())
          {
            auto& targetUnit = *(std::next(tu.begin(), index));
            m_selectedBuilding->addUnitToBuildQueue(targetUnit);
            m_players[m_currentPlayerId]->removeResources(m_unitLookUpTable[targetUnit].m_resourceCost);
            return;
          }
        }
      }
    }
  }
  if (m_selectedUnits.size() > 0 and y <= m_virtualHeight - 5 and y >= m_virtualHeight - 35)
  {
    auto cb = getConstructableBuildings(m_selectedTile, m_currentPlayerId);
    bool bClickedBuildingIcon =
    (m_virtualWidth - x - 5) % 35 <= 30 and (m_virtualWidth - x - 5) / 35 < cb.size();
    if (bClickedBuildingIcon)
    {
      int index = (m_virtualWidth - x - 5) / 35;
      auto cb = getConstructableBuildings(m_selectedTile, m_currentPlayerId);
      if (index < cb.size())
      {
        auto& targetBuilding = *(std::next(cb.begin(), index));
        constructBuildingForPlayer(m_selectedTile, targetBuilding, m_currentPlayerId);
        m_selectedUnits = {};
      }
      return;
    }
    int dx = 5 + m_units[m_selectedTile].size() * 24;
    bool bSelectedBuiling = (m_buildings.count(m_selectedTile) and x >= dx and x <= dx + 30);
    if (bSelectedBuiling)
    {
      if (!playerOwnsBuilding(m_selectedTile, m_currentPlayerId))
      {
        return;
      }
      if (m_selectedBuilding == nullptr)
      {
        m_selectedUnits.clear();
        m_selectedBuilding = m_buildings[m_selectedTile];
      }
      else
      {
        m_selectedBuilding = nullptr;
      }
      return;
    }
  }
  if (m_selectedBuilding != nullptr and y >= m_virtualHeight - 50 and y <= m_virtualHeight - 30)
  {
    auto bq = m_selectedBuilding->getBuildQueue();
    int index = (m_virtualWidth - x - 5) / 25;
    bool bClickedUnitInTraining = (m_virtualWidth - x - 5) % 25 <= 20 and index < bq.size();
    if (bClickedUnitInTraining)
    {
      auto tu = m_selectedBuilding->removeUnitFromBuildQueue(index);
      m_players[m_currentPlayerId]->addResources(m_unitLookUpTable[tu].m_resourceCost);
      return;
    }
  }

  auto clickedTile = getClickedTile(x, y);

  m_selectedUnits.clear();
  m_selectedBuilding = nullptr;

  if (clickedTile == nullptr or
      clickedTile == m_selectedTile or
      (numUnitsForPlayer(clickedTile, m_currentPlayerId) == 0 and
       !playerOwnsBuilding(clickedTile, m_currentPlayerId)))
  {
    return;
  }

  m_selectedTile = clickedTile;
  auto sU = strongestUnitForPlayer(m_selectedTile, m_currentPlayerId);
  if (sU != nullptr)
  {
    m_selectedUnits = { sU };
  }
  else if (m_buildings.count(clickedTile) > 0)
  {
    auto tileBuilding = m_buildings[clickedTile];
    if (tileBuilding->getOwner() == m_currentPlayerId)
    {
      m_selectedBuilding = tileBuilding;
    }
  }
}

std::shared_ptr<SUnit> SGameManager::strongestUnit(std::shared_ptr<SNode> tile)
{
  const auto& tileUnits = m_units[tile];
  if (tileUnits.empty())
  {
    return nullptr;
  }
  return *std::max_element(tileUnits.begin(), tileUnits.end(), compareUnits);
}

std::shared_ptr<SUnit> SGameManager::strongestUnit(std::unordered_set<std::shared_ptr<SUnit>> units)
{
  {
    auto it = std::max_element(units.begin(), units.end(), compareUnits);
    if (it == units.end())
    {
      return nullptr;
    }
    return *it;
  }
}

bool SGameManager::bCoinToss(float chance)
{
  return m_coinTossDist(m_gen) <= chance;
}

void SGameManager::generateMana()
{
  for (auto& tile : m_tiles->getTiles())
  {
    if (tile->bHasGeyser() and !tile->bHasMana())
    {
      tile->addMana(m_manaGeyserValue(m_gen));
    }
    tryAddManaBall(tile);
  }
}

void SGameManager::tryAddManaBall(std::shared_ptr<SNode> tile)
{
  if (m_numManaBallRemaining > 0 and !tile->bHasGeyser() and !tile->bHasMana() and
      m_units[tile].empty() and m_buildings.count(tile) == 0 and bCoinToss(m_manaBallChance))
  {
    tile->addMana(m_manaBallValue(m_gen));
    m_numManaBallRemaining--;
  }
}

bool SGameManager::bCanConstructBuilding(std::shared_ptr<SNode> tile, std::string buildingId, int playerId)
{
  return tile->bHasFoundation() and m_buildings.count(tile) == 0 and
         m_players[playerId]->hasResources(m_buildingLookUpTable[buildingId].m_resourceCost);
}

std::unordered_set<std::string> SGameManager::getConstructableBuildings(std::shared_ptr<SNode> tile, int playerId)
{
  std::unordered_set<std::string> cb;
  for (auto& p : m_buildingLookUpTable)
  {
    auto bId = p.first;
    if (bCanConstructBuilding(tile, bId, playerId))
    {
      cb.insert(bId);
    }
  }
  return cb;
}

std::shared_ptr<SBuilding>
SGameManager::constructBuildingForPlayer(std::shared_ptr<SNode> tile, BUILDING_ID building, int playerId)
{
  if (m_buildingLookUpTable.count(building) == 0 or !bCanConstructBuilding(tile, building, playerId))
  {
    return nullptr;
  }
  auto newBuilding = std::make_shared<SBuilding>(m_buildingLookUpTable[building]);
  newBuilding->setOwner(playerId);
  m_buildings[tile] = newBuilding;
  m_players[playerId]->removeResources(newBuilding->m_resourceCost);
  return newBuilding;
}

std::shared_ptr<SBuilding>
SGameManager::spawnBuildingForPlayer(std::shared_ptr<SNode> tile, BUILDING_ID building, int playerId)
{
  if (m_buildingLookUpTable.count(building) == 0)
  {
    return nullptr;
  }
  auto newBuilding = std::make_shared<SBuilding>(m_buildingLookUpTable[building]);
  newBuilding->setOwner(playerId);
  m_buildings[tile] = newBuilding;
  newBuilding->finishConstruction();
  return newBuilding;
}

std::unordered_set<std::shared_ptr<SUnit>>
SGameManager::spawnUnitsForPlayer(std::shared_ptr<SNode> tile, UNIT_ID unit, int playerId, int num)
{
  if (m_unitLookUpTable.count(unit) == 0)
  {
    return {};
  }
  std::unordered_set<std::shared_ptr<SUnit>> nus;
  for (int i = 0; i < num; i++)
  {
    auto newUnit = std::make_shared<SUnit>(m_unitLookUpTable[unit]);
    newUnit->setOwner(playerId);
    m_units[tile].insert(newUnit);
    nus.insert(newUnit);
  }
  return nus;
}

bool SGameManager::bCanTrainUnit(std::shared_ptr<SBuilding> building, std::string unit)
{
  return !building->bUnderConstruction() and building->bCanTrainUnit(unit) and
         m_players[building->getOwner()]->hasResources(m_unitLookUpTable[unit].m_resourceCost);
}

std::unordered_set<std::string> SGameManager::getBuildableUnits(std::shared_ptr<SBuilding> building)
{
  std::unordered_set<std::string> bb;
  for (const auto& tu : building->m_buildableUnits)
  {
    if (bCanTrainUnit(building, tu))
    {
      bb.insert(tu);
    }
  }
  return bb;
}

std::shared_ptr<SNode> SGameManager::getUnitTile(std::shared_ptr<SUnit> unit)
{
  for (const auto& [t, us] : m_units)
  {
    if (us.count(unit) > 0)
    {
      return t;
    }
  }
  return nullptr;
}

std::shared_ptr<SNode> SGameManager::getUnitTargetTile(std::shared_ptr<SUnit> unit)
{
  if (m_unitTargetTiles.count(unit) > 0)
  {
    return m_unitTargetTiles[unit];
  }
  return nullptr;
}

void SGameManager::setUnitTargetTile(std::shared_ptr<SUnit> unit, std::shared_ptr<SNode> tile)
{
  m_unitTargetTiles[unit] = tile;
}

void SGameManager::resetUnitTargetTile(std::shared_ptr<SUnit> unit)
{
  if (m_unitTargetTiles.count(unit) > 0)
  {
    m_unitTargetTiles.erase(unit);
  }
}

bool SGameManager::bUnitFinishedRoute(std::shared_ptr<SUnit> unit)
{
  return getUnitTile(unit) == getUnitTargetTile(unit);
}

void SGameManager::moveUnitToTile(std::shared_ptr<SUnit> unit, std::shared_ptr<SNode> tile)
{
  addResourcesForPlayer(unit->getOwner(), tile->getAndRemoveMana());
  const auto& currentTile = getUnitTile(unit);
  unit->removeMoves(tile->m_movementCost);
  m_units[tile].insert(unit);
  m_units[currentTile].erase(unit);
}

void SGameManager::attackTile(std::shared_ptr<SUnit> unit, std::shared_ptr<SNode> tile)
{
  const auto& currentTile = getUnitTile(unit);
  int tx, ty, cx, cy, d;
  std::tie(cx, cy) = currentTile->getPos();
  std::tie(tx, ty) = tile->getPos();
  d = std::max(std::abs(tx - cx), std::abs(ty - cy));
  if (d == 1 and unit->m_attackRange == 0)
  {
  }
  else if (unit->m_attackRange < d)
  {
    return;
  }

  auto su = strongestUnit(tile);
  auto it = m_buildings.find(tile);
  std::shared_ptr<SCombatReady> target = nullptr;
  bool bUnit = false;
  bool bBuilding = false;

  if (su != nullptr and su->getOwner() != unit->getOwner())
  {
    target = su;
    bUnit = true;
  }
  else if (it != m_buildings.end() and it->second->getOwner() != unit->getOwner())
  {
    target = it->second;
    bBuilding = true;
  }

  if (target != nullptr)
  {
    int uDmg = unit->dealDamage(m_coinTossDist(m_gen), tile->m_accuracyModifier);
    int xp = target->applyDamage(uDmg, tile->m_armorModifier);
    if (xp > 0)
    {
      std::cout << unit->m_ID << " attacked " << target->m_ID << " for " << xp << " damage. " << target->m_ID << " now has " << target->getHealth() << " health" << std::endl;
    }
    else
    {
      std::cout << unit->m_ID << " missed!" << std::endl;
    }
    unit->addXP(xp);
    unit->removeMoves(unit->getMoves());
    if (target->bIsDead())
    {
      unit->addXP(xp);
      if (bUnit)
      {
        m_units[tile].erase(su);
        m_unitTargetTiles.erase(su);
      }
      else if (bBuilding)
      {
        m_buildings.erase(it);
      }
      if (unit->m_attackRange == 0 and !bTileHasEnemiesForPlayer(tile, unit->getOwner()))
      {
        unit->addMoves(1);
      }
    }
    else if (target->m_damage > 0)
    {
      if (d == 1 and unit->m_attackRange == 0)
      {
      }
      else if (target->m_attackRange < d)
      {
        return;
      }
      int tDmg = target->dealDamage(m_coinTossDist(m_gen), currentTile->m_accuracyModifier);
      xp = unit->applyDamage(tDmg, currentTile->m_armorModifier);
      if (xp > 0)
      {
        std::cout << target->m_ID << " counterattacked " << unit->m_ID << " for " << xp << " damage. " << unit->m_ID << " now has " << unit->getHealth() << " health" << std::endl;
      }
      else
      {
        std::cout << target->m_ID << " missed!" << std::endl;
      }
      if (bUnit)
      {
        su->addXP(xp);
      }
      if (unit->bIsDead())
      {
        if (bUnit)
        {
          su->addXP(xp);
        }
        m_units[currentTile].erase(unit);
        m_unitTargetTiles.erase(unit);
      }
    }
  }
}

void SGameManager::startUnitMovement(std::shared_ptr<SUnit> unit, std::shared_ptr<SNode> tile)
{
  setUnitTargetTile(unit, tile);
}

void SGameManager::stopUnitMovement(std::shared_ptr<SUnit> unit)
{
  resetUnitTargetTile(unit);
}

void SGameManager::addResourcesForPlayer(int playerId, int amount)
{
  m_players[playerId]->addResources(amount);
}

bool SGameManager::playerOwnsBuilding(std::shared_ptr<SNode> tile, int playerId)
{
  auto it = m_buildings.find(tile);
  return it != m_buildings.end() and (*it).second->getOwner() == playerId;
}

int SGameManager::numUnitsForPlayer(std::shared_ptr<SNode> tile, int playerId)
{
  int numUnits = 0;
  for (const auto& unit : m_units[tile])
  {
    if (unit->getOwner() == playerId)
    {
      numUnits++;
    }
  }
  return numUnits;
}

std::shared_ptr<SUnit> SGameManager::strongestUnitForPlayer(std::shared_ptr<SNode> tile, int playerId)
{
  std::vector<std::shared_ptr<SUnit>> tileUnits(m_units[tile].begin(), m_units[tile].end());
  std::sort(tileUnits.begin(), tileUnits.end(), compareUnits);
  std::reverse(tileUnits.begin(), tileUnits.end());
  std::shared_ptr<SUnit> su = nullptr;
  for (const auto& unit : tileUnits)
  {
    if (unit->getOwner() == playerId)
    {
      su = unit;
      break;
    }
  }
  return su;
}

bool SGameManager::bTileHasEnemiesForPlayer(std::shared_ptr<SNode> tile, int playerId)
{
  const auto& su = strongestUnit(tile);
  if (su != nullptr and su->getOwner() != playerId)
  {
    return true;
  }
  auto it = m_buildings.find(tile);
  if (it != m_buildings.end() and it->second->getOwner() != playerId)
  {
    return true;
  }
  return false;
}

//int SGameManager::countPlayerBuildings(int playerId)
//{
//  int count = 0;
//  for (const auto& [tile, building] : m_buildings)
//  {
//    if (building->getOwner() == playerId)
//    {
//      count++;
//    }
//  }
//  return count;
//}

//void SGameManager::checkAndRemovePlayers()
//{
//  for (int i = 0; i < m_numPlayers; i++)
//  {
//    if (countPlayerBuildings(i) == 0)
//    {
//      std::cout << "Player " << i << " has been defeated!" << std::endl;
//      m_players.erase(i);
//    }
//  }
//  if (m_players.size() == 1)
//  {
//    std::cout << "Player " << m_players.begin()->first << " wins!" << std::endl;
//    m_bQuit = true;
//  }
//}
