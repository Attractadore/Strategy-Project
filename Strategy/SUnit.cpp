#include "SUnit.hpp"

#include "SNode.hpp"

#include <algorithm>
#include <iostream>

SUnit::SUnit() {}

SUnit::SUnit(std::string p_unitId) { m_unitId = p_unitId; }

SUnit::SUnit(const SUnit &other) { copyStats(other); }

void SUnit::copyStats(const SUnit &other) {
  m_unitId = other.m_unitId;
  m_maxHP = other.m_maxHP;
  m_damage = other.m_damage;
  m_accuracy = other.m_accuracy;
  m_maxMoves = other.m_maxMoves;
  m_buildTime = other.m_buildTime;
  m_resourceCost = other.m_resourceCost;

  m_sprite = other.m_sprite;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;
}

SUnit::~SUnit() {}

SUnit &SUnit::operator=(const SUnit &other) {
  copyStats(other);
  return *this;
}

bool SUnit::isDead() { return m_currentHP <= 0; }

int SUnit::removeMoves(int numMoves) {
  if (m_currentMoves <= 0) {
    return 0;
  }
  m_currentMoves -= numMoves;
  return numMoves;
}

int SUnit::applyDamage(int amount) {
  int dHP = std::max(amount, m_currentHP);
  m_currentHP -= amount;
  return dHP;
}

int SUnit::dealDamage(float chance) {
  if (chance > m_accuracy) {
    return 0;
  } else {
    return m_damage;
  }
}

std::shared_ptr<SSprite> SUnit::getSprite() { return m_sprite; }

void SUnit::setSprite(std::shared_ptr<SSprite> newSprite) {
  m_sprite = newSprite;
}

void SUnit::setOwner(int ownerId) { m_owningPlayerId = ownerId; }

int SUnit::getOwner() { return m_owningPlayerId; }

void SUnit::setParams(std::unordered_map<std::string, float> params) {
  auto it = params.find("maxHP");
  if (it != params.end()) {
    m_maxHP = int(it->second);
  }
  it = params.find("damage");
  if (it != params.end()) {
    m_damage = int(it->second);
  }
  it = params.find("accuracy");
  if (it != params.end()) {
    m_accuracy = it->second;
  }
  it = params.find("maxMoves");
  if (it != params.end()) {
    m_maxMoves = int(it->second);
  }
  it = params.find("buildTime");
  if (it != params.end()) {
    m_buildTime = int(it->second);
  }
  it = params.find("resourceCost");
  if (it != params.end()) {
    m_resourceCost = int(it->second);
  }
}

void SUnit::setTargetTile(std::shared_ptr<SNode> newTargetTile) {
  m_targetTile = newTargetTile;
}

void SUnit::resetTargetTile() { m_targetTile = nullptr; }

void SUnit::setCurrentTile(std::shared_ptr<SNode> newCurrentTile) {
  m_currentTile = newCurrentTile;
}

std::shared_ptr<SNode> SUnit::getTargetTile() { return m_targetTile; }

std::shared_ptr<SNode> SUnit::getCurrentTile() { return m_currentTile; }

void SUnit::moveTile(std::shared_ptr<SNode> tile) {
  m_currentMoves -= tile->getMovementCost();
  m_currentTile = tile;
}

bool SUnit::canAdvanceRoute() {
  return m_currentMoves > 0 and m_currentTile != m_targetTile and
         m_targetTile != nullptr;
}

bool SUnit::finishedRoute() { return m_currentTile == m_targetTile; }

void SUnit::refresh() { m_currentMoves = m_maxMoves; }
