#include "SNode.hpp"

#include "SUnit.hpp"

#include <algorithm>
#include <iostream>

SNode::SNode()
{
}

SNode::SNode(const SNode& other)
{
  m_sprite = other.m_sprite;
  m_movementCost = other.m_movementCost;
  m_armorModifier = other.m_armorModifier;
  m_accuracyModifier = other.m_accuracyModifier;
  bPassable = other.bPassable;

  m_currentMana = 0;
}

SNode::SNode(int x, int y)
{
  pos = std::make_pair(x, y);
}

SNode::~SNode()
{
}

SNode& SNode::operator=(const SNode& other)
{
  m_sprite = other.m_sprite;
  m_movementCost = other.m_movementCost;
  m_armorModifier = other.m_armorModifier;
  m_accuracyModifier = other.m_accuracyModifier;
  bPassable = other.bPassable;

  m_currentMana = 0;

  return *this;
}

std::pair<int, int> SNode::getPos()
{
  return pos;
}

void SNode::setPos(int x, int y)
{
  pos = std::make_pair(x, y);
}

void SNode::setPos(const std::pair<int, int>& newPos)
{
  pos = newPos;
}

void SNode::setSprite(std::shared_ptr<SSprite> newSprite)
{
  m_sprite = newSprite;
}

std::shared_ptr<SSprite> SNode::getSprite()
{
  return m_sprite;
}

void SNode::addGeyser()
{
  m_bHasGeyser = true;
}

void SNode::addMana(int amount)
{
  m_currentMana += amount;
}

bool SNode::bHasGeyser()
{
  return m_bHasGeyser;
}

bool SNode::bHasMana()
{
  return m_currentMana > 0;
}

int SNode::getAndRemoveMana()
{
  int tmp = m_currentMana;
  m_currentMana = 0;
  return tmp;
}

int SNode::getCurrentMana()
{
  return m_currentMana;
}

void SNode::addFoundation()
{
  m_bHasFoundation = true;
}

bool SNode::bHasFoundation()
{
  return m_bHasFoundation;
}
