#pragma once

#include "SUnit.hpp"

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct SSprite;

class SBuilding {
public:
  SBuilding();
  SBuilding(const SBuilding &other);
  //  SBuilding(SBuilding &&other);
  //  ~SBuilding();

  SBuilding &operator=(const SBuilding &other);
  //  SBuilding &operator=(SBuilding &&other);

  std::shared_ptr<SSprite> getSprite();
  std::shared_ptr<SSprite> getuiIcon();
  void setSprite(std::shared_ptr<SSprite> newSprite);
  void setuiIcon(std::shared_ptr<SSprite> newSprite);
  void
  setUnitLookUpTable(std::unordered_map<std::string, SUnit> *p_unitLookUpTable);
  void setParams(std::unordered_map<std::string, float> buildingParams);

  void addUnitToBuildQueue(std::string unitId);
  std::string removeUnitFromBuildQueue(int index);
  std::string removeLastUnitFromBuildQueue();

  bool finisingBuilding();
  std::string unitUnderConstruction();
  const std::list<std::string> &getBuildQueue() const;
  bool isBuilding();
  bool bUnderConstruction();
  void finishConstruction();
  void resetConstruction();

  void refresh();

  void setOwner(int ownerId);
  int getOwner();

  bool bCanTrainUnit(std::string unit);

  int m_armour;
  int m_maxHealth;
  int m_resourceGatherRate;
  int m_resourceCost;
  int m_constructionTime;
  std::unordered_set<std::string> m_buildableUnits;

protected:
  std::shared_ptr<SSprite> m_sprite;
  std::shared_ptr<SSprite> m_uiIconSprite;
  std::unordered_map<std::string, SUnit> *m_unitLookUpTable;

private:
  float m_currentHealth;
  int m_owningPlayerId;
  std::list<std::string> m_buildQueue;
  int m_numTurnsBuilding;
  int m_currentConstructionTurns;
};
