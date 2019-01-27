#include "SUnit.hpp"

#include "SNode.hpp"

#include <algorithm>
#include <iostream>

SUnit::SUnit() {}

SUnit::SUnit(const SUnit &other) {
  m_maxHP = other.m_maxHP;
  m_damage = other.m_damage;
  m_accuracy = other.m_accuracy;
  m_maxMoves = other.m_maxMoves;

  m_texturePath = other.m_texturePath;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;
}

SUnit::SUnit(SUnit &&other) {
  m_maxHP = other.m_maxHP;
  m_damage = other.m_damage;
  m_accuracy = other.m_accuracy;
  m_maxMoves = other.m_maxMoves;

  m_texturePath = other.m_texturePath;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;
}

SUnit::~SUnit() {}

SUnit &SUnit::operator=(const SUnit &other) {
  m_maxHP = other.m_maxHP;
  m_damage = other.m_damage;
  m_accuracy = other.m_accuracy;
  m_maxMoves = other.m_maxMoves;

  m_texturePath = other.m_texturePath;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;

  return *this;
}

SUnit &SUnit::operator=(SUnit &&other) {
  m_maxHP = other.m_maxHP;
  m_damage = other.m_damage;
  m_accuracy = other.m_accuracy;
  m_maxMoves = other.m_maxMoves;

  m_texturePath = other.m_texturePath;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;

  return *this;
}

bool SUnit::isDead() { return m_currentHP <= 0; }

int SUnit::restoreMoves() {
  int dm = m_maxMoves - m_currentMoves;
  m_currentMoves = SUnit::m_maxMoves;
  return dm;
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

std::string &SUnit::getTexturePath() { return m_texturePath; }

void SUnit::setTexturePath(const std::string &newTexturePath) {
  m_texturePath = newTexturePath;
}

void SUnit::setMaxHP(int p_maxHP) { m_maxHP = p_maxHP; }

void SUnit::setDamage(int p_damage) { m_damage = p_damage; }

void SUnit::setAccuracy(int p_accuracy) { m_accuracy = p_accuracy; }

void SUnit::setMaxMoves(int p_maxMoves) { m_maxMoves = p_maxMoves; }

void SUnit::setMovementRoute(
    const std::vector<std::shared_ptr<SNode>> &p_route) {
  m_route = p_route;
  std::reverse(m_route.begin(), m_route.end());
  std::cout << "Set unit movement route of length " << m_route.size()
            << std::endl;
}

void SUnit::moveRoute() {
  while (m_currentMoves > 0) {
    auto n = m_route.back();
    auto npos = n->getPos();
    //    std::cout << "Currently at tile at " << npos.first << " " <<
    //    npos.second
    //              << std::endl;
    m_route.pop_back();
    if (m_route.size() == 0) {
      break;
    }
    n->moveUnitToTile(m_route.back(), std::shared_ptr<SUnit>(this));
    //    std::cout << "Moved unit to next tile " << std::endl;
    n = m_route.back();
    npos = n->getPos();
    //    std::cout << "Now at tile at " << npos.first << " " << npos.second
    //              << std::endl;
    //    std::cout << "Number of present units " << n->getTileUnits().size()
    //              << std::endl;
    //    std::cout << "Present unit is self " << std::boolalpha
    //              << (std::shared_ptr<SUnit>(this) == n->getTileUnits()[0])
    //              << std::endl;
    m_currentMoves -= m_route.back()->getMovementCost();
  }
}

void SUnit::refresh() {
  if (m_currentMoves > 0 and m_route.size() > 0) {
    moveRoute();
  }
  m_currentMoves = m_maxMoves;
}
