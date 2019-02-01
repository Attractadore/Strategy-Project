#pragma once

#include <memory>
#include <string>
#include <vector>

class SNode;
struct SSprite;

class SUnit {
public:
  SUnit();
  SUnit(const SUnit &other);
  SUnit(SUnit &&other);
  virtual ~SUnit();

  SUnit &operator=(const SUnit &other);
  SUnit &operator=(SUnit &&other);

  virtual bool isDead();
  virtual int applyDamage(int amount);
  virtual int dealDamage(float chance);

  std::shared_ptr<SSprite> getSprite();
  void setSprite(std::shared_ptr<SSprite> newSprite);

  void setMaxHP(int p_maxHP);
  void setDamage(int p_damage);
  void setAccuracy(int p_accuracy);
  void setMaxMoves(int p_maxMoves);

  void refresh();

protected:
  int m_maxHP;
  int m_damage;
  float m_accuracy;
  int m_maxMoves;

  std::shared_ptr<SSprite> m_sprite;

private:
  int m_currentHP;
  int m_currentMoves;

  std::vector<std::shared_ptr<SNode>> m_route;
};
