#include "SBuilding.hpp"

#include <iostream>

SBuilding::SBuilding() { m_numTurnsBuilding = 0; }

SBuilding::SBuilding(const SBuilding &other) {
  m_sprite = other.m_sprite;
  m_uiIconSprite = other.m_uiIconSprite;
  m_resourceGatherRate = other.m_resourceGatherRate;
  m_resourceCost = other.m_resourceCost;
  m_unitLookUpTable = other.m_unitLookUpTable;
  m_constructionTime = other.m_constructionTime;
  m_maxHealth = other.m_maxHealth;
  m_armour = other.m_armour;
  m_buildableUnits = other.m_buildableUnits;

  m_currentHealth = m_maxHealth / float(m_constructionTime);
  m_currentConstructionTurns = 0;
  m_numTurnsBuilding = 0;
}

// SBuilding::SBuilding(SBuilding &&other) {
//  m_sprite = other.m_sprite;
//  m_resourceGatherRate = other.m_resourceGatherRate;

//  m_numTurnsBuilding = 0;
//}

// SBuilding::~SBuilding() {}

SBuilding &SBuilding::operator=(const SBuilding &other) {
  m_sprite = other.m_sprite;
  m_uiIconSprite = other.m_uiIconSprite;
  m_resourceGatherRate = other.m_resourceGatherRate;
  m_resourceCost = other.m_resourceCost;
  m_unitLookUpTable = other.m_unitLookUpTable;
  m_constructionTime = other.m_constructionTime;
  m_maxHealth = other.m_maxHealth;
  m_armour = other.m_armour;
  m_buildableUnits = other.m_buildableUnits;

  m_currentHealth = m_maxHealth / float(m_constructionTime);
  m_numTurnsBuilding = 0;
  m_currentConstructionTurns = 0;

  return *this;
}

// SBuilding &SBuilding::operator=(SBuilding &&other) {
//  m_sprite = other.m_sprite;
//  m_resourceGatherRate = other.m_resourceGatherRate;

//  m_numTurnsBuilding = 0;

//  return *this;
//}

void SBuilding::setParams(
    std::unordered_map<std::string, float> buildingParams) {
  auto it = buildingParams.find("resourceGatherRate");
  if (it != buildingParams.end()) {
    m_resourceGatherRate = int(it->second);
  }
}

void SBuilding::setUnitLookUpTable(
    std::unordered_map<std::string, SUnit> *p_unitLookUpTable) {
  m_unitLookUpTable = p_unitLookUpTable;
}

void SBuilding::setSprite(std::shared_ptr<SSprite> newSprite) {
  m_sprite = newSprite;
}

void SBuilding::setuiIcon(std::shared_ptr<SSprite> newSprite) {
  m_uiIconSprite = newSprite;
}

std::shared_ptr<SSprite> SBuilding::getSprite() { return m_sprite; }

std::shared_ptr<SSprite> SBuilding::getuiIcon() { return m_uiIconSprite; }

void SBuilding::setOwner(int ownerId) { m_owningPlayerId = ownerId; }

int SBuilding::getOwner() { return m_owningPlayerId; }

bool SBuilding::finisingBuilding() {
  if (m_buildQueue.empty()) {
    return false;
  }

  return m_numTurnsBuilding ==
         (*m_unitLookUpTable)[m_buildQueue.front()].m_buildTime - 1;
}

std::string SBuilding::unitUnderConstruction() {
  if (m_buildQueue.empty()) {
    return "";
  }
  return m_buildQueue.front();
}

std::string SBuilding::removeUnitFromBuildQueue(int index) {
  if (index >= m_buildQueue.size()) {
    return "";
  }
  auto it = std::next(m_buildQueue.begin(), index);
  std::string unitUnderConstruction = *it;
  m_buildQueue.erase(it);
  return unitUnderConstruction;
}

const std::list<std::string> &SBuilding::getBuildQueue() const {
  return m_buildQueue;
}

bool SBuilding::isBuilding() { return !m_buildQueue.empty(); }

bool SBuilding::bUnderConstruction() {
  return m_currentConstructionTurns < m_constructionTime;
}

void SBuilding::refresh() {
  if (bUnderConstruction()) {
    m_currentConstructionTurns++;
    m_currentHealth += m_maxHealth / float(m_constructionTime);
  }
  if (!isBuilding()) {
    return;
  }
  std::cout << "Building " << m_buildQueue.front() << " for "
            << m_numTurnsBuilding << " turns" << std::endl;
  m_numTurnsBuilding++;
  if (m_numTurnsBuilding ==
      (*m_unitLookUpTable)[m_buildQueue.front()].m_buildTime) {
    m_buildQueue.pop_front();
    m_numTurnsBuilding = 0;
  }
}

void SBuilding::addUnitToBuildQueue(std::string unitId) {
  if (m_buildableUnits.count(unitId)) {
    m_buildQueue.push_back(unitId);
  }
}

void SBuilding::finishConstruction() {
  m_currentConstructionTurns = m_constructionTime;
}

void SBuilding::resetConstruction() { m_currentConstructionTurns = 0; }

bool SBuilding::bCanTrainUnit(std::string unit) {
  return m_buildableUnits.count(unit);
}
