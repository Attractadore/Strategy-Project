#pragma once

#include "SCamera.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class SNodeGraph;

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Texture;
struct SDL_Rect;

class SRenderer {
public:
  SRenderer(std::shared_ptr<SNodeGraph> tiles);
  ~SRenderer();

  void render();

  void addCameraMovementInput(glm::vec2 inp, float deltaTime);
  void addCameraZoom(float val, float deltaTime);

private:
  void loadTexture(std::string path);

  inline void renderCenter(SDL_Texture *texture, SDL_Rect *srcrect,
                           SDL_Rect dstrect);

  SCamera renderCamera{};
  SDL_Renderer *renderer = nullptr;
  SDL_Window *window = nullptr;

  std::shared_ptr<SNodeGraph> tiles;

  std::unordered_map<std::string, SDL_Texture *> textures;
};
