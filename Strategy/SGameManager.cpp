#include "SGameManager.hpp"

#include "SCamera.hpp"
#include "SNode.hpp"
#include "SNodeGraph.hpp"
#include "SSprite.hpp"
#include "SUnit.hpp"

#include <glm/glm.hpp>

#include <SDL2/SDL.h>

#include <chrono>
#include <iostream>
#include <thread>

#include <cmath>

SGameManager::SGameManager() {
  m_worldWidth = 10;
  m_worldHeight = 10;
  m_tiles =
      std::make_shared<SNodeGraph>(m_worldWidth, m_worldHeight, false, false);
  m_camera = std::make_shared<SCamera>();

  m_camera->pos = {0, 0};
  m_camera->zoomRate = 3.0f;
  m_camera->cameraSpeed = 500.0f;
  m_camera->defaultZoom = 2.0f;
  m_camera->currentZoom = m_camera->defaultZoom;
  m_camera->viewportWidth = 1280;
  m_camera->viewportHeight = 720;

  m_renderer.setRenderCamera(m_camera);

  m_aspectRatio = float(m_camera->viewportWidth) / m_camera->viewportHeight;
  m_realVirtualRatio = m_camera->viewportHeight / 1000.0f;
  m_maxFPS = 120;
  m_maxFrameTime = 1.0f / m_maxFPS;

  std::shared_ptr<SSprite> defaultUnitSprite =
      std::make_shared<SSprite>("./assets/spearman.png", 1, 10, 3);
  m_selectionSprite =
      std::make_shared<SSprite>("./assets/selection.png", 1, 60, 4);

  SUnit spearman{};
  spearman.setSprite(defaultUnitSprite);
  spearman.setMaxHP(10);
  spearman.setDamage(1);
  spearman.setAccuracy(1);
  spearman.setMaxMoves(1);

  m_gen.seed(std::random_device()());

  std::uniform_int_distribution<> distX(0, m_worldWidth - 1);
  std::uniform_int_distribution<> distY(0, m_worldHeight - 1);

  for (int i = 0; i < m_worldWidth * 2; i++) {
    //  for (int i = 0; i < 1; i++) {
    int x = distX(m_gen);
    int y = distY(m_gen);
    auto targetTile = m_tiles->getTileAt(x, y);
    m_units[targetTile].push_back(std::make_shared<SUnit>(spearman));
  }

  m_bQuit = false;
}

SGameManager::~SGameManager() {}

void SGameManager::run() {
  auto time = std::chrono::steady_clock::now();

  while (!m_bQuit) {

    auto newTime = std::chrono::steady_clock::now();
    m_deltaTime =
        std::chrono::duration_cast<std::chrono::microseconds>(newTime - time)
            .count() /
        1000000.0f;

    time = newTime;

    if (m_maxFPS != 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(std::max(
          0, static_cast<int>(1000000 * (m_maxFrameTime - m_deltaTime)))));
    }

    handleInput();

    updateCamera();

    for (auto &tile : m_tiles->getTiles()) {
      auto sprite = tile->getSprite();
      float x, y;
      std::tie(x, y) = tile->getPos();
      x *= sprite->m_size;
      y *= sprite->m_size;
      m_renderer.submitRenderRequest(tile->getSprite(), x, y, 0,
                                     RenderLocation::RENDER_CENTER);
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

      auto unitVec = p.second;
      int numUnits = unitVec.size();
      if (numUnits == 0) {
        continue;
      }
      float dx, dy;
      dx = dy = -(numUnits - 1) / 2 * unitVec[0]->getSprite()->m_tileSize;
      for (auto &unit : unitVec) {
        auto unitSprite = unit->getSprite();
        m_renderer.submitRenderRequest(unitSprite, x + dx, y + dy, 0,
                                       RenderLocation::RENDER_CENTER);
        dx += unitSprite->m_tileSize;
        dy += unitSprite->m_tileSize;
      }
    }

    m_renderer.render();
  }
}

void SGameManager::handleInput() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    //      std::cout << "Got event" << std::endl;
    if (e.type == SDL_QUIT) {
      m_bQuit = true;
    }

    else if (e.type == SDL_KEYDOWN) {
      auto ks = e.key.keysym.scancode;
      if (ks == SDL_SCANCODE_W) {
        m_cameraMovementInput.y -= 1;
      } else if (ks == SDL_SCANCODE_S) {
        m_cameraMovementInput.y += 1;
      } else if (ks == SDL_SCANCODE_A) {
        m_cameraMovementInput.x -= 1;
      } else if (ks == SDL_SCANCODE_D) {
        m_cameraMovementInput.x += 1;
      }
    }

    else if (e.type == SDL_MOUSEWHEEL) {
      m_cameraZoomInput = e.wheel.y;
    }

    else if (e.type == SDL_MOUSEBUTTONDOWN) {
      int clickX, clickY;
      SDL_GetMouseState(&clickX, &clickY);
      int x, y;
      x = int(clickX / m_realVirtualRatio);
      y = int(clickY / m_realVirtualRatio);

      if (e.button.button == SDL_BUTTON_LEFT) {
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

        auto unitPath = m_tiles->shortestPath(m_selectedTile, clickedTile);

        // Do right click logic
      }
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
  std::cout << "Ending turn (doing nothing)" << std::endl;
}
