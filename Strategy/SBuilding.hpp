#pragma once

#include "SCombatReady.hpp"
#include "SUnit.hpp"

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

using BUILDING_ID = std::string;

class SBuilding : public SCombatReady
{
  public:
  SBuilding();
  SBuilding(BUILDING_ID buildingID);
  SBuilding(const SBuilding& other);

  SBuilding& operator=(const SBuilding& other);

  virtual void copyStats(const SBuilding& other);

  void setUnitLookUpTable(std::unordered_map<std::string, SUnit>* p_unitLookUpTable);

  void addUnitToBuildQueue(std::string unitId);
  std::string removeUnitFromBuildQueue(int index);
  std::string removeLastUnitFromBuildQueue();

  bool finisingBuilding();
  std::string unitUnderConstruction();
  const std::list<std::string>& getBuildQueue() const;
  bool isBuilding();
  bool bUnderConstruction();
  void finishConstruction();

  virtual void refresh() override;

  virtual bool bCanMove() override;
  virtual int removeMoves(int) override;
  virtual int getMoves() override;

  bool bCanTrainUnit(std::string unit);

  std::unordered_set<UNIT_ID> m_buildableUnits;
  int m_resourceGatherRate;


  protected:
  std::unordered_map<std::string, SUnit>* m_unitLookUpTable;

  std::list<std::string> m_buildQueue;
  int m_numTurnsBuilding;
  int m_currentConstructionTurns;
};
