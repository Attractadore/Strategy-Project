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

void SGameInput::reset() {
  mouseWheelScroll = 0;
  mouseX = -1;
  mouseY = -1;
  mouseClickedButtons.clear();
  keyboardPressedKeys.clear();
  keyboardReleasedKeys.clear();
}

void SGameInput::activateKeyboardKeys(std::unordered_set<KeyboardKey> keys) {
  keyboardPressedKeys.insert(keys.begin(), keys.end());
  keyboardActiveKeys.insert(keys.begin(), keys.end());
}

void SGameInput::pressKeyboardKey(KeyboardKey key) {
  keyboardPressedKeys.insert(key);
  keyboardActiveKeys.insert(key);
}

void SGameInput::releaseKeyboardKeys(std::unordered_set<KeyboardKey> keys) {
  keyboardReleasedKeys.insert(keys.begin(), keys.end());
  keyboardActiveKeys.erase(keys.begin(), keys.end());
}

void SGameInput::releaseKeyboardKey(KeyboardKey key) {
  keyboardReleasedKeys.insert(key);
  keyboardActiveKeys.erase(key);
}

bool SGameInput::bShiftDown() { return bRightShiftDown or bLeftShiftDown; }

void SGameInput::clickMouseButton(MouseButton button) {
  mouseClickedButtons.insert(button);
}

bool compareUnits(const std::shared_ptr<SUnit> &lhs,
                  const std::shared_ptr<SUnit> &rhs) {
  return lhs->getCurrentHealth() * lhs->m_damage * lhs->m_accuracy <
         rhs->getCurrentHealth() * rhs->m_damage * rhs->m_accuracy;
}

