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

SGameManager::SGameManager() {
  m_tiles = std::make_shared<SNodeGraph>(10, 10, false, false);
  m_camera = std::make_shared<SCamera>();

  m_camera->pos = {0, 0};
  m_camera->zoomRate = 3.0f;
  m_camera->cameraSpeed = 500.0f;
  m_camera->defaultZoom = 2.0f;
  m_camera->currentZoom = m_camera->defaultZoom;
  m_camera->viewportWidth = 1280;
  m_camera->viewportHeight = 720;

  m_renderer.setRenderCamera(m_camera);
  //  m_renderer.setRenderTiles(m_tiles);

  m_aspectRatio = float(m_camera->viewportWidth) / m_camera->viewportHeight;
  m_realVirtualRatio = m_camera->viewportHeight / 1000.0f;
  m_maxFPS = 120;
  m_maxFrameTime = 1.0f / m_maxFPS;
}

SGameManager::~SGameManager() {}

void SGameManager::run() {
  bool bQuit = false;
  SDL_Event e;

  auto time = std::chrono::steady_clock::now();

  while (!bQuit) {

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

    while (SDL_PollEvent(&e) != 0) {
      //      std::cout << "Got event" << std::endl;
      if (e.type == SDL_QUIT) {
        bQuit = true;
      } else if (e.type == SDL_KEYDOWN) {
        auto ks = e.key.keysym.scancode;
        switch (ks) {
        case SDL_SCANCODE_W:
          m_cameraMovementInput.y -= 1;
          break;
        case SDL_SCANCODE_S:
          m_cameraMovementInput.y += 1;
          break;
        case SDL_SCANCODE_A:
          m_cameraMovementInput.x -= 1;
          break;
        case SDL_SCANCODE_D:
          m_cameraMovementInput.x += 1;
          break;
        }
      } else if (e.type == SDL_MOUSEWHEEL) {
        m_cameraZoomInput = e.wheel.y;
      } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        if (glm::length(glm::vec2{1000 * m_aspectRatio - x / m_realVirtualRatio,
                                  1000 - y / m_realVirtualRatio}) <= 150) {
          if (e.button.button == SDL_BUTTON_LEFT) {
            endTurn();
          }
        }

        auto t = getClickedTile(x, y);
        if (e.button.button == SDL_BUTTON_LEFT) {
          if (t != nullptr and t->getTileUnits().size() > 0) {
            if (t == m_selectedTile) {
              m_selectedTile = nullptr;
            } else {
              m_selectedTile = t;
            }
          }
        } else if (e.button.button == SDL_BUTTON_RIGHT) {
          if (t != m_selectedTile and m_selectedTile != nullptr and
              t != nullptr) {
            auto path = m_tiles->shortestPath(m_selectedTile, t);
            m_selectedTile = nullptr;
          }
        }
      }
    }

    updateCamera();

    for (auto &t : m_tiles->getTiles()) {
      auto s = t->getSprite();
      int x, y;
      std::tie(x, y) = t->getPos();
      x *= s->m_size;
      y *= s->m_size;
      m_renderer.submitRenderRequest(t->getSprite(), x, y, s->m_renderPriority,
                                     RenderLocation::RENDER_CENTER);
    }

    m_renderer.render();
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
  float realVirtualRatio = m_camera->viewportHeight / 1000.0f;
  x /= realVirtualRatio;
  y /= realVirtualRatio;
  auto dr =
      m_camera->pos -
      glm::vec2{500 * m_camera->viewportWidth / m_camera->viewportHeight, 500} *
          m_camera->currentZoom;
  dr += glm::vec2{x, y} * m_camera->currentZoom;
  dr += glm::vec2{30, 30};

  return m_tiles->getTileAt(dr.x / 60, dr.y / 60);
}

void SGameManager::endTurn() {
  std::cout << "Ending turn (doing nothing)" << std::endl;
}
