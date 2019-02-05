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

  m_worldWidth = 10;
  m_worldHeight = 10;
  m_tiles = std::make_shared<SNodeGraph>(m_worldWidth, m_worldHeight);
  m_camera = std::make_shared<SCamera>();

  m_camera->pos = {0, 0};
  m_camera->zoomRate = 3.0f;
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

  SUnit spearman{"UNIT_SPEARMAN"};
  std::unordered_map<std::string, float> spearmanStats = {
      {"maxHP", 10},        {"damage", 1},     {"accuracy", 1},
      {"maxMoves", 2},      {"supplyCost", 1}, {"buildTime", 3},
      {"resourceCost", 50}, {"size", 4}};
  spearman.setSprite(defaultUnitSprite);
  spearman.setParams(spearmanStats);
  m_unitLookUpTable["UNIT_SPEARMAN"] = spearman;

  SBuilding productionBuilding;
  productionBuilding.setUnitLookUpTable(m_unitLookUpTable);
  auto productionBuildingSprite =
      std::make_shared<SSprite>("./assets/art/building.png", 1, 60, 2);
  productionBuilding.setSprite(productionBuildingSprite);
  productionBuilding.setParams({{"resourceGatherRate", 0}});
  SBuilding resourceBuilding;
  resourceBuilding.setUnitLookUpTable(m_unitLookUpTable);
  resourceBuilding.setSprite(productionBuildingSprite);
  resourceBuilding.setParams({{"resourceGatherRate", 30}});

  m_gen.seed(std::random_device()());

  std::uniform_int_distribution<> distX(0, m_worldWidth - 1);
  std::uniform_int_distribution<> distY(0, m_worldHeight - 1);

  for (int i = 0; i < m_worldWidth * 2; i++) {
    int x = distX(m_gen);
    int y = distY(m_gen);
    auto spawnTile = m_tiles->getTileAt(x, y);
    auto newUnit = std::make_shared<SUnit>(spearman);
    newUnit->setOwner(defaultPlayer.getPlayerId());
    newUnit->setCurrentTile(spawnTile);
    m_units[spawnTile].insert(newUnit);
  }

  auto newBuilding = std::make_shared<SBuilding>(productionBuilding);
  newBuilding->setOwner(defaultPlayer.getPlayerId());
  m_buildings[m_tiles->getTileAt(0, 0)] = newBuilding;
  newBuilding = std::make_shared<SBuilding>(resourceBuilding);
  newBuilding->setOwner(defaultPlayer.getPlayerId());
  m_buildings[m_tiles->getTileAt(m_worldWidth - 1, m_worldHeight - 1)] =
      newBuilding;

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

  std::unordered_set<KeyboardKey> pressedKeys;
  std::unordered_set<KeyboardKey> releasedKeys;

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
      m_inputStruct.mouseWheelScroll = e.wheel.y;
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
    if (m_buildings.count(tile) != 0) {
      m_renderer.submitRenderRequest(m_buildings[tile]->getSprite(), x, y, 0,
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
    if (tile == m_selectedTile) {
      std::vector<std::shared_ptr<SUnit>> tileUnits(unitSet.begin(),
                                                    unitSet.end());
      std::copy(unitSet.begin(), unitSet.end(), tileUnits.begin());
      std::sort(tileUnits.rbegin(), tileUnits.rend(), compareUnits);
      int dx = 5;
      int dy = -5;
      for (auto &unit : tileUnits) {
        m_renderer.submitRenderRequest(
            unit->getSprite(), dx, m_virtualHeight + dy, 0,
            RenderLocation::RENDER_BOTTOM_LEFT, false);
        if (m_selectedUnits.count(unit) > 0) {
          m_renderer.submitRenderRequest(
              m_buttonSelectionSprite, dx, m_virtualHeight + dy, 0,
              RenderLocation::RENDER_BOTTOM_LEFT, false);
        }
        dx += unit->getSprite()->m_size + 4;
      }
    }
  }

  m_renderer.submitRenderRequest(m_endTurnButtonSprite, m_virtualWidth,
                                 m_virtualHeight, 0,
                                 RenderLocation::RENDER_BOTTOM_RIGHT, false);

  m_renderer.render();
}

