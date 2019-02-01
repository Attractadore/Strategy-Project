#include "SBuilding.hpp"

SBuilding::SBuilding() {}

SBuilding::SBuilding(const SBuilding &other) { m_sprite = other.m_sprite; }

SBuilding::SBuilding(SBuilding &&other) { m_sprite = other.m_sprite; }

SBuilding::~SBuilding() {}

SBuilding &SBuilding::operator=(const SBuilding &other) {
  m_sprite = other.m_sprite;
  return *this;
}

SBuilding &SBuilding::operator=(SBuilding &&other) {
  m_sprite = other.m_sprite;
  return *this;
}

void SBuilding::setSprite(std::shared_ptr<SSprite> newSprite) {
  m_sprite = newSprite;
}

std::shared_ptr<SSprite> SBuilding::getSprite() { return m_sprite; }
