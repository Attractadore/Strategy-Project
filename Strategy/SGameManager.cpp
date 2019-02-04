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
#include <thread>

#include <cmath>

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
  m_realVirtualRatio = m_camera->viewportHeight / 1000.0f;
  m_maxFPS = 120;
  m_maxFrameTime = 1.0f / m_maxFPS;

  std::shared_ptr<SSprite> defaultUnitSprite =
      std::make_shared<SSprite>("./assets/art/spearman.png", 1, 10, 3);
  m_selectionSprite =
      std::make_shared<SSprite>("./assets/art/selection.png", 1, 60, 4);
  m_endTurnButtonSprite =
      std::make_shared<SSprite>("./assets/art/ui/endTurn.png", 1, 150, 5);

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

  //  SDL_SetRelativeMouseMode(SDL_TRUE);
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
  int clickX, clickY;
  SDL_GetMouseState(&clickX, &clickY);
  int x, y;
  x = int(clickX / m_realVirtualRatio);
  y = int(clickY / m_realVirtualRatio);
  //  std::cout << "Mouse at " << x << " " << y << std::endl;
  if (x <= 5) {
    m_cameraMovementInput.x -= 1;
  } else if (x >= (1000 * m_aspectRatio - 5)) {
    m_cameraMovementInput.x += 1;
  }
  if (y <= 5) {
    m_cameraMovementInput.y -= 1;
  } else if (y >= 995) {
    m_cameraMovementInput.y += 1;
  }

  while (SDL_PollEvent(&e) != 0) {
    //      std::cout << "Got event" << std::endl;
    if (e.type == SDL_QUIT) {
      m_bQuit = true;
    }

    else if (e.type == SDL_KEYDOWN) {
      auto ks = e.key.keysym.scancode;
      if (ks == SDL_SCANCODE_ESCAPE) {
        m_bQuit = true;
      } else if (ks == SDL_SCANCODE_RETURN) {
        endTurn();
      } else if (ks == SDL_SCANCODE_E) {
        auto buildUnit = m_unitLookUpTable["UNIT_SPEARMAN"];
        if (defaultPlayer.hasResources(buildUnit.m_resourceCost)) {

          std::cout << "Added unit to queue" << std::endl;
          defaultPlayer.removeResources(buildUnit.m_resourceCost);
          m_buildings[m_tiles->getTileAt(0, 0)]->addUnitToBuildQueue(
              "UNIT_SPEARMAN");
        }
      }
    }

    else if (e.type == SDL_MOUSEWHEEL) {
      m_cameraZoomInput = e.wheel.y;
    }

    else if (e.type == SDL_MOUSEBUTTONDOWN) {

      if (e.button.button == SDL_BUTTON_LEFT) {

        if (glm::length(glm::vec2{1000 * m_aspectRatio - x, 1000 - y}) <=
            m_endTurnButtonSprite->m_size) {
          endTurn();
          continue;
        }

        auto clickedTile = getClickedTile(x, y);
        if (clickedTile == nullptr or m_units[clickedTile].size() == 0) {
          continue;
        }

        if (clickedTile == m_selectedTile) {
          m_selectedTile = nullptr;
        } else {
          m_selectedTile = clickedTile;
        }
      }

      else if (e.button.button == SDL_BUTTON_RIGHT) {
        auto clickedTile = getClickedTile(x, y);
        if (m_selectedTile == nullptr or clickedTile == nullptr or
            clickedTile == m_selectedTile) {
          continue;
        }

        auto unitRoute = m_tiles->shortestPath(m_selectedTile, clickedTile);
        auto movingUnits = m_units[m_selectedTile];

        for (auto &unit : movingUnits) {
          unit->setTargetTile(unitRoute.back());
          m_movingUnits.insert(unit);
        }

        performeUnitMovement(movingUnits);

        m_selectedTile = nullptr;
      }
    }
  }
}

void SGameManager::handleLogic() { updateCamera(); }

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
    int numUnits = unitSet.size();
    if (numUnits == 0) {
      continue;
    }
    float dx, dy;
    dx = dy = -(numUnits - 1) * 5 / 8;
    for (auto &unit : unitSet) {
      auto unitSprite = unit->getSprite();
      m_renderer.submitRenderRequest(unitSprite, x + dx, y + dy, 0,
                                     RenderLocation::RENDER_CENTER);
      dx += unitSprite->m_tileSize / 4;
      dy += unitSprite->m_tileSize / 4;
    }
  }

  m_renderer.submitRenderRequest(m_endTurnButtonSprite, 1000.0f * m_aspectRatio,
                                 1000, 0, RenderLocation::RENDER_BOTTOM_RIGHT,
                                 false);

  m_renderer.render();
}

void SGameManager::performeUnitMovement(
    std::unordered_set<std::shared_ptr<SUnit>> units) {
  for (auto &unit : units) {
    //    std::cout << "Performing unit movement" << std::endl;
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
      m_units[currentTile].erase(unit);
      m_units[nextTile].insert(unit);
      unit->moveTile(nextTile);
    }
  }
}

void SGameManager::updateCamera() {
  if (glm::length(m_cameraMovementInput) != 0) {
    m_camera->pos +=
        m_cameraMovementInput * m_camera->cameraSpeed * m_deltaTime;
    m_cameraMovementInput = {0, 0};
  }
  if (m_cameraZoomInput != 0) {
    m_camera->currentZoom *= std::pow<float, float>(
        1 - m_camera->zoomRate * m_deltaTime, m_cameraZoomInput);
    m_cameraZoomInput = 0;
  }
}

std::shared_ptr<SNode> SGameManager::getClickedTile(int x, int y) {
  auto dr = m_camera->pos -
            glm::vec2{500 * m_aspectRatio, 500} * m_camera->currentZoom;
  dr += glm::vec2{x, y} * m_camera->currentZoom;
  dr += glm::vec2{30, 30};

  return m_tiles->getTileAt(std::floor(dr.x / 60), std::floor(dr.y / 60));
}

void SGameManager::endTurn() {
  performeUnitMovement(m_movingUnits);
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