void SGameManager::performUnitMovement(
    std::unordered_set<std::shared_ptr<SUnit>> units) {
  for (auto &unit : units) {
    //    std::cointut << "Performing unit movement" << std::endl;
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
      int x, y;
      std::tie(x, y) = currentTile->getPos();
      std::cout << "Currently at " << x << " " << y << std::endl;
      std::tie(x, y) = nextTile->getPos();
      std::cout << "Moving to " << x << " " << y << std::endl;
      std::cout << "Units at next tile: " << m_units[nextTile].size()
                << std::endl;
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
        1 - m_camera->zoomRate * m_deltaTime, m_inputStruct.mouseWheelScroll);
  }
}

std::shared_ptr<SNode> SGameManager::getClickedTile(int x, int y) {
  std::cout << "Clicked at " << x << " " << y << std::endl;
  auto dr = m_camera->pos - glm::vec2{m_virtualWidth, m_virtualHeight} / 2.0f *
                                m_camera->currentZoom;
  dr += glm::vec2{x, y} * m_camera->currentZoom;
  dr += glm::vec2{30, 30};

  return m_tiles->getTileAt(std::floor(dr.x / 60), std::floor(dr.y / 60));
}

void SGameManager::endTurn() {
  m_selectedTile = nullptr;
  performUnitMovement(m_movingUnits);
  for (auto &p : m_units) {
    auto unitVec = p.second;
    for (auto &unit : unitVec) {
      unit->refresh();
    }
  }
  for (auto &p : m_buildings) {
    auto building = p.second;
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

  m_selectedTile = nullptr;
  m_selectedUnits.clear();
}

void SGameManager::handleLeftClick() {
  int x = m_inputStruct.mouseX;
  int y = m_inputStruct.mouseY;
  if (glm::length(glm::vec2{m_virtualWidth - x, m_virtualHeight - y}) <=
      m_endTurnButtonSprite->m_size) {
    endTurn();
    return;
  }
  if (m_selectedTile != nullptr and y <= m_virtualHeight - 5 and
      y >= m_virtualHeight - 25) {
    std::cout << "Clicking unit card " << std::endl;
    bool bClickedUnitIcon = (x - 5) % 24 <= 20;
    if (bClickedUnitIcon) {
      int index = (x - 5) / 24;
      std::cout << "Index = " << index << std::endl;
      auto unitSet = m_units[m_selectedTile];
      if (index < unitSet.size()) {
        std::vector<std::shared_ptr<SUnit>> tileUnits(unitSet.begin(),
                                                      unitSet.end());
        std::copy(unitSet.begin(), unitSet.end(), tileUnits.begin());
        std::sort(tileUnits.rbegin(), tileUnits.rend(), compareUnits);
        auto targetUnit = tileUnits[index];
        std::cout << "Num selected units " << m_selectedUnits.size()
                  << std::endl;
        bool bTargetUnitSelected = m_selectedUnits.count(targetUnit) > 0;
        std::cout << "Unit already selected " << std::boolalpha
                  << bTargetUnitSelected << std::endl;
        if (m_inputStruct.bShiftDown()) {
          if (bTargetUnitSelected) {
            m_selectedUnits.erase(targetUnit);
          } else {
            m_selectedUnits.insert(targetUnit);
          }
        } else {
          if (bTargetUnitSelected) {
            m_selectedUnits.erase(targetUnit);
          } else {
            m_selectedUnits = {targetUnit};
          }
        }
        std::cout << "Num selected units " << m_selectedUnits.size()
                  << std::endl;
        if (m_selectedUnits.empty()) {
          m_selectedTile = nullptr;
        }
        return;
      }
    }
  }

  auto clickedTile = getClickedTile(x, y);
  if (clickedTile == nullptr or m_units[clickedTile].size() == 0) {
    m_selectedTile = nullptr;
    return;
  }

  if (clickedTile == m_selectedTile) {
    m_selectedTile = nullptr;
  } else {
    m_selectedTile = clickedTile;
    m_selectedUnits = {strongestUnit(m_selectedTile)};
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
