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

  m_sprite = other.m_sprite;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;
}

SUnit::SUnit(SUnit &&other) {
  m_maxHP = other.m_maxHP;
  m_damage = other.m_damage;
  m_accuracy = other.m_accuracy;
  m_maxMoves = other.m_maxMoves;

  m_sprite = other.m_sprite;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;
}

SUnit::~SUnit() {}

SUnit &SUnit::operator=(const SUnit &other) {
  m_maxHP = other.m_maxHP;
  m_damage = other.m_damage;
  m_accuracy = other.m_accuracy;
  m_maxMoves = other.m_maxMoves;

  m_sprite = other.m_sprite;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;

  return *this;
}

SUnit &SUnit::operator=(SUnit &&other) {
  m_maxHP = other.m_maxHP;
  m_damage = other.m_damage;
  m_accuracy = other.m_accuracy;
  m_maxMoves = other.m_maxMoves;

  m_sprite = other.m_sprite;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;

  return *this;
}

bool SUnit::isDead() { return m_currentHP <= 0; }

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

void SUnit::setMaxHP(int p_maxHP) { m_maxHP = p_maxHP; }

void SUnit::setDamage(int p_damage) { m_damage = p_damage; }

void SUnit::setAccuracy(int p_accuracy) { m_accuracy = p_accuracy; }

void SUnit::setMaxMoves(int p_maxMoves) { m_maxMoves = p_maxMoves; }

void SUnit::refresh() { m_currentMoves = m_maxMoves; }
