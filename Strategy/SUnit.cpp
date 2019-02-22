#include "SUnit.hpp"

#include "SNode.hpp"

#include <algorithm>
#include <iostream>

SUnit::SUnit()
{
}

SUnit::SUnit(UNIT_ID unitID)
{
  m_ID = unitID;
}

SUnit::SUnit(const SUnit& other)
{
  copyStats(other);
  m_cHealth = m_health;
  m_cMoves = m_moves;
  m_cXP = 0;
}

SUnit& SUnit::operator=(const SUnit& other)
{
  copyStats(other);
  m_cHealth = m_health;
  m_cMoves = m_moves;
  m_cXP = 0;
  return *this;
}

void SUnit::setPromotionUnit(SUnit* newPromotionUnit)
{
  m_promotionUnit = newPromotionUnit;
}

void SUnit::copyStats(const SUnit& other)
{
  SCombatReady::copyStats(other);
  m_promotionUnit = other.m_promotionUnit;
}

void SUnit::addXP(int amount)
{
  m_cXP += amount;
  if (m_cXP >= PROMOTION_XP)
  {
    promote();
  }
}

int SUnit::getXP()
{
  return m_cXP;
}

void SUnit::promote()
{
  if (m_promotionUnit == nullptr)
  {
    return;
  }
  copyStats(*m_promotionUnit);
  m_cHealth = m_health;
  // Movement points remain the same
  m_cXP = 0;
}
