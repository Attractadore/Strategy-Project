#include "SNode.hpp"

#include "SUnit.hpp"

#include <algorithm>
#include <iostream>

SNode::SNode() {}

SNode::SNode(const SNode &other) {
  m_sprite = other.m_sprite;
  this->movementCost = other.movementCost;
}

SNode::SNode(SNode &&other) {
  m_sprite = other.m_sprite;
  this->movementCost = other.movementCost;
}

SNode::SNode(int x, int y) { this->pos = std::make_pair(x, y); }

SNode::~SNode() {}

SNode &SNode::operator=(const SNode &other) {
  m_sprite = other.m_sprite;
  this->movementCost = other.movementCost;
  return *this;
}

SNode &SNode::operator=(SNode &&other) {
  m_sprite = other.m_sprite;
  this->movementCost = other.movementCost;
  return *this;
}

std::pair<int, int> SNode::getPos() { return this->pos; }

int SNode::getMovementCost() { return this->movementCost; }

std::shared_ptr<SBuilding> SNode::getTileBuilding() {
  return this->tileBuilding;
}

std::vector<std::shared_ptr<SUnit>> SNode::getTileUnits() {
  return m_presentUnits;
}

void SNode::setPos(int x, int y) { this->pos = std::make_pair(x, y); }

void SNode::setPos(const std::pair<int, int> &newPos) { this->pos = newPos; }

void SNode::setMovementCost(int newMovementCost) {
  this->movementCost = newMovementCost;
}

void SNode::setTileBuilding(std::shared_ptr<SBuilding> newBuilding) {
  this->tileBuilding = newBuilding;
}

void SNode::addUnitToTile(std::shared_ptr<SUnit> unit) {
  m_presentUnits.push_back(unit);
}

void SNode::setSprite(std::shared_ptr<SSprite> newSprite) {
  m_sprite = newSprite;
}

std::shared_ptr<SSprite> SNode::getSprite() { return m_sprite; }
