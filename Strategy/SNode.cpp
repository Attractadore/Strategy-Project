#include "SNode.hpp"

#include <algorithm>
#include <iostream>

SNode::SNode() {}

SNode::SNode(const SNode &other) {
  this->texturePath = other.texturePath;
  this->movementCost = other.movementCost;
}

SNode::SNode(SNode &&other) {
  this->texturePath = other.texturePath;
  this->movementCost = other.movementCost;
}

SNode::SNode(int x, int y) { this->pos = std::make_pair(x, y); }

SNode::~SNode() {}

SNode &SNode::operator=(const SNode &other) {
  this->texturePath = other.texturePath;
  this->movementCost = other.movementCost;
  return *this;
}

SNode &SNode::operator=(SNode &&other) {
  this->texturePath = other.texturePath;
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

void SNode::addUnitsToTile(std::vector<std::shared_ptr<SUnit>> p_units) {
  m_presentUnits.insert(m_presentUnits.end(), p_units.begin(), p_units.end());
}

std::shared_ptr<SUnit> SNode::removeUnitFromTile(std::shared_ptr<SUnit> unit) {
  auto it = std::find(m_presentUnits.begin(), m_presentUnits.end(), unit);
  if (it == m_presentUnits.end()) {
    return nullptr;
  }
  return *it;
}

std::vector<std::shared_ptr<SUnit>>
SNode::moveUnitsToTile(std::shared_ptr<SNode> p_target) {
  p_target->addUnitsToTile(m_presentUnits);
  auto tmp = m_presentUnits;
  m_presentUnits = {};
  return tmp;
}

std::shared_ptr<SUnit> SNode::moveUnitToTile(std::shared_ptr<SNode> p_target,
                                             std::shared_ptr<SUnit> p_unit) {
  auto it = std::find(m_presentUnits.begin(), m_presentUnits.end(), p_unit);
  if (it == m_presentUnits.end()) {
    return nullptr;
  }
  p_target->addUnitToTile(p_unit);
  m_presentUnits.erase(it);
  std::cout << "Number of present units " << m_presentUnits.size() << std::endl;
  return p_unit;
}

void SNode::setTexturePath(const std::string &newTexturePath) {
  this->texturePath = newTexturePath;
}

std::string &SNode::getTexturePath() { return this->texturePath; }
