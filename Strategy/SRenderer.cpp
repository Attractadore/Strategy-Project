#include "SRenderer.hpp"

#include "SBuilding.hpp"
#include "SCamera.hpp"
#include "SNode.hpp"
#include "SNodeGraph.hpp"
#include "SUnit.hpp"

#include <glm/glm.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

SRenderer::SRenderer() {

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    throw std::runtime_error("Failed to init SDL");
  }

  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    throw std::runtime_error("Failed to init SDL_image");
  }

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);
  std::cout << "Dm size " << dm.w << " " << dm.h << std::endl;
  m_screenWidth = dm.w * (3.f / 4.f);
  m_screenHeight = dm.h * (3.f / 4.f);
  //  m_screenWidth = 1600;
  //  m_screenHeight = 1000;
  //  const int screenWidth = 2100;
  //  const int screenHeight = 900;

  window = SDL_CreateWindow("Strategy", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, m_screenWidth,
                            m_screenHeight, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    throw std::runtime_error("Failed to create window SDL error " +
                             std::string(SDL_GetError()));
  }

  this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_SetRenderDrawColor(this->renderer, 0x00, 0x80, 0x80, 0x00);

  std::string nodeTexturePath = "./assets/tile.png";
  std::string buildingTexturePath = "./assets/building.png";
  std::string unitTexturePath = "./assets/spearman.png";
  std::string selectionTexturePath = "./assets/selection.png";
  std::string endTurnButtonTexturePath = "./assets/ui/endTurn.png";

  this->loadTexture(nodeTexturePath);
  this->loadTexture(buildingTexturePath);
  this->loadTexture(unitTexturePath);
  this->loadTexture(selectionTexturePath);
  this->loadTexture(endTurnButtonTexturePath);
}

SRenderer::~SRenderer() {

  for (auto &t : this->textures) {
    SDL_DestroyTexture(t.second);
  }

  SDL_DestroyRenderer(this->renderer);
  SDL_DestroyWindow(this->window);

  IMG_Quit();
  SDL_Quit();
}

void SRenderer::render() {
  SDL_RenderClear(this->renderer);
  auto dts = m_tiles->getTiles();

  float virtualRatio =
      static_cast<float>(m_camera->viewportWidth) / m_camera->viewportHeight;
  float virtualHeight = 1000.0f;

  float realVirtualRatio = m_camera->viewportHeight / virtualHeight;

  float tileSize = 60.0f;
  float unitSize = 20.0f;
  float selectionSize = 80.0f;
  SDL_Rect endTurnButtonPos;
  endTurnButtonPos.w = 150;
  endTurnButtonPos.h = 150;
  endTurnButtonPos.x = 1000 * virtualRatio - endTurnButtonPos.w;
  endTurnButtonPos.y = 1000 - endTurnButtonPos.h;

  auto camPos = m_camera->pos;
  glm::vec2 drawOffset =
      -camPos / m_camera->currentZoom +
      glm::vec2(virtualHeight * virtualRatio / 2, virtualHeight / 2);

  int x, y;
  SDL_Rect dstRect;
  for (auto &dt : dts) {
    std::tie(x, y) = dt->getPos();
    dstRect.w = dstRect.h =
        tileSize / m_camera->currentZoom * realVirtualRatio + 2;

    dstRect.x = x * tileSize / m_camera->currentZoom + drawOffset.x;
    dstRect.y = y * tileSize / m_camera->currentZoom + drawOffset.y;

    dstRect.x *= realVirtualRatio;
    dstRect.y *= realVirtualRatio;

    this->renderCenter(this->textures[dt->getTexturePath()], nullptr, dstRect);

    if (dt == m_selectedTile) {
      dstRect.w = dstRect.h =
          selectionSize / m_camera->currentZoom * realVirtualRatio;
      this->renderCenter(this->textures["./assets/selection.png"], nullptr,
                         dstRect);
    }

    if (dt->getTileBuilding() != nullptr) {
      dstRect.w = dstRect.h =
          tileSize / m_camera->currentZoom * realVirtualRatio;
      this->renderCenter(
          this->textures[dt->getTileBuilding()->getTexturePath()], nullptr,
          dstRect);
    }
    auto presUnits = dt->getTileUnits();
    int numUnits = presUnits.size();
    if (numUnits > 0) {
      float w = unitSize / 2 * (numUnits - 1) / m_camera->currentZoom *
                realVirtualRatio;
      float h = unitSize / 4 * (numUnits - 1) / m_camera->currentZoom *
                realVirtualRatio;
      dstRect.w = dstRect.h =
          unitSize / m_camera->currentZoom * realVirtualRatio;
      dstRect.x -= w / 2;
      dstRect.y -= h / 2;
      for (int i = 0; i < numUnits; i++) {
        this->renderCenter(this->textures[presUnits[i]->getTexturePath()],
                           nullptr, dstRect);
        if (numUnits > 1) {
          dstRect.x += w / (numUnits - 1);
          dstRect.y += h / (numUnits - 1);
        }
      }
    }
  }

  endTurnButtonPos.w *= realVirtualRatio;
  endTurnButtonPos.h *= realVirtualRatio;
  endTurnButtonPos.x *= realVirtualRatio;
  endTurnButtonPos.y *= realVirtualRatio;

  //  std::cout << "Render end turn button at " << endTurnButtonPos.x << " "
  //            << endTurnButtonPos.y << std::endl;

  SDL_RenderCopy(renderer, textures["./assets/ui/endTurn.png"], nullptr,
                 &endTurnButtonPos);

  SDL_RenderPresent(this->renderer);
}

void SRenderer::loadTexture(std::string path) {
  SDL_Surface *tmpSurf = IMG_Load(path.c_str());
  if (tmpSurf == nullptr) {
    throw std::runtime_error("Failed to load texture from " + path +
                             " SDL_image error " + IMG_GetError());
  }
  SDL_Texture *tex = SDL_CreateTextureFromSurface(this->renderer, tmpSurf);
  if (tex == nullptr) {
    throw std::runtime_error("Failed to create texture SDL error " +
                             std::string(SDL_GetError()));
  }
  SDL_FreeSurface(tmpSurf);
  this->textures[path] = tex;
}

inline void SRenderer::renderCenter(SDL_Texture *texture, SDL_Rect *srcrect,
                                    SDL_Rect dstrect) {
  dstrect.x -= dstrect.w / 2;
  dstrect.y -= dstrect.h / 2;
  SDL_RenderCopy(this->renderer, texture, srcrect, &dstrect);
}

void SRenderer::setRenderCamera(std::shared_ptr<SCamera> p_camera) {
  m_camera = p_camera;
  m_camera->viewportWidth = m_screenWidth;
  m_camera->viewportHeight = m_screenHeight;
}

void SRenderer::setRenderTiles(std::shared_ptr<SNodeGraph> p_renderTiles) {
  m_tiles = p_renderTiles;
}

void SRenderer::setSelectedTile(std::shared_ptr<SNode> p_tile) {
  m_selectedTile = p_tile;
}

void SRenderer::resetSelectedTile() { m_selectedTile = nullptr; }
