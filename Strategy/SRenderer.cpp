#include "SRenderer.hpp"

#include "SBuilding.hpp"
#include "SCamera.hpp"
#include "SNode.hpp"
#include "SNodeGraph.hpp"
#include "SSprite.hpp"
#include "SUnit.hpp"

#include <glm/glm.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

RenderRequest::RenderRequest(std::shared_ptr<SSprite> p_sprite, int p_x,
                             int p_y, int p_frameIndex,
                             RenderLocation p_renderLocation) {
  m_sprite = p_sprite;
  m_x = p_x;
  m_y = p_y;
  m_frameIndex = p_frameIndex;
  m_renderLocation = p_renderLocation;
}

RenderRequest::RenderRequest(const RenderRequest &other) {
  m_sprite = other.m_sprite;
  m_x = other.m_x;
  m_y = other.m_y;
  m_frameIndex = other.m_frameIndex;
  m_renderLocation = other.m_renderLocation;
}

RenderRequest::~RenderRequest() {}

RenderRequest &RenderRequest::operator=(const RenderRequest &other) {
  m_sprite = other.m_sprite;
  m_x = other.m_x;
  m_y = other.m_y;
  m_frameIndex = other.m_frameIndex;
  m_renderLocation = other.m_renderLocation;
  return *this;
}

bool operator==(const RenderRequest &lhs, const RenderRequest &rhs) {
  return lhs.m_sprite->m_renderPriority == rhs.m_sprite->m_renderPriority;
}

bool operator<(const RenderRequest &lhs, const RenderRequest &rhs) {
  // Sign swap so that lower priority sprites appear first in priority queue
  return lhs.m_sprite->m_renderPriority > rhs.m_sprite->m_renderPriority;
}

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
  m_screenRatio = float(m_screenWidth) / m_screenHeight;
  m_realVirtualRatio = m_screenHeight / 1000.0f;

  window = SDL_CreateWindow("Strategy", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, m_screenWidth,
                            m_screenHeight, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    throw std::runtime_error("Failed to create window SDL error " +
                             std::string(SDL_GetError()));
  }

  m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_SetRenderDrawColor(m_renderer, 0x00, 0x80, 0x80, 0x00);

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

  m_bFirstTime = true;
}

SRenderer::~SRenderer() {

  for (auto &t : this->textures) {
    SDL_DestroyTexture(t.second);
  }

  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(this->window);

  IMG_Quit();
  SDL_Quit();
}

void SRenderer::render() {
  if (!m_bFirstTime) {
    m_renderThread.join();
  } else {
    m_bFirstTime = false;
  }
  m_tmpQueue = std::move(m_drawQueue);
  m_tmpCameraPos = m_camera->pos;
  m_tmpCameraZoom = m_camera->currentZoom;
  m_drawQueue = {};
  m_renderThread = std::thread(&SRenderer::renderThread, this);
}

void SRenderer::renderThread() {
  SDL_RenderClear(m_renderer);

  SDL_Rect dstRect;
  SDL_Rect srcRect;

  glm::vec2 drawOffset =
      glm::vec2(500 * m_screenRatio, 500) - m_tmpCameraPos / m_tmpCameraZoom;

  while (!m_tmpQueue.empty()) {
    auto rr = m_tmpQueue.top();
    m_tmpQueue.pop();
    switch (rr.m_renderLocation) {
    case RenderLocation::RENDER_CENTER:
      rr.m_x -= rr.m_sprite->m_size / 2;
      rr.m_y -= rr.m_sprite->m_size / 2;
      break;
    case RenderLocation::RENDER_TOP_LEFT:
      break;
    case RenderLocation::RENDER_TOP_RIGHT:
      rr.m_x -= rr.m_sprite->m_size;
      break;
    case RenderLocation::RENDER_BOTTOM_LEFT:
      rr.m_y -= rr.m_sprite->m_size;
      break;
    case RenderLocation::RENDER_BOTTOM_RIGHT:
      rr.m_x -= rr.m_sprite->m_size;
      rr.m_y -= rr.m_sprite->m_size;
      break;
    }
    dstRect.w = dstRect.h =
        int(rr.m_sprite->m_tileSize / m_tmpCameraZoom * m_realVirtualRatio + 2);
    dstRect.x =
        int((rr.m_x / m_tmpCameraZoom + drawOffset.x) * m_realVirtualRatio);
    dstRect.y =
        int((rr.m_y / m_tmpCameraZoom + drawOffset.y) * m_realVirtualRatio);
    if (rr.m_sprite->m_numTiles == 1) {
      SDL_RenderCopy(m_renderer, textures[rr.m_sprite->m_texturePath], nullptr,
                     &dstRect);
    } else {
      srcRect.w = srcRect.h = rr.m_sprite->m_tileSize;
      srcRect.x = srcRect.w * (rr.m_frameIndex % rr.m_sprite->m_numTilesAxis);
      srcRect.y = srcRect.h * (rr.m_frameIndex / rr.m_sprite->m_numTilesAxis);
      SDL_RenderCopy(m_renderer, textures[rr.m_sprite->m_texturePath], &srcRect,
                     &dstRect);
    }
  }
  SDL_RenderPresent(m_renderer);
}

void SRenderer::loadTexture(std::string path) {
  SDL_Surface *tmpSurf = IMG_Load(path.c_str());
  if (tmpSurf == nullptr) {
    throw std::runtime_error("Failed to load texture from " + path +
                             " SDL_image error " + IMG_GetError());
  }
  SDL_Texture *tex = SDL_CreateTextureFromSurface(m_renderer, tmpSurf);
  if (tex == nullptr) {
    throw std::runtime_error("Failed to create texture SDL error " +
                             std::string(SDL_GetError()));
  }
  SDL_FreeSurface(tmpSurf);
  this->textures[path] = tex;
}

void SRenderer::setRenderCamera(std::shared_ptr<SCamera> p_camera) {
  m_camera = p_camera;
  m_camera->viewportWidth = m_screenWidth;
  m_camera->viewportHeight = m_screenHeight;
}

void SRenderer::submitRenderRequest(std::shared_ptr<SSprite> p_sprite, int x,
                                    int y, int frameIndex,
                                    RenderLocation renderLocation) {
  m_drawQueue.push(RenderRequest{p_sprite, x, y, frameIndex, renderLocation});
}
