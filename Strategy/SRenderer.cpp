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
#include <SDL2/SDL_ttf.h>

#include <iostream>

RenderRequest::RenderRequest(std::shared_ptr<SSprite> p_sprite, int p_x,
                             int p_y, int p_frameIndex,
                             RenderLocation p_renderLocation, bool p_bWorld, SDL_Color p_modulationColor) {
  m_sprite = p_sprite;
  m_x = p_x;
  m_y = p_y;
  m_frameIndex = p_frameIndex;
  m_bWorld = p_bWorld;
  m_renderLocation = p_renderLocation;
  m_modulationColor = p_modulationColor;
}

RenderRequest::RenderRequest(const RenderRequest &other) {
  m_sprite = other.m_sprite;
  m_x = other.m_x;
  m_y = other.m_y;
  m_frameIndex = other.m_frameIndex;
  m_renderLocation = other.m_renderLocation;
  m_bWorld = other.m_bWorld;
  m_modulationColor = other.m_modulationColor;
}

RenderRequest::~RenderRequest() {}

RenderRequest &RenderRequest::operator=(const RenderRequest &other) {
  m_sprite = other.m_sprite;
  m_x = other.m_x;
  m_y = other.m_y;
  m_frameIndex = other.m_frameIndex;
  m_renderLocation = other.m_renderLocation;
  m_bWorld = other.m_bWorld;
  return *this;
}

TextRenderRequest::TextRenderRequest(std::string p_text, int p_x, int p_y,
                                     int p_h, int p_renderPriority,
                                     RenderLocation p_renderLocation) {
  m_text = p_text;
  m_x = p_x;
  m_y = p_y;
  m_h = p_h;
  m_renderPriority = p_renderPriority;
  m_renderLocation = p_renderLocation;
}

bool operator==(const RenderRequest &lhs, const RenderRequest &rhs) {
  return lhs.m_sprite->m_renderPriority == rhs.m_sprite->m_renderPriority;
}

bool operator<(const RenderRequest &lhs, const RenderRequest &rhs) {
  // Sign swap so that lower priority sprites appear first in priority queue
  return lhs.m_sprite->m_renderPriority > rhs.m_sprite->m_renderPriority;
}

bool operator==(const TextRenderRequest &lhs, const TextRenderRequest &rhs) {
  return lhs.m_renderPriority == rhs.m_renderPriority;
}

bool operator<(const TextRenderRequest &lhs, const TextRenderRequest &rhs) {
  return lhs.m_renderPriority > rhs.m_renderPriority;
}

SRenderer::SRenderer() {

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    throw std::runtime_error("Failed to init SDL");
  }

  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    throw std::runtime_error("Failed to init SDL_image");
  }

  if (TTF_Init() < 0) {
    throw std::runtime_error("Failed to init SDL_ttf");
  }

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);
  m_screenWidth = dm.w * (3.f / 4.f);
  m_screenHeight = dm.h * (3.f / 4.f);
  m_screenRatio = float(m_screenWidth) / m_screenHeight;
  m_realVirtualRatio = m_screenHeight / 1000.0f;

  window =
      SDL_CreateWindow("Strategy", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, m_screenWidth, m_screenHeight,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_GRABBED);
  if (window == nullptr) {
    throw std::runtime_error("Failed to create window SDL error " +
                             std::string(SDL_GetError()));
  }

  m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_SetRenderDrawColor(m_renderer, 0x00, 0x80, 0x80, 0x00);

  std::string font = "./assets/fonts/OpenSans-Bold.ttf";
  m_textFont = TTF_OpenFont(font.c_str(), 48);
  if (m_textFont == nullptr) {
    throw std::runtime_error("Failed to open font " + font + " SDL_ttf error " +
                             TTF_GetError());
  }
  m_textColor = {0xE0, 0xE0, 0xE0, 0x00};

  m_renderThread = std::thread(&SRenderer::checkMTSupport, this);
}

