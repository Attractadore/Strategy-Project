#include "SRenderer.hpp"

#include "SBuilding.hpp"
#include "SNode.hpp"
#include "SNodeGraph.hpp"
#include "SUnit.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

SRenderer::SRenderer(std::shared_ptr<SNodeGraph> tiles) {

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    throw std::runtime_error("Failed to init SDL");
  }

  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
    throw std::runtime_error("Failed to init SDL_image");
  }

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);
  const int screenWidth = dm.w * (2.0f / 3.0f);
  const int screenHeight = dm.h * (2.0f / 3.0f);
  //  const int screenWidth = 2100;
  //  const int screenHeight = 900;

  window = SDL_CreateWindow("Strategy", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight,
                            SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    throw std::runtime_error("Failed to create window SDL error " +
                             std::string(SDL_GetError()));
  }

  this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_SetRenderDrawColor(this->renderer, 0x00, 0x80, 0x80, 0x00);

  std::string nodeTexturePath = "./assets/tile.png";

  this->loadTexture(nodeTexturePath);

  SNode::setNodeTexture(this->textures[nodeTexturePath]);

  std::string buildingTexturePath = "./assets/building.png";

  this->loadTexture(buildingTexturePath);

  SBuilding::setBuildingTexture(this->textures[buildingTexturePath]);

  std::string unitTexturePath = "./assets/spearman.png";

  this->loadTexture(unitTexturePath);

  this->tiles = tiles;

  this->renderCamera.viewportWidth = screenWidth;
  this->renderCamera.viewportHeight = screenHeight;
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
  auto dts = this->tiles->getTiles();

  float virtualRatio = static_cast<float>(this->renderCamera.viewportWidth) /
                       this->renderCamera.viewportHeight;
  float virtualHeight = 1000.0f;

  float realVirtualRatio = this->renderCamera.viewportHeight / virtualHeight;

  float tileSize = 60.0f;
  float unitSize = 10.0f;

  auto camPos = this->renderCamera.pos;
  glm::vec2 drawOffset =
      -camPos / this->renderCamera.currentZoom +
      glm::vec2(virtualHeight * virtualRatio / 2, virtualHeight / 2);

  int x, y;
  SDL_Rect dstRect;
  dstRect.w = dstRect.h =
      tileSize / this->renderCamera.currentZoom * realVirtualRatio + 2;
  SDL_Texture *tex = SNode::getNodeTexture();
  SDL_Texture *bTex = SBuilding::getBuildingTexture();
  for (auto &dt : dts) {
    std::tie(x, y) = dt->getPos();
    dstRect.x = x * tileSize / this->renderCamera.currentZoom + drawOffset.x;
    dstRect.y = y * tileSize / this->renderCamera.currentZoom + drawOffset.y;

    dstRect.x *= realVirtualRatio;
    dstRect.y *= realVirtualRatio;

    //    SDL_RenderCopy(this->renderer, tex, nullptr, &dstRect);
    this->renderCenter(tex, nullptr, dstRect);

    if (dt->getTileBuilding() != nullptr) {
      //      SDL_RenderCopy(this->renderer, bTex, nullptr, &dstRect);
      this->renderCenter(bTex, nullptr, dstRect);
    }
    auto presUnits = dt->getTileUnits();
    int numUnits = presUnits.size();
  }

  SDL_RenderPresent(this->renderer);
}

void SRenderer::addCameraMovementInput(glm::vec2 inp, float deltaTime) {
  this->renderCamera.pos += inp * this->renderCamera.cameraSpeed * deltaTime;
}

void SRenderer::addCameraZoom(float val, float deltaTime) {
  this->renderCamera.currentZoom *=
      std::pow(1 - this->renderCamera.zoomRate, val);
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
