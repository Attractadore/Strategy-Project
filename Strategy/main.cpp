#include "SNodeGraph.hpp"
#include "SRenderer.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main() {

  auto ng = std::make_shared<SNodeGraph>(250, 250, false, false);
  SRenderer renderer{ng};

  bool bQuit = false;
  SDL_Event e;

  glm::vec2 camInput{0.0f, 0.0f};

  auto time = std::chrono::steady_clock::now();
  float deltaTime = 0.0f;

  while (!bQuit) {

    auto newTime = std::chrono::steady_clock::now();
    deltaTime =
        std::chrono::duration_cast<std::chrono::microseconds>(newTime - time)
            .count() /
        1000000.0f;

    time = newTime;

    while (SDL_PollEvent(&e) != 0) {
      //      std::cout << "Got event" << std::endl;
      if (e.type == SDL_QUIT) {
        bQuit = true;
      } else if (e.type == SDL_KEYDOWN) {
        auto ks = e.key.keysym.scancode;
        switch (ks) {
        case SDL_SCANCODE_W:
          camInput.y -= 1.0f;
          break;
        case SDL_SCANCODE_S:
          camInput.y += 1.0f;
          break;
        case SDL_SCANCODE_A:
          camInput.x -= 1.0f;
          break;
        case SDL_SCANCODE_D:
          camInput.x += 1.0f;
          break;
        }
      } else if (e.type == SDL_MOUSEWHEEL) {
        renderer.addCameraZoom(std::min(1, std::max(e.wheel.y, -1)), deltaTime);
      }
    }
    if (glm::length(camInput) != 0.0f) {
      renderer.addCameraMovementInput(camInput, deltaTime);
    }
    camInput = glm::vec2(0.0f, 0.0f);
    renderer.render();

    //    std::this_thread::sleep_for(
    //        std::chrono::microseconds(1000000 / 60) -
    //        std::chrono::duration_cast<std::chrono::microseconds>(
    //            std::chrono::steady_clock::now() - time));
  }

  return 0;
}
