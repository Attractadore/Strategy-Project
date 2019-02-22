#include "SBuilding.hpp"

#include <iostream>

SBuilding::SBuilding()
{
  m_numTurnsBuilding = 0;
  m_currentConstructionTurns = 0;
}

SBuilding::SBuilding(BUILDING_ID buildingID)
: SBuilding()
{
  m_ID = buildingID;
}

SBuilding::SBuilding(const SBuilding& other)
{
  copyStats(other);
  m_cHealth = m_health / float(m_buildTime + 1);
  m_numTurnsBuilding = 0;
  m_currentConstructionTurns = 0;
}

SBuilding& SBuilding::operator=(const SBuilding& other)
{
  copyStats(other);
  m_cHealth = m_health / float(m_buildTime + 1);
  m_numTurnsBuilding = 0;
  m_currentConstructionTurns = 0;

  return *this;
}

void SBuilding::copyStats(const SBuilding& other)
{
  SCombatReady::copyStats(other);
  m_unitLookUpTable = other.m_unitLookUpTable;
  m_resourceGatherRate = other.m_resourceGatherRate;
  m_buildableUnits = other.m_buildableUnits;
  m_numTurnsBuilding = 0;
  m_currentConstructionTurns = 0;
}

void SBuilding::setUnitLookUpTable(
std::unordered_map<std::string, SUnit>* p_unitLookUpTable)
{
  m_unitLookUpTable = p_unitLookUpTable;
}

bool SBuilding::finisingBuilding()
{
  if (m_buildQueue.empty())
  {
    return false;
  }

  return m_numTurnsBuilding ==
         (*m_unitLookUpTable)[m_buildQueue.front()].m_buildTime - 1;
}

std::string SBuilding::unitUnderConstruction()
{
  if (m_buildQueue.empty())
  {
    return "";
  }
  return m_buildQueue.front();
}

std::string SBuilding::removeUnitFromBuildQueue(int index)
{
  if (index >= m_buildQueue.size())
  {
    return "";
  }
  auto it = std::next(m_buildQueue.begin(), index);
  std::string unitUnderConstruction = *it;
  m_buildQueue.erase(it);
  return unitUnderConstruction;
}

const std::list<std::string>& SBuilding::getBuildQueue() const
{
  return m_buildQueue;
}

bool SBuilding::isBuilding()
{
  return !m_buildQueue.empty();
}

bool SBuilding::bUnderConstruction()
{
  return m_currentConstructionTurns < m_buildTime;
}

void SBuilding::refresh()
{
  if (bUnderConstruction())
  {
    m_currentConstructionTurns++;
    m_cHealth += m_health / float(m_buildTime + 1);
  }
  if (!isBuilding())
  {
    return;
  }
  std::cout << "Building " << m_buildQueue.front() << " for "
            << m_numTurnsBuilding << " turns" << std::endl;
  m_numTurnsBuilding++;
  if (m_numTurnsBuilding ==
      (*m_unitLookUpTable)[m_buildQueue.front()].m_buildTime)
  {
    m_buildQueue.pop_front();
    m_numTurnsBuilding = 0;
  }
}

void SBuilding::addUnitToBuildQueue(std::string unitId)
{
  if (m_buildableUnits.count(unitId))
  {
    m_buildQueue.push_back(unitId);
  }
}

void SBuilding::finishConstruction()
{
  m_cHealth += m_health * (m_buildTime - m_currentConstructionTurns) / float(m_buildTime + 1);
  m_currentConstructionTurns = m_buildTime;
}

bool SBuilding::bCanTrainUnit(std::string unit)
{
  return m_buildableUnits.count(unit);
}
