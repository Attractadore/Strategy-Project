#include "SGameManager.hpp"

#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main() {
  SGameManager gm{};
  gm.run();
  return 0;
}
