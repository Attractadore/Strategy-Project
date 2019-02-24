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
  SNode(int x, int y);
  ~SNode();

  SNode& operator=(const SNode& other);

  std::pair<int, int> getPos();

  void setPos(int x, int y);
  void setPos(const std::pair<int, int>& newPos);
  void addGeyser();
  void addFoundation();
  bool bHasFoundation();
  void addMana(int amount);
  int getAndRemoveMana();

  void setSprite(std::shared_ptr<SSprite> newSprite);
  std::shared_ptr<SSprite> getSprite();

  int getCurrentMana();
  bool bHasGeyser();
  bool bHasMana();

  int m_movementCost = 2;
  int m_armorModifier = 0;
  float m_accuracyModifier = 0.0f;
  bool bPassable = true;

  protected:
  std::shared_ptr<SSprite> m_sprite;
  //  int movementCost;

  private:
  std::pair<int, int> pos;
  int m_currentMana = 0;
  bool m_bHasGeyser = false;
  bool m_bHasFoundation = false;
};
