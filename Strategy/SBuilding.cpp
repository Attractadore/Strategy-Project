#include "SBuilding.hpp"

SBuilding::SBuilding() {}

SBuilding::SBuilding(const SBuilding &other) {
  this->texturePath = other.texturePath;
}

SBuilding::SBuilding(SBuilding &&other) {
  this->texturePath = other.texturePath;
}

SBuilding::~SBuilding() {}

SBuilding &SBuilding::operator=(const SBuilding &other) {
  this->texturePath = other.texturePath;
  return *this;
}

SBuilding &SBuilding::operator=(SBuilding &&other) {
  this->texturePath = other.texturePath;
  return *this;
}

void SBuilding::setTexturePath(const std::string &newTexturePath) {
  this->texturePath = newTexturePath;
}

std::string &SBuilding::getTexturePath() { return this->texturePath; }