SGameManager::SGameManager() {
  defaultPlayer = SPlayer(0);
  m_players.push_back(&defaultPlayer);

  m_worldWidth = 32;
  m_worldHeight = 16;
  m_numManaBallRemaining = int(m_worldWidth * m_worldHeight * m_wsmbRatio);

  m_tiles = std::make_shared<SNodeGraph>(m_worldWidth, m_worldHeight);
  m_camera = std::make_shared<SCamera>();

  m_camera->pos = {0, 0};
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

  std::shared_ptr<SSprite> defaultUnitSprite =
      std::make_shared<SSprite>("./assets/art/spearman.png", 1, 20, 3);
  m_selectionSprite =
      std::make_shared<SSprite>("./assets/art/selection.png", 1, 60, 4);
  m_endTurnButtonSprite =
      std::make_shared<SSprite>("./assets/art/ui/endTurn.png", 1, 150, 5);
  m_buttonSelectionSprite = std::make_shared<SSprite>(
      "./assets/art/ui/buttonSelection.png", 1, 20, 5);
  m_buttonSelectionLargeSprite = std::make_shared<SSprite>(
      "./assets/art/ui/buttonSelection.png", 1, 30, 5);
  m_manaIconSprite =
      std::make_shared<SSprite>("./assets/art/ui/manaIcon.png", 1, 15, 5);
  m_manaGeyserSprite =
      std::make_shared<SSprite>("./assets/art/manaGeyser.png", 1, 50, 2);
  m_manaBallSprite =
      std::make_shared<SSprite>("./assets/art/manaBall.png", 1, 30, 2.5);
  m_foundationSprite = std::make_shared<SSprite>(
      "./assets/art/ancientFoundation.png", 1, 60, 1.5);
  m_uiIconBackgroundSprite =
      std::make_shared<SSprite>("./assets/art/ui/uiBackground.png", 1, 20, 2.9);
  m_uiIconBackgroundBuildingSprite =
      std::make_shared<SSprite>("./assets/art/ui/uiBackground.png", 1, 30, 4.9);
  m_buildingConstructionSprite = std::make_shared<SSprite>(
      "./assets/art/buildingConstruction.png", 1, 60, 2);
  auto buildingBarracksIconSprite =
      std::make_shared<SSprite>("./assets/art/building.png", 1, 30, 5);
  auto buildingShrineIconSprite =
      std::make_shared<SSprite>("./assets/art/shrine.png", 1, 30, 5);
  auto productionBuildingSprite =
      std::make_shared<SSprite>("./assets/art/building.png", 1, 60, 2);
  auto shrineSprite =
      std::make_shared<SSprite>("./assets/art/shrine.png", 1, 60, 2);

  SUnit spearman{"UNIT_SPEARMAN"};
  std::unordered_map<std::string, float> spearmanStats = {
      {"maxHP", 10},        {"damage", 1},     {"accuracy", 1},
      {"maxMoves", 2},      {"supplyCost", 1}, {"buildTime", 3},
      {"resourceCost", 50}, {"size", 4}};
  spearman.setSprite(defaultUnitSprite);
  spearman.setParams(spearmanStats);
  m_unitLookUpTable["UNIT_SPEARMAN"] = spearman;

  SBuilding productionBuilding;
  productionBuilding.setUnitLookUpTable(&m_unitLookUpTable);
  productionBuilding.setSprite(productionBuildingSprite);
  productionBuilding.setuiIcon(buildingBarracksIconSprite);
  productionBuilding.m_resourceGatherRate = 0;
  productionBuilding.m_constructionTime = 10;
  productionBuilding.m_armour = 0;
  productionBuilding.m_maxHealth = 400;
  productionBuilding.m_resourceCost = 800;
  productionBuilding.m_buildableUnits = {"UNIT_SPEARMAN"};
  SBuilding resourceBuilding;
  resourceBuilding.setUnitLookUpTable(&m_unitLookUpTable);
  resourceBuilding.setSprite(shrineSprite);
  resourceBuilding.setuiIcon(buildingShrineIconSprite);
  resourceBuilding.m_resourceGatherRate = 30;
  resourceBuilding.m_constructionTime = 8;
  resourceBuilding.m_armour = 2;
  resourceBuilding.m_maxHealth = 200;
  resourceBuilding.m_resourceCost = 600;
  m_buildingLookUpTable["BUILDING_BARRACKS"] = productionBuilding;
  m_buildingLookUpTable["BUILDING_SHRINE"] = resourceBuilding;

  m_gen.seed(std::random_device()());

  std::unordered_set<std::shared_ptr<SNode>> foundationTiles;

  for (auto &t : m_tiles->getTiles()) {
    if (bCoinToss(m_geyserChance) and m_buildings.count(t) == 0) {
      t->addGeyser();
      t->addMana(m_manaGeyserValue(m_gen));
    }
    tryAddManaBall(t);
    if (!t->bHasGeyser() and bCoinToss(m_foundationChance)) {
      t->addFoundation();
      foundationTiles.insert(t);
    }
  }

  std::uniform_int_distribution<> dist(0, foundationTiles.size() - 1);
  auto spawnTile = *std::next(foundationTiles.begin(), dist(m_gen));
  spawnBuidlingForPlayer(spawnTile, "BUILDING_BARRACKS", defaultPlayer);
  spawnUnitsForPlayer(spawnTile, "UNIT_SPEARMAN", defaultPlayer, 3);

  m_bQuit = false;
}

SGameManager::~SGameManager() {}

void SGameManager::run() {
  auto time = std::chrono::steady_clock::now();

  while (!m_bQuit) {

    auto newTime = std::chrono::steady_clock::now();
    m_deltaTime = m_deltaTime =
        std::chrono::duration_cast<std::chrono::microseconds>(newTime - time)
            .count() /
        1000000.0f;
    time = newTime;

    if (m_maxFPS != 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(std::max(
          0, static_cast<int>(1000000 * (m_maxFrameTime - m_deltaTime)))));
    }

    handleInput();
    handleLogic();
    handleRendering();
  }
}

