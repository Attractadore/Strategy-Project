#pragma once

#include <memory>
#include <string>

struct SSprite;

class SCombatReady
{
  public:
  virtual int dealDamage(float chance, float accuracyModifier = 0);
  virtual int applyDamage(int damage, int armorModifier = 0);
  virtual void setOwner(int newOwnerId);

  virtual bool bIsDead();

  virtual void refresh() = 0;

  virtual int getHealth();
  virtual int getOwner();

  virtual std::shared_ptr<SSprite> getSprite();
  virtual std::shared_ptr<SSprite> getTCSprite();
  virtual std::shared_ptr<SSprite> getUISprite();
  virtual void setSprite(std::shared_ptr<SSprite> newSprite);
  virtual void setTCSprite(std::shared_ptr<SSprite> newTCSprite);
  virtual void setUISprite(std::shared_ptr<SSprite> newUISprite);

  virtual void copyStats(const SCombatReady& other);

  std::string m_ID;
  int m_health;
  int m_armor;
  int m_damage;
  int m_attackRange;
  float m_accuracy;
  int m_buildTime;
  int m_resourceCost;

  protected:
  std::shared_ptr<SSprite> m_sprite;
  std::shared_ptr<SSprite> m_TCSprite;
  std::shared_ptr<SSprite> m_UISprite;

  int m_cHealth;
  int m_ownerId;
};
