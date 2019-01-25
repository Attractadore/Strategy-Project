#pragma once

#include <glm/vec2.hpp>

#include <list>
#include <memory>

class SUnit;
class SBuilding;

class SNode {
public:
  SNode();
  SNode(const SNode &other);
  SNode(SNode &&other);
  SNode(int x, int y);
  ~SNode();

  SNode &operator=(const SNode &other);
  SNode &operator=(SNode &&other);

  std::pair<int, int> getPos();
  int getMovementCost();
  std::shared_ptr<SBuilding> getTileBuilding();
  std::list<std::shared_ptr<SUnit>> getTileUnits();

  void setPos(int x, int y);
  void setPos(const std::pair<int, int> &newPos);
  void setMovementCost(int newMovementCost);

  std::list<std::shared_ptr<SUnit>>
  removeUnitsFromTile(std::list<std::shared_ptr<SUnit>> units);
  std::shared_ptr<SUnit> removeUnitFromTile(std::shared_ptr<SUnit> unit);

  void addUnitToTile(std::shared_ptr<SUnit> unit);

  void setTileBuilding(std::shared_ptr<SBuilding> newBuilding);

  void setTexturePath(const std::string &newTexturePath);
  std::string &getTexturePath();

protected:
  std::string texturePath;
  int movementCost;

private:
  std::pair<int, int> pos;
  std::shared_ptr<SBuilding> tileBuilding;
  std::list<std::shared_ptr<SUnit>> presentUnits;
};
