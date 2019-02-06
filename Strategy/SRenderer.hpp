#pragma once

//#include "SSprite.hpp"
#include "SCamera.hpp"

#include <SDL2/SDL.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

class SNodeGraph;
class SNode;
struct SSprite;
struct SCamera;

// struct SDL_Renderer;
// struct SDL_Window;
// struct SDL_Texture;
// struct SDL_Rect;
typedef struct _TTF_Font TTF_Font;

enum class RenderLocation {
  RENDER_TOP_LEFT,
  RENDER_TOP_RIGHT,
  RENDER_CENTER,
  RENDER_BOTTOM_LEFT,
  RENDER_BOTTOM_RIGHT
};

struct RenderRequest {
  RenderRequest(std::shared_ptr<SSprite> p_sprite, int p_x, int p_y,
                int p_frameIndex, RenderLocation p_renderLocation,
                bool p_bWorld);
  RenderRequest(const RenderRequest &other);
  ~RenderRequest();

  RenderRequest &operator=(const RenderRequest &other);

  std::shared_ptr<SSprite> m_sprite;
  int m_x;
  int m_y;
  int m_frameIndex;
  int m_bWorld;
  RenderLocation m_renderLocation;
};

struct TextRenderRequest {
  TextRenderRequest(std::string p_text, int p_x, int p_y, int p_h,
                    int p_renderPriority, RenderLocation p_renderLocation);

  std::string m_text;
  int m_x;
  int m_y;
  int m_w;
  int m_h;
  int m_renderPriority;
  RenderLocation m_renderLocation;
};

bool operator==(const RenderRequest &lhs, const RenderRequest &rhs);
bool operator<(const RenderRequest &lhs, const RenderRequest &rhs);
bool operator==(const TextRenderRequest &lhs, const TextRenderRequest &rhs);
bool operator<(const TextRenderRequest &lhs, const TextRenderRequest &rhs);

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
  void submitRenderRequest(std::shared_ptr<SSprite> p_sprite, int x, int y,
                           int frameIndex, RenderLocation renderLocation,
                           bool bWorld = true);
  void submitTextRenderRequest(std::string text, int x, int y, int h,
                               int renderPriority,
                               RenderLocation renderLocation);
  //  void submitTextRenderRequest(std::string text, int x, int y,
  //                               RenderLocation renderLocation);

private:
  void renderThread();
  void loadTexture(std::string path);
  void renderText(std::string text);

  std::shared_ptr<SCamera> m_camera;
  glm::vec2 m_tmpCameraPos;
  float m_tmpCameraZoom;
  SDL_Renderer *m_renderer = nullptr;
  SDL_Window *window = nullptr;

  std::unordered_map<std::string, SDL_Texture *> textures;
  std::unordered_map<std::string, float> m_textAspectRatios;

  int m_screenWidth;
  int m_screenHeight;
  float m_screenRatio;
  float m_realVirtualRatio;

  std::priority_queue<RenderRequest> m_drawQueue;
  std::priority_queue<TextRenderRequest> m_textDrawQueue;
  std::priority_queue<RenderRequest> m_tmpQueue;
  std::priority_queue<TextRenderRequest> m_tmpTextDrawQueue;
  std::thread m_renderThread;

  SDL_Color m_textColor;
  TTF_Font *m_textFont;
};