void SGameManager::handleInput() {
  SDL_Event e;
  int x, y;
  SDL_GetMouseState(&x, &y);
  x /= m_realVirtualRatio;
  y /= m_realVirtualRatio;

  m_inputStruct.mouseX = x;
  m_inputStruct.mouseY = y;

  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      m_bQuit = true;
    }

    else if (e.type == SDL_KEYDOWN and !e.key.repeat) {
      auto ks = e.key.keysym.scancode;
      if (ks == SDL_SCANCODE_ESCAPE) {
        m_inputStruct.pressKeyboardKey(KeyboardKey::KEY_ESCAPE);
      } else if (ks == SDL_SCANCODE_RETURN) {
        m_inputStruct.pressKeyboardKey(KeyboardKey::KEY_ENTER);
      } else if (ks == SDL_SCANCODE_RSHIFT) {
        m_inputStruct.pressKeyboardKey(KeyboardKey::KEY_RIGHT_SHIFT);
        m_inputStruct.bRightShiftDown = true;
      } else if (ks == SDL_SCANCODE_LSHIFT) {
        m_inputStruct.pressKeyboardKey(KeyboardKey::KEY_LEFT_SHIFT);
        m_inputStruct.bLeftShiftDown = true;
      }
    } else if (e.type == SDL_KEYUP and !e.key.repeat) {
      auto ks = e.key.keysym.scancode;
      if (ks == SDL_SCANCODE_ESCAPE) {
        m_inputStruct.releaseKeyboardKey(KeyboardKey::KEY_ESCAPE);
      } else if (ks == SDL_SCANCODE_RETURN) {
        m_inputStruct.releaseKeyboardKey(KeyboardKey::KEY_ENTER);
      } else if (ks == SDL_SCANCODE_RSHIFT) {
        m_inputStruct.releaseKeyboardKey(KeyboardKey::KEY_RIGHT_SHIFT);
        m_inputStruct.bRightShiftDown = false;
      } else if (ks == SDL_SCANCODE_LSHIFT) {
        m_inputStruct.releaseKeyboardKey(KeyboardKey::KEY_LEFT_SHIFT);
        m_inputStruct.bLeftShiftDown = false;
      }
    }

    else if (e.type == SDL_MOUSEWHEEL) {
      m_inputStruct.mouseWheelScroll += e.wheel.y;
    }

    else if (e.type == SDL_MOUSEBUTTONDOWN) {

      if (e.button.button == SDL_BUTTON_LEFT) {
        m_inputStruct.clickMouseButton(MouseButton::BUTTON_LEFT);
      }

      else if (e.button.button == SDL_BUTTON_RIGHT) {
        m_inputStruct.clickMouseButton(MouseButton::BUTTON_RIGHT);
      }
    }
  }
}

void SGameManager::handleLogic() {
  for (auto &k : m_inputStruct.keyboardPressedKeys) {
    if (k == KeyboardKey::KEY_ESCAPE) {
      m_bQuit = true;
    } else if (k == KeyboardKey::KEY_ENTER) {
      endTurn();
    }
  }
  for (auto &m : m_inputStruct.mouseClickedButtons) {
    if (m == MouseButton::BUTTON_LEFT) {
      handleLeftClick();
    } else if (m == MouseButton::BUTTON_RIGHT) {
      handleRightClick();
    }
  }
  updateCamera();

  if (m_selectedUnits.size() == 0 and m_selectedBuilding == nullptr) {
    m_selectedTile = nullptr;
  }

  m_inputStruct.reset();
}