SRenderer::~SRenderer() {
  m_renderThread.join();

  for (auto &t : this->textures) {
    SDL_DestroyTexture(t.second);
  }

  TTF_CloseFont(m_textFont);

  SDL_DestroyRenderer(m_renderer);
  SDL_DestroyWindow(this->window);

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

void SRenderer::render() {
  if (m_bUseRenderThread) {
    m_renderThread.join();
  }
  m_tmpQueue = std::move(m_drawQueue);
  m_tmpTextDrawQueue = std::move(m_textDrawQueue);
  m_tmpCameraPos = m_camera->pos;
  m_tmpCameraZoom = m_camera->currentZoom;
  m_drawQueue = {};
  m_textDrawQueue = {};
  if (m_bUseRenderThread) {
    m_renderThread = std::thread(&SRenderer::renderThread, this);
  } else {
    renderThread();
  }
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
    if (rr.m_bWorld) {
      dstRect.w = dstRect.h = int(
          rr.m_sprite->m_tileSize / m_tmpCameraZoom * m_realVirtualRatio + 2);
      dstRect.x =
          int((rr.m_x / m_tmpCameraZoom + drawOffset.x) * m_realVirtualRatio);
      dstRect.y =
          int((rr.m_y / m_tmpCameraZoom + drawOffset.y) * m_realVirtualRatio);
    } else {
      dstRect.w = dstRect.h = rr.m_sprite->m_tileSize * m_realVirtualRatio + 2;
      dstRect.x = rr.m_x * m_realVirtualRatio;
      dstRect.y = rr.m_y * m_realVirtualRatio;
    }

    if (textures.count(rr.m_sprite->m_texturePath) == 0) {
      loadTexture(rr.m_sprite->m_texturePath);
    }

    if (rr.m_modulationColor.r != 0xFF and
        rr.m_modulationColor.g != 0xFF and
        rr.m_modulationColor.b != 0xFF and
        rr.m_modulationColor.a != 0x00){
        SDL_SetTextureColorMod(textures[rr.m_sprite->m_texturePath], rr.m_modulationColor.r, rr.m_modulationColor.g, rr.m_modulationColor.b);
    }

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

  while (!m_tmpTextDrawQueue.empty()) {
    auto trr = m_tmpTextDrawQueue.top();
    m_tmpTextDrawQueue.pop();
    if (textures.count(trr.m_text) == 0 or
        m_textAspectRatios.count(trr.m_text) == 0) {
      renderText(trr.m_text);
    }
    trr.m_w = int(trr.m_h * m_textAspectRatios[trr.m_text]);
    switch (trr.m_renderLocation) {
    case RenderLocation::RENDER_CENTER:
      trr.m_x -= trr.m_w / 2;
      trr.m_y -= trr.m_h / 2;
      break;
    case RenderLocation::RENDER_TOP_LEFT:
      break;
    case RenderLocation::RENDER_TOP_RIGHT:
      trr.m_x -= trr.m_w;
      break;
    case RenderLocation::RENDER_BOTTOM_LEFT:
      trr.m_y -= trr.m_h;
      break;
    case RenderLocation::RENDER_BOTTOM_RIGHT:
      trr.m_x -= trr.m_w;
      trr.m_y -= trr.m_h;
      break;
    }
    dstRect.w = int(trr.m_w * m_realVirtualRatio);
    dstRect.h = int(trr.m_h * m_realVirtualRatio);
    dstRect.x = int(trr.m_x * m_realVirtualRatio);
    dstRect.y = int(trr.m_y * m_realVirtualRatio);
    SDL_RenderCopy(m_renderer, textures[trr.m_text], nullptr, &dstRect);
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

void SRenderer::renderText(std::string text) {
  if (text == "") {
    return;
  }
  SDL_Surface *tmpSurface =
      TTF_RenderText_Blended(m_textFont, text.c_str(), m_textColor);
  if (tmpSurface == nullptr) {
    throw std::runtime_error(
        std::string("Failed to render text SDL_ttf error ") + TTF_GetError());
  }
  m_textAspectRatios[text] = float(tmpSurface->w) / tmpSurface->h;
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(m_renderer, tmpSurface);
  if (textTexture == nullptr) {
    throw std::runtime_error("Failed to create texture SDL error " +
                             std::string(SDL_GetError()));
  }
  SDL_FreeSurface(tmpSurface);
  this->textures[text] = textTexture;
}

void SRenderer::submitTextRenderRequest(std::string text, int x, int y, int h,
                                        int renderPriority,
                                        RenderLocation renderLocation) {
  m_textDrawQueue.push(
      TextRenderRequest(text, x, y, h, renderPriority, renderLocation));
}

void SRenderer::setRenderCamera(std::shared_ptr<SCamera> p_camera) {
  m_camera = p_camera;
  m_camera->viewportWidth = m_screenWidth;
  m_camera->viewportHeight = m_screenHeight;
}

void SRenderer::submitRenderRequest(std::shared_ptr<SSprite> p_sprite, int x,
                                    int y, int frameIndex,
                                    RenderLocation renderLocation,
                                    bool bWorld, SDL_Color modulationColor) {
  m_drawQueue.push(
      RenderRequest{p_sprite, x, y, frameIndex, renderLocation, bWorld, modulationColor});
}

void SRenderer::checkMTSupport() {
  SDL_ClearError();
  SDL_RenderClear(m_renderer);
  SDL_RenderPresent(m_renderer);
  if (std::string(SDL_GetError()).size() > 0) {
    std::cout << "Render in worker thread failed, disabling it" << std::endl;
    m_bUseRenderThread = false;
  } else {
    std::cout << "Render in worker thread successful, keeping it" << std::endl;
  }
}
