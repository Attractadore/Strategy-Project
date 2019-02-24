#include "SCombatReady.hpp"

int SCombatReady::dealDamage(float chance, float accuracyModifier)
{
  if (m_attackRange == 0)
  {
    accuracyModifier = 0;
  }
  return (chance <= m_accuracy + accuracyModifier) ? m_damage : 0;
}

int SCombatReady::applyDamage(int damage, int armorModifier)
{
  int appliedDamage = std::min(m_cHealth, damage - (m_armor + armorModifier));
  if (appliedDamage < 0)
  {
    return 0;
  }
  m_cHealth -= appliedDamage;
  return appliedDamage;
}

void SCombatReady::setOwner(int newOwnerId)
{
  m_ownerId = newOwnerId;
}

bool SCombatReady::bIsDead()
{
  return m_cHealth <= 0;
}
int SCombatReady::getHealth()
{
  return m_cHealth;
}

int SCombatReady::getOwner()
{
  return m_ownerId;
}

std::shared_ptr<SSprite> SCombatReady::getSprite()
{
  return m_sprite;
}

std::shared_ptr<SSprite> SCombatReady::getTCSprite()
{
  return m_TCSprite;
}

std::shared_ptr<SSprite> SCombatReady::getUISprite()
{
  return m_UISprite;
}

void SCombatReady::setSprite(std::shared_ptr<SSprite> newSprite)
{
  m_sprite = newSprite;
}

void SCombatReady::setTCSprite(std::shared_ptr<SSprite> newTCSprite)
{
  m_TCSprite = newTCSprite;
}

void SCombatReady::setUISprite(std::shared_ptr<SSprite> newUISprite)
{
  m_UISprite = newUISprite;
}

void SCombatReady::copyStats(const SCombatReady& other)
{
  m_ID = other.m_ID;
  m_health = other.m_health;
  m_armor = other.m_armor;
  m_damage = other.m_damage;
  m_attackRange = other.m_attackRange;
  m_accuracy = other.m_accuracy;
  m_buildTime = other.m_buildTime;
  m_resourceCost = other.m_resourceCost;

  m_sprite = other.m_sprite;
  m_TCSprite = other.m_TCSprite;
  m_UISprite = other.m_UISprite;
}
