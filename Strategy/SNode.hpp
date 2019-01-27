#pragma once

#include <glm/vec2.hpp>

#include <memory>
#include <vector>

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
  std::vector<std::shared_ptr<SUnit>> getTileUnits();
  std::vector<std::shared_ptr<SUnit>>
  moveUnitsToTile(std::shared_ptr<SNode> p_target);
  std::vector<std::shared_ptr<SUnit>>
  moveUnitsToTile(std::shared_ptr<SNode> p_target,
                  std::vector<std::shared_ptr<SUnit>> p_units);
  std::shared_ptr<SUnit> moveUnitToTile(std::shared_ptr<SNode> p_target,
                                        std::shared_ptr<SUnit> p_unit);

  void setPos(int x, int y);
  void setPos(const std::pair<int, int> &newPos);
  void setMovementCost(int newMovementCost);

  std::vector<std::shared_ptr<SUnit>>
  removeUnitsFromTile(std::vector<std::shared_ptr<SUnit>> units);
  std::shared_ptr<SUnit> removeUnitFromTile(std::shared_ptr<SUnit> unit);

  void addUnitToTile(std::shared_ptr<SUnit> unit);
  void addUnitsToTile(std::vector<std::shared_ptr<SUnit>> p_units);

  void setTileBuilding(std::shared_ptr<SBuilding> newBuilding);

  void setTexturePath(const std::string &newTexturePath);
  std::string &getTexturePath();

protected:
  std::string texturePath;
  int movementCost;

private:
  std::pair<int, int> pos;
  std::shared_ptr<SBuilding> tileBuilding;
  std::vector<std::shared_ptr<SUnit>> m_presentUnits;
};
