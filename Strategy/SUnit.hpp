#pragma once

#include <memory>
#include <string>

class SUnit {
public:
  SUnit();
  SUnit(const SUnit &other);
  SUnit(SUnit &&other);
  virtual ~SUnit();

  SUnit &operator=(const SUnit &other);
  SUnit &operator=(SUnit &&other);

  virtual bool isDead();
  virtual int restoreMoves();
  virtual int applyDamage(int amount);
  virtual int dealDamage(float chance);

  std::string &getTexturePath();
  void setTexturePath(const std::string &texturePath);

protected:
  int maxHP;
  int damage;
  float accuracy;
  int maxMoves;

  std::string texturePath;

private:
  int currentHP;
  int currentMoves;
};
