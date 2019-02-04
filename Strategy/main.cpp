#define SDL_MAIN_HANDLED
#include "SGameManager.hpp"

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main() {
  SGameManager().run();
  return 0;
}
