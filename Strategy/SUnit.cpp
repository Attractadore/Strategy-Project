#include <SDL2/SDL.h>

#include "SUnit.hpp"

SUnit::SUnit() {
  //  this->currentHP = SUnit::maxHP;
  //  this->currentMoves = SUnit::maxMoves;
  this->currentHP = maxHP;
  this->currentMoves = maxMoves;
}

bool SUnit::isDead() { return this->currentHP <= 0; }

int SUnit::restoreMoves() {
  int dm = SUnit::maxMoves - this->currentMoves;
  this->currentMoves = SUnit::maxMoves;
  return dm;
}

int SUnit::applyDamage(int amount) {
  int dHP = std::max(amount, this->currentHP);
  this->currentHP -= amount;
  return dHP;
}

int SUnit::dealDamage(float chance) {
  if (chance > SUnit::accuracy) {
    return 0;
  } else {
    return SUnit::damage;
  }
}

SDL_Texture* SUnit::getTexture() { return texture; }

void SUnit::setTexture(SDL_Texture* texture) {
  SUnit::texture = texture;
}

SSpearman::SSpearman() : SUnit() {}

SSpearman::~SSpearman() {}