void SGameManager::handleRendering() {

  for (auto &tile : m_tiles->getTiles()) {
    auto sprite = tile->getSprite();
    float x, y;
    std::tie(x, y) = tile->getPos();
    x *= sprite->m_size;
    y *= sprite->m_size;
    m_renderer.submitRenderRequest(tile->getSprite(), x, y, 0,
                                   RenderLocation::RENDER_CENTER);
    if (m_buildings.count(tile) > 0) {
      auto tileBuidling = m_buildings[tile];
      if (tileBuidling->bUnderConstruction()) {
        m_renderer.submitRenderRequest(m_buildingConstructionSprite, x, y, 0,
                                       RenderLocation::RENDER_CENTER);
      } else {
        m_renderer.submitRenderRequest(tileBuidling->getSprite(), x, y, 0,
                                       RenderLocation::RENDER_CENTER);
      }
    }
    if (tile->bHasGeyser()) {
      m_renderer.submitRenderRequest(m_manaGeyserSprite, x, y, 0,
                                     RenderLocation::RENDER_CENTER);
    }
    if (tile->bHasMana()) {
      m_renderer.submitRenderRequest(m_manaBallSprite, x, y, 0,
                                     RenderLocation::RENDER_CENTER);
    }
    if (tile->bHasFoundation()) {
      m_renderer.submitRenderRequest(m_foundationSprite, x, y, 0,
                                     RenderLocation::RENDER_CENTER);
    }
  }

  if (m_selectedTile != nullptr) {
    auto tileSprite = m_selectedTile->getSprite();
    float x, y;
    std::tie(x, y) = m_selectedTile->getPos();
    x *= tileSprite->m_size;
    y *= tileSprite->m_size;

    m_renderer.submitRenderRequest(m_selectionSprite, x, y, 0,
                                   RenderLocation::RENDER_CENTER);
    if (!m_selectedUnits.empty()) {
      auto constructableBuildings =
          getConstructableBuidlings(m_selectedTile, defaultPlayer);
      int dx = 5;
      int dy = 5;
      for (auto &cb : constructableBuildings) {
        m_renderer.submitRenderRequest(
            m_uiIconBackgroundBuildingSprite, m_virtualWidth - dx,
            m_virtualHeight - dy, 0, RenderLocation::RENDER_BOTTOM_RIGHT,
            false);
        m_renderer.submitRenderRequest(
            m_buildingLookUpTable[cb].getuiIcon(), m_virtualWidth - dx,
            m_virtualHeight - dy, 0, RenderLocation::RENDER_BOTTOM_RIGHT,
            false);
        dx += m_buildingLookUpTable[cb].getuiIcon()->m_size + 5;
      }
    }
    int dx = 5;
    int dy = -5;
    auto unitSet = m_units[m_selectedTile];
    std::vector<std::shared_ptr<SUnit>> tileUnits(unitSet.begin(),
                                                  unitSet.end());
    std::copy(unitSet.begin(), unitSet.end(), tileUnits.begin());
    std::sort(tileUnits.rbegin(), tileUnits.rend(), compareUnits);
    for (auto &unit : tileUnits) {
      m_renderer.submitRenderRequest(m_uiIconBackgroundSprite, dx,
                                     m_virtualHeight + dy, 0,
                                     RenderLocation::RENDER_BOTTOM_LEFT, false);
      m_renderer.submitRenderRequest(unit->getSprite(), dx,
                                     m_virtualHeight + dy, 0,
                                     RenderLocation::RENDER_BOTTOM_LEFT, false);
      if (m_selectedUnits.count(unit) > 0) {
        m_renderer.submitRenderRequest(
            m_buttonSelectionSprite, dx, m_virtualHeight + dy, 0,
            RenderLocation::RENDER_BOTTOM_LEFT, false);
      }
      dx += unit->getSprite()->m_size + 4;
    }

    if (m_buildings.count(m_selectedTile) > 0) {
      m_renderer.submitRenderRequest(m_buildings[m_selectedTile]->getuiIcon(),
                                     dx, m_virtualHeight + dy, 0,
                                     RenderLocation::RENDER_BOTTOM_LEFT, false);
      m_renderer.submitRenderRequest(m_uiIconBackgroundBuildingSprite, dx,
                                     m_virtualHeight + dy, 0,
                                     RenderLocation::RENDER_BOTTOM_LEFT, false);
      if (m_selectedBuilding == m_buildings[m_selectedTile]) {
        m_renderer.submitRenderRequest(
            m_buttonSelectionLargeSprite, dx, m_virtualHeight + dy, 0,
            RenderLocation::RENDER_BOTTOM_LEFT, false);
        auto bu = getBuildableUnits(m_selectedBuilding);
        dx = 5;
        dy = 5;
        for (const auto &u : bu) {
          m_renderer.submitRenderRequest(
              m_unitLookUpTable[u].getSprite(), m_virtualWidth - dx,
              m_virtualHeight - dy, 0, RenderLocation::RENDER_BOTTOM_RIGHT,
              false);
          m_renderer.submitRenderRequest(
              m_uiIconBackgroundSprite, m_virtualWidth - dx,
              m_virtualHeight - dy, 0, RenderLocation::RENDER_BOTTOM_RIGHT,
              false);
          dx += 25;
        }
        dx = 5;
        dy = 30;
        for (const auto &u : m_buildings[m_selectedTile]->getBuildQueue()) {
          m_renderer.submitRenderRequest(
              m_unitLookUpTable[u].getSprite(), m_virtualWidth - dx,
              m_virtualHeight - dy, 0, RenderLocation::RENDER_BOTTOM_RIGHT,
              false);
          m_renderer.submitRenderRequest(
              m_uiIconBackgroundSprite, m_virtualWidth - dx,
              m_virtualHeight - dy, 0, RenderLocation::RENDER_BOTTOM_RIGHT,
              false);
          dx += 25;
        }
      }
    }
  }

  for (auto &p : m_units) {
    auto tile = p.first;
    auto tileSprite = tile->getSprite();
    float x, y;
    std::tie(x, y) = tile->getPos();
    x *= tileSprite->m_size;
    y *= tileSprite->m_size;

    auto unitSet = p.second;
    if (unitSet.empty()) {
      continue;
    }
    auto strongestUnit =
        std::max_element(unitSet.begin(), unitSet.end(), compareUnits);
    auto unitSprite = (*strongestUnit)->getSprite();
    m_renderer.submitRenderRequest(unitSprite, x, y, 0,
                                   RenderLocation::RENDER_CENTER);
  }

  m_renderer.submitRenderRequest(m_manaIconSprite, m_virtualWidth - 10, 10, 0,
                                 RenderLocation::RENDER_TOP_RIGHT, false);

  m_renderer.submitRenderRequest(m_endTurnButtonSprite, 0, 0, 0,
                                 RenderLocation::RENDER_TOP_LEFT, false);

  m_renderer.submitTextRenderRequest(
      std::to_string(defaultPlayer.getCurrentManaAmount()), m_virtualWidth - 35,
      10, m_manaIconSprite->m_size, 5, RenderLocation::RENDER_TOP_RIGHT);

  m_renderer.render();
}

