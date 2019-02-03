#include "SBuilding.hpp"

#include <iostream>

SBuilding::SBuilding() { m_numTurnsBuilding = 0; }

SBuilding::SBuilding(const SBuilding &other) {
  m_sprite = other.m_sprite;
  m_supplyProvided = other.m_supplyProvided;
  m_resourceGatherRate = other.m_resourceGatherRate;

  m_numTurnsBuilding = 0;
}

SBuilding::SBuilding(SBuilding &&other) {
  m_sprite = other.m_sprite;
  m_supplyProvided = other.m_supplyProvided;
  m_resourceGatherRate = other.m_resourceGatherRate;

  m_numTurnsBuilding = 0;
}

SBuilding::~SBuilding() {}

SBuilding &SBuilding::operator=(const SBuilding &other) {
  m_sprite = other.m_sprite;
  m_supplyProvided = other.m_supplyProvided;
  m_resourceGatherRate = other.m_resourceGatherRate;

  m_numTurnsBuilding = 0;

  return *this;
}

SBuilding &SBuilding::operator=(SBuilding &&other) {
  m_sprite = other.m_sprite;
  m_supplyProvided = other.m_supplyProvided;
  m_resourceGatherRate = other.m_resourceGatherRate;

  m_numTurnsBuilding = 0;

  return *this;
}

void SBuilding::setParams(
    std::unordered_map<std::string, float> buildingParams) {
  auto it = buildingParams.find("supplyProvided");
  if (it != buildingParams.end()) {
    m_supplyProvided = int(it->second);
  }
  it = buildingParams.find("resourceGatherRate");
  if (it != buildingParams.end()) {
    m_resourceGatherRate = int(it->second);
  }
}

void SBuilding::setUnitLookUpTable(
    std::unordered_map<std::string, SUnit> &p_unitLookUpTable) {
  m_unitLookUpTable = p_unitLookUpTable;
}

void SBuilding::setSprite(std::shared_ptr<SSprite> newSprite) {
  m_sprite = newSprite;
}

std::shared_ptr<SSprite> SBuilding::getSprite() { return m_sprite; }

void SBuilding::setOwner(int ownerId) { m_owningPlayerId = ownerId; }

int SBuilding::getOwner() { return m_owningPlayerId; }

bool SBuilding::finisingBuilding() {
  if (m_buildQueue.empty()) {
    return false;
  }

  return m_numTurnsBuilding ==
         m_unitLookUpTable[m_buildQueue.front()].getBuildTime() - 1;
}

std::string SBuilding::unitUnderConstruction() {
  if (m_buildQueue.empty()) {
    return "";
  }
  return m_buildQueue.front();
}

bool SBuilding::isBuilding() {
  std::cout << "Building for " << m_numTurnsBuilding << std::endl;
  if (!m_buildQueue.empty()) {
    std::cout << "Unit build time "
              << m_unitLookUpTable[m_buildQueue.front()].getBuildTime()
              << std::endl;
  }
  return !m_buildQueue.empty();
}

void SBuilding::refresh() {
  if (!isBuilding()) {
    return;
  }
  m_numTurnsBuilding++;
  if (m_numTurnsBuilding ==
      m_unitLookUpTable[m_buildQueue.front()].getBuildTime()) {
    m_buildQueue.pop_front();
    m_numTurnsBuilding = 0;
  }
}

int SBuilding::getResourceGatherRate() { return m_resourceGatherRate; }

int SBuilding::getSupplyProvided() { return m_supplyProvided; }

void SBuilding::addUnitToBuildQueue(std::string unitId) {
  m_buildQueue.push_back(unitId);
}
