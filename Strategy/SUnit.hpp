#pragma once

#include <memory>

struct SDL_Texture;

class SUnit {
public:
  SUnit();
  virtual ~SUnit();

  virtual bool isDead();
  virtual int restoreMoves();
  virtual int applyDamage(int amount);
  virtual int dealDamage(float chance);

  SDL_Texture *getTexture();
  void setTexture(SDL_Texture *texture);

protected:
  int currentHP;
  int currentMoves;

  int maxHP;
  int damage;
  float accuracy;
  int maxMoves;

  SDL_Texture *texture;
};