void SGameManager::performUnitMovement(
    std::unordered_set<std::shared_ptr<SUnit>> units) {
  for (auto &unit : units) {
    auto it = std::find(m_movingUnits.begin(), m_movingUnits.end(), unit);
    if (it == m_movingUnits.end()) {
      continue;
    }
    while (true) {

      if (unit->finishedRoute()) {
        m_movingUnits.erase(it);
        break;
      }

      if (!unit->canAdvanceRoute()) {
        break;
      }

      auto route =
          m_tiles->shortestPath(unit->getCurrentTile(), unit->getTargetTile());

      if (route.size() == 0) {
        break;
      }

      auto nextTile = route.front();
      auto currentTile = unit->getCurrentTile();
      m_players[unit->getOwner()]->addResources(nextTile->getAndRemoveMana());
      m_units[nextTile].insert(unit);
      m_units[currentTile].erase(unit);
      unit->moveTile(nextTile);
    }
  }
}

void SGameManager::updateCamera() {
  glm::vec2 cameraMovementInput{0, 0};
  if (m_inputStruct.mouseX <= 5) {
    cameraMovementInput.x -= 1;
  } else if (m_inputStruct.mouseX >= (m_virtualWidth - 5)) {
    cameraMovementInput.x += 1;
  }
  if (m_inputStruct.mouseY <= 5) {
    cameraMovementInput.y -= 1;
  } else if (m_inputStruct.mouseY >= m_virtualHeight - 5) {
    cameraMovementInput.y += 1;
  }
  if (glm::length(cameraMovementInput) != 0) {
    m_camera->pos += cameraMovementInput * m_camera->cameraSpeed * m_deltaTime;
    cameraMovementInput = {0, 0};
  }
  if (m_inputStruct.mouseWheelScroll != 0) {
    m_camera->currentZoom *= std::pow<float, float>(
        m_camera->zoomRate, m_inputStruct.mouseWheelScroll);
  }
}

