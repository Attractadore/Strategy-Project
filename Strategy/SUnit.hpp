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

  void setParams(std::unordered_map<std::string, float> params);
  void setTargetTile(std::shared_ptr<SNode> newTargetTile);
  void resetTargetTile();
  void setCurrentTile(std::shared_ptr<SNode> newCurrentTile);
  std::shared_ptr<SNode> getTargetTile();
  std::shared_ptr<SNode> getCurrentTile();
  void moveTile(std::shared_ptr<SNode> tile);
  bool canAdvanceRoute();
  bool finishedRoute();
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

private:
  int m_currentHP;
  int m_currentMoves;
  int m_owningPlayerId;

  std::shared_ptr<SNode> m_currentTile;
  std::shared_ptr<SNode> m_targetTile;
};
