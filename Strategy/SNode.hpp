#pragma once

#include <glm/vec2.hpp>

#include <memory>
#include <vector>

class SUnit;
class SBuilding;
struct SSprite;

class SNode
{
  public:
  SNode();
  SNode(const SNode& other);
  SNode(SNode&& other);
  SNode(int x, int y);
  ~SNode();

  SNode& operator=(const SNode& other);
  SNode& operator=(SNode&& other);

  std::pair<int, int> getPos();
  int getMovementCost();
  std::shared_ptr<SBuilding> getTileBuilding();
  std::vector<std::shared_ptr<SUnit>> getTileUnits();

  void setPos(int x, int y);
  void setPos(const std::pair<int, int>& newPos);
  void setMovementCost(int newMovementCost);
  void addGeyser();
  void addFoundation();
  bool bHasFoundation();
  void addMana(int amount);
  int getAndRemoveMana();

  void addUnitToTile(std::shared_ptr<SUnit> unit);

  void setTileBuilding(std::shared_ptr<SBuilding> newBuilding);

  void setSprite(std::shared_ptr<SSprite> newSprite);
  std::shared_ptr<SSprite> getSprite();

  int getCurrentMana();
  bool bHasGeyser();
  bool bHasMana();

  protected:
  std::shared_ptr<SSprite> m_sprite;
  int movementCost;

  private:
  std::pair<int, int> pos;
  std::shared_ptr<SBuilding> tileBuilding;
  std::vector<std::shared_ptr<SUnit>> m_presentUnits;
  int m_currentMana = 0;
  bool m_bHasGeyser = false;
  bool m_bHasFoundation = false;
};
