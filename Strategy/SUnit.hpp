#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class SNode;
struct SSprite;

class SUnit
{
  public:
  SUnit();
  SUnit(std::string p_unitId);
  SUnit(const SUnit& other);
  virtual ~SUnit();

  SUnit& operator=(const SUnit& other);

  int removeMoves(int numMoves);

  std::shared_ptr<SSprite> getSprite();
  std::shared_ptr<SSprite> getTeamColorSprite();
  void setSprite(std::shared_ptr<SSprite> newSprite);
  void setTeamColorSprite(std::shared_ptr<SSprite> newTeamColorSprite);
  void setOwner(int ownerId);
  int getOwner();

  int getCurrentHealth();
  bool bCanMove();
  void refresh();

  std::string m_unitId;
  int m_maxHP;
  int m_damage;
  float m_accuracy;
  int m_maxMoves;
  int m_buildTime;
  int m_resourceCost;

  protected:
  std::shared_ptr<SSprite> m_sprite;
  std::shared_ptr<SSprite> m_teamColorSprite;

  void copyStats(const SUnit& other);

  private:
  int m_currentHP;
  int m_currentMoves;
  int m_owningPlayerId;
};
