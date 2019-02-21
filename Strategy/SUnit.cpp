#include "SUnit.hpp"

#include "SNode.hpp"

#include <algorithm>
#include <iostream>

SUnit::SUnit()
{
}

SUnit::SUnit(std::string p_unitId)
{
  m_unitId = p_unitId;
}

///*
SUnit::SUnit(const SUnit& other)
{
  copyStats(other);
}

void SUnit::copyStats(const SUnit& other)
{
  m_unitId = other.m_unitId;
  m_maxHP = other.m_maxHP;
  m_damage = other.m_damage;
  m_accuracy = other.m_accuracy;
  m_maxMoves = other.m_maxMoves;
  m_buildTime = other.m_buildTime;
  m_resourceCost = other.m_resourceCost;

  m_sprite = other.m_sprite;
  m_teamColorSprite = other.m_teamColorSprite;

  m_currentHP = m_maxHP;
  m_currentMoves = m_maxMoves;
}

SUnit::~SUnit()
{
}

SUnit& SUnit::operator=(const SUnit& other)
{
  copyStats(other);
  return *this;
}

//*/

int SUnit::removeMoves(int numMoves)
{
  if (m_currentMoves <= 0)
  {
    return 0;
  }
  m_currentMoves -= numMoves;
  return numMoves;
}

std::shared_ptr<SSprite> SUnit::getSprite()
{
  return m_sprite;
}

void SUnit::setSprite(std::shared_ptr<SSprite> newSprite)
{
  m_sprite = newSprite;
}

std::shared_ptr<SSprite> SUnit::getTeamColorSprite()
{
  return m_teamColorSprite;
}

void SUnit::setTeamColorSprite(std::shared_ptr<SSprite> newTeamColorSprite)
{
  m_teamColorSprite = newTeamColorSprite;
}

void SUnit::setOwner(int ownerId)
{
  m_owningPlayerId = ownerId;
}

int SUnit::getOwner()
{
  return m_owningPlayerId;
}

int SUnit::getCurrentHealth()
{
  return m_currentHP;
}

bool SUnit::bCanMove()
{
  return m_currentMoves > 0;
}

void SUnit::refresh()
{
  m_currentMoves = m_maxMoves;
}
