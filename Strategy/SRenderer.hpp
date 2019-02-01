#pragma once

//#include "SSprite.hpp"
#include "SCamera.hpp"

#include <mutex>
#include <memory>
#include <string>
#include <unordered_map>
#include <queue>
#include <thread>
#include <atomic>

class SNodeGraph;
class SNode;
struct SSprite;
struct SCamera;

struct SDL_Renderer;
struct SDL_Window;
struct SDL_Texture;
struct SDL_Rect;

enum class RenderLocation{
    RENDER_TOP_LEFT,
    RENDER_TOP_RIGHT,
    RENDER_CENTER,
    RENDER_BOTTOM_LEFT,
    RENDER_BOTTOM_RIGHT
};

struct RenderRequest {
    RenderRequest(std::shared_ptr<SSprite> p_sprite, int p_x, int p_y, int p_frameIndex, RenderLocation p_renderLocation);
    RenderRequest(const RenderRequest& other);
    ~RenderRequest();

    RenderRequest& operator=(const RenderRequest& other);
    bool operator==(const RenderRequest other);
    bool operator<(const RenderRequest other);

    std::shared_ptr<SSprite> m_sprite;
    int m_x;
    int m_y;
    int m_frameIndex;
    RenderLocation m_renderLocation;
};

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
  void submitRenderRequest(std::shared_ptr<SSprite> p_sprite, int x, int y, int frameIndex, RenderLocation renderLocation);

private:
  void renderThread();
  void loadTexture(std::string path);

//  inline void renderCenter(SDL_Texture *texture, SDL_Rect *srcrect,
//                           SDL_Rect dstrect);

  //  void updateCameraPos();

  std::shared_ptr<SCamera> m_camera;
  SCamera m_tmpCamera;
  SDL_Renderer *renderer = nullptr;
  SDL_Window *window = nullptr;

  std::unordered_map<std::string, SDL_Texture *> textures;

  int m_screenWidth;
  int m_screenHeight;

  std::priority_queue<RenderRequest> m_drawQueue;
  std::priority_queue<RenderRequest> m_tmpQueue;
  std::atomic<bool> m_bRenderDone;
  std::thread m_renderThread;

};
