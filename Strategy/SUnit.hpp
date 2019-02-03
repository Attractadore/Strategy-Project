#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class SNode;
struct SSprite;

class SUnit {
public:
  SUnit();
  SUnit(std::string p_unitId);
  SUnit(const SUnit &other);
  virtual ~SUnit();

  void copyStats(const SUnit &other);

  SUnit &operator=(const SUnit &other);

  int removeMoves(int numMoves);
  virtual bool isDead();
  virtual int applyDamage(int amount);
  virtual int dealDamage(float chance);

  std::shared_ptr<SSprite> getSprite();
  void setSprite(std::shared_ptr<SSprite> newSprite);
  void setOwner(int ownerId);
  int getOwner();
  std::string getUnitId();
  int getBuildTime();
  int getSupplyCost();
  int getResourceCost();

  void setParams(std::unordered_map<std::string, float> params);

  //  void setMaxHP(int p_maxHP);
  //  void setDamage(int p_damage);
  //  void setAccuracy(int p_accuracy);
  //  void setMaxMoves(int p_maxMoves);

  //  void setRoute(std::vector<std::shared_ptr<SNode>> newRoute);
  void setTargetTile(std::shared_ptr<SNode> newTargetTile);
  void resetTargetTile();
  void setCurrentTile(std::shared_ptr<SNode> newCurrentTile);
  std::shared_ptr<SNode> getTargetTile();
  std::shared_ptr<SNode> getCurrentTile();
  void moveTile(std::shared_ptr<SNode> tile);
  bool canAdvanceRoute();
  bool finishedRoute();
  void refresh();

protected:
  std::string m_unitId;
  int m_maxHP;
  int m_damage;
  float m_accuracy;
  int m_maxMoves;
  int m_supplyCost;
  int m_buildTime;
  int m_resourceCost;
  int m_size;

  std::shared_ptr<SSprite> m_sprite;

private:
  int m_currentHP;
  int m_currentMoves;
  int m_owningPlayerId;

  //  std::vector<std::shared_ptr<SNode>> m_route;
  std::shared_ptr<SNode> m_currentTile;
  std::shared_ptr<SNode> m_targetTile;
};
