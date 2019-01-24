#include "SBuilding.hpp"

SBuilding::SBuilding() {}

void SBuilding::setBuildingTexture(SDL_Texture *newTexture) {
  SBuilding::buildingTexture = newTexture;
}

SDL_Texture *SBuilding::getBuildingTexture() {
  return SBuilding::buildingTexture;
}