std::shared_ptr<SNode> SGameManager::getClickedTile(int x, int y) {
  auto dr = m_camera->pos - glm::vec2{m_virtualWidth, m_virtualHeight} / 2.0f *
                                m_camera->currentZoom;
  dr += glm::vec2{x, y} * m_camera->currentZoom;
  dr += glm::vec2{30, 30};

  return m_tiles->getTileAt(std::floor(dr.x / 60), std::floor(dr.y / 60));
}

void SGameManager::endTurn() {
  m_selectedTile = nullptr;
  performUnitMovement(m_movingUnits);
  generateMana();
  for (auto &p : m_units) {
    auto unitVec = p.second;
    if (!unitVec.empty()) {
      m_players[(*unitVec.begin())->getOwner()]->addResources(
          p.first->getAndRemoveMana());
    }
    for (auto &unit : unitVec) {
      unit->refresh();
    }
  }
  for (auto &p : m_buildings) {
    auto building = p.second;
    if (!building->bUnderConstruction()) {
      m_players[building->getOwner()]->addResources(
          building->m_resourceGatherRate);
      building->isBuilding();
      if (building->finisingBuilding()) {
        auto tile = p.first;
        auto newUnit = std::make_shared<SUnit>(
            m_unitLookUpTable[building->unitUnderConstruction()]);
        newUnit->setCurrentTile(tile);
        newUnit->setOwner(defaultPlayer.getPlayerId());
        m_units[tile].insert(newUnit);
      }
    }
    building->refresh();
  }
}

void SGameManager::handleRightClick() {
  int x = m_inputStruct.mouseX;
  int y = m_inputStruct.mouseY;
  auto clickedTile = getClickedTile(x, y);
  if (m_selectedTile == nullptr or clickedTile == nullptr or
      clickedTile == m_selectedTile) {
    return;
  }

  auto movingUnits = m_selectedUnits;

  for (auto &unit : movingUnits) {
    unit->setTargetTile(clickedTile);
    m_movingUnits.insert(unit);
  }

  performUnitMovement(movingUnits);

  m_selectedUnits.clear();
}

