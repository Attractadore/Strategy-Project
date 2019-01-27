#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class SNodeGraph;
class SNode;
struct SCamera;

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Texture;
struct SDL_Rect;

class SRenderer {
public:
  SRenderer();
  SRenderer(const SRenderer &other) = delete;
  SRenderer(SRenderer &&other) = delete;
  ~SRenderer();

  SRenderer &operator=(const SRenderer &other) = delete;
  SRenderer &operator=(SRenderer &&other) = delete;

  void render();

  void setRenderCamera(std::shared_ptr<SCamera> p_camera);
  void setRenderTiles(std::shared_ptr<SNodeGraph> p_renderTiles);
  void setSelectedTile(std::shared_ptr<SNode> p_tile);
  void resetSelectedTile();

private:
  void loadTexture(std::string path);

  inline void renderCenter(SDL_Texture *texture, SDL_Rect *srcrect,
                           SDL_Rect dstrect);

  //  void updateCameraPos();

  std::shared_ptr<SCamera> m_camera;
  SDL_Renderer *renderer = nullptr;
  SDL_Window *window = nullptr;

  std::shared_ptr<SNodeGraph> m_tiles;
  std::shared_ptr<SNode> m_selectedTile;

  std::unordered_map<std::string, SDL_Texture *> textures;

  int m_screenWidth;
  int m_screenHeight;
};
