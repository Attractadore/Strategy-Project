#pragma once

#include "SUnit.hpp"

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

struct SSprite;

class SBuilding {
public:
  SBuilding();
  SBuilding(const SBuilding &other);
  SBuilding(SBuilding &&other);
  ~SBuilding();

  SBuilding &operator=(const SBuilding &other);
  SBuilding &operator=(SBuilding &&other);

  std::shared_ptr<SSprite> getSprite();
  void setSprite(std::shared_ptr<SSprite> newSprite);
  void
  setUnitLookUpTable(std::unordered_map<std::string, SUnit> &p_unitLookUpTable);
  void setParams(std::unordered_map<std::string, float> buildingParams);

  int getResourceGatherRate();
  int getSupplyProvided();

  void addUnitToBuildQueue(std::string unitId);
  void removeUnitFromBuildQueue(int index);
  void removeLastUnitFromBuildQueue();

  bool finisingBuilding();
  std::string unitUnderConstruction();
  bool isBuilding();

  void refresh();

  void setOwner(int ownerId);
  int getOwner();

protected:
  std::shared_ptr<SSprite> m_sprite;
  std::unordered_map<std::string, SUnit> m_unitLookUpTable;

  int m_resourceGatherRate;
  int m_supplyProvided;

private:
  int m_owningPlayerId;
  std::list<std::string> m_buildQueue;
  int m_numTurnsBuilding;
};