void SGameManager::handleLeftClick() {
  int x = m_inputStruct.mouseX;
  int y = m_inputStruct.mouseY;
  if (glm::length(glm::vec2{x, y}) <= m_endTurnButtonSprite->m_size) {
    endTurn();
    return;
  }
  if (m_selectedTile != nullptr and y <= m_virtualHeight - 5 and
      y >= m_virtualHeight - 25) {
    int range = 5 + (m_units[m_selectedTile].size() - 1) * 24 + 20;
    //    std::cout << "Range " << range << std::endl;
    bool bClickedUnitIcon = (x - 5) % 24 <= 20 and x >= 5 and x <= range;
    if (bClickedUnitIcon) {
      std::cout << "Clicked unit selection card " << std::endl;
      int index = (x - 5) / 24;
      auto unitSet = m_units[m_selectedTile];
      if (index < unitSet.size()) {
        std::vector<std::shared_ptr<SUnit>> tileUnits(unitSet.begin(),
                                                      unitSet.end());
        std::copy(unitSet.begin(), unitSet.end(), tileUnits.begin());
        std::sort(tileUnits.rbegin(), tileUnits.rend(), compareUnits);
        auto targetUnit = tileUnits[index];
        bool bTargetUnitSelected = m_selectedUnits.count(targetUnit) > 0;
        m_selectedBuilding = nullptr;
        if (bTargetUnitSelected) {
          m_selectedUnits.erase(targetUnit);
        } else {
          if (m_inputStruct.bShiftDown()) {
            m_selectedUnits.insert(targetUnit);
          } else {
            m_selectedUnits = {targetUnit};
          }
        }
        return;
      }
      //      else if (y >= (5 + index * 20) and y <= (5 + index * 20 + 30) and
      //                 m_buildings.count(m_selectedTile)) {
      //        m_selectedBuilding = m_buildings[m_selectedTile];
      //      }
    }
    if (m_selectedBuilding != nullptr) {
      auto tu = getBuildableUnits(m_selectedBuilding);
      if (m_selectedBuilding != nullptr and tu.size() > 0) {
        int range = 5 + (tu.size() - 1) * 25 + 20;
        bool bClickedTrainingIcon = (m_virtualWidth - x - 5) % 25 <= 20 and
                                    x <= m_virtualWidth - 5 and
                                    x >= m_virtualWidth - range;
        if (bClickedTrainingIcon) {
          std::cout << "Clicked unit training card " << std::endl;
          int index = (m_virtualWidth - x - 5) / 25;
          if (index < tu.size()) {
            auto &targetUnit = *(std::next(tu.begin(), index));
            m_selectedBuilding->addUnitToBuildQueue(targetUnit);
            defaultPlayer.removeResources(
                m_unitLookUpTable[targetUnit].m_resourceCost);
            return;
          }
        }
      }
    }
  }
  if (m_selectedUnits.size() > 0 and y <= m_virtualHeight - 5 and
      y >= m_virtualHeight - 35) {
    auto cb = getConstructableBuidlings(m_selectedTile, defaultPlayer);
    bool bClickedBuildingIcon = (m_virtualWidth - x - 5) % 35 <= 30 and
                                (m_virtualWidth - x - 5) / 35 < cb.size();
    if (bClickedBuildingIcon) {
      int index = (m_virtualWidth - x - 5) / 35;
      auto cb = getConstructableBuidlings(m_selectedTile, defaultPlayer);
      if (index < cb.size()) {
        auto &targetBuilding = *(std::next(cb.begin(), index));
        constructBuidlingForPlayer(m_selectedTile, targetBuilding,
                                   defaultPlayer);
        m_selectedUnits = {};
      }
      return;
    }
    int dx = 5 + m_units[m_selectedTile].size() * 24;
    bool bSelectedBuiling =
        (m_buildings.count(m_selectedTile) and x >= dx and x <= dx + 30);
    if (bSelectedBuiling) {
      if (m_selectedBuilding == nullptr) {
        m_selectedUnits.clear();
        m_selectedBuilding = m_buildings[m_selectedTile];
      } else {
        m_selectedBuilding = nullptr;
      }
      return;
    }
  }
  if (m_selectedBuilding != nullptr and y >= m_virtualHeight - 50 and
      y <= m_virtualHeight - 30) {
    auto bq = m_selectedBuilding->getBuildQueue();
    int index = (m_virtualWidth - x - 5) / 25;
    bool bClickedUnitInTraining =
        (m_virtualWidth - x - 5) % 25 <= 20 and index < bq.size();
    if (bClickedUnitInTraining) {
      auto tu = m_selectedBuilding->removeUnitFromBuildQueue(index);
      defaultPlayer.addResources(m_unitLookUpTable[tu].m_resourceCost);
      return;
    }
  }

  auto clickedTile = getClickedTile(x, y);
  if (clickedTile == nullptr or (m_units[clickedTile].size() == 0 and
                                 m_buildings.count(clickedTile) == 0)) {
    m_selectedUnits.clear();
    m_selectedBuilding = nullptr;
    return;
  }

  m_selectedUnits.clear();
  m_selectedBuilding = nullptr;
  if (clickedTile == m_selectedTile) {
    m_selectedBuilding = nullptr;
  } else {
    m_selectedTile = clickedTile;
    auto sU = strongestUnit(m_selectedTile);
    if (sU != nullptr) {
      m_selectedUnits = {sU};
    } else if (m_buildings.count(clickedTile)) {
      m_selectedBuilding = m_buildings[clickedTile];
    }
  }
}

std::shared_ptr<SUnit>
SGameManager::strongestUnit(std::shared_ptr<SNode> tile) {
  if (m_units[tile].empty()) {
    return nullptr;
  }
  return *std::max_element(m_units[tile].begin(), m_units[tile].end(),
                           compareUnits);
}

