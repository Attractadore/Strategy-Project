#include "SUnit.hpp"

SUnit::SUnit() {
  this->currentHP = maxHP;
  this->currentMoves = maxMoves;
}

SUnit::SUnit(const SUnit &other) {
  this->maxHP = other.maxHP;
  this->damage = other.damage;
  this->accuracy = other.accuracy;
  this->maxMoves = other.maxMoves;

  this->texturePath = other.texturePath;

  this->currentHP = this->maxHP;
  this->currentMoves = this->maxMoves;
}

SUnit::SUnit(SUnit &&other) {
  this->maxHP = other.maxHP;
  this->damage = other.damage;
  this->accuracy = other.accuracy;
  this->maxMoves = other.maxMoves;

  this->texturePath = other.texturePath;

  this->currentHP = this->maxHP;
  this->currentMoves = this->maxMoves;
}

SUnit::~SUnit() {}

SUnit &SUnit::operator=(const SUnit &other) {
  this->maxHP = other.maxHP;
  this->damage = other.damage;
  this->accuracy = other.accuracy;
  this->maxMoves = other.maxMoves;

  this->texturePath = other.texturePath;

  this->currentHP = this->maxHP;
  this->currentMoves = this->maxMoves;

  return *this;
}

SUnit &SUnit::operator=(SUnit &&other) {
  this->maxHP = other.maxHP;
  this->damage = other.damage;
  this->accuracy = other.accuracy;
  this->maxMoves = other.maxMoves;

  this->texturePath = other.texturePath;

  this->currentHP = this->maxHP;
  this->currentMoves = this->maxMoves;

  return *this;
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
  if (chance > this->accuracy) {
    return 0;
  } else {
    return this->damage;
  }
}

std::string &SUnit::getTexturePath() { return this->texturePath; }

void SUnit::setTexturePath(const std::string &newTexturePath) {
  this->texturePath = newTexturePath;
}