bool SGameManager::bCoinToss(float chance) {
  return m_coinTossDist(m_gen) <= chance;
}

void SGameManager::generateMana() {
  for (auto &tile : m_tiles->getTiles()) {
    if (tile->bHasGeyser() and !tile->bHasMana()) {
      tile->addMana(m_manaGeyserValue(m_gen));
    }
    tryAddManaBall(tile);
  }
}

void SGameManager::tryAddManaBall(std::shared_ptr<SNode> tile) {
  if (m_numManaBallRemaining > 0 and !tile->bHasGeyser() and
      !tile->bHasMana() and m_units[tile].empty() and
      m_buildings.count(tile) == 0 and bCoinToss(m_manaBallChance)) {
    tile->addMana(m_manaBallValue(m_gen));
    m_numManaBallRemaining--;
  }
}

bool SGameManager::bCanConstructBuilding(std::shared_ptr<SNode> tile,
                                         std::string buildingId,
                                         SPlayer &player) {
  return tile->bHasFoundation() and m_buildings.count(tile) == 0 and
         player.hasResources(m_buildingLookUpTable[buildingId].m_resourceCost);
}

std::unordered_set<std::string>
SGameManager::getConstructableBuidlings(std::shared_ptr<SNode> tile,
                                        SPlayer &player) {
  std::unordered_set<std::string> cb;
  for (auto &p : m_buildingLookUpTable) {
    auto bId = p.first;
    if (bCanConstructBuilding(tile, bId, player)) {
      cb.insert(bId);
    }
  }
  return cb;
}

std::shared_ptr<SBuilding> SGameManager::constructBuidlingForPlayer(
    std::shared_ptr<SNode> tile, BUILDING_ID building, SPlayer &player) {
  if (!bCanConstructBuilding(tile, building, player)) {
    return nullptr;
  }
  auto newBuilding = spawnBuidlingForPlayer(tile, building, player);
  newBuilding->resetConstruction();
  player.removeResources(newBuilding->m_resourceCost);
  return newBuilding;
}

std::shared_ptr<SBuilding>
SGameManager::spawnBuidlingForPlayer(std::shared_ptr<SNode> tile,
                                     BUILDING_ID building, SPlayer &player) {
  if (m_buildingLookUpTable.count(building) == 0) {
    return nullptr;
  }
  auto newBuilding =
      std::make_shared<SBuilding>(m_buildingLookUpTable[building]);
  newBuilding->setOwner(player.getPlayerId());
  m_buildings[tile] = newBuilding;
  newBuilding->finishConstruction();
  return newBuilding;
}

std::unordered_set<std::shared_ptr<SUnit>>
SGameManager::spawnUnitsForPlayer(std::shared_ptr<SNode> tile, UNIT_ID unit,
                                  SPlayer &player, int num) {
  if (m_unitLookUpTable.count(unit) == 0) {
    return {};
  }
  std::unordered_set<std::shared_ptr<SUnit>> nus;
  for (int i = 0; i < num; i++) {
    auto newUnit = std::make_shared<SUnit>(m_unitLookUpTable[unit]);
    newUnit->setOwner(player.getPlayerId());
    m_units[tile].insert(newUnit);
    newUnit->setCurrentTile(tile);
    nus.insert(newUnit);
  }
  return nus;
}

bool SGameManager::bCanTrainUnit(std::shared_ptr<SBuilding> building,
                                 std::string unit) {
  return !building->bUnderConstruction() and building->bCanTrainUnit(unit) and
         m_players[building->getOwner()]->hasResources(
             m_unitLookUpTable[unit].m_resourceCost);
}

std::unordered_set<std::string>
SGameManager::getBuildableUnits(std::shared_ptr<SBuilding> building) {
  std::unordered_set<std::string> bb;
  for (const auto &tu : building->m_buildableUnits) {
    if (bCanTrainUnit(building, tu)) {
      bb.insert(tu);
    }
  }
  return bb;
}
