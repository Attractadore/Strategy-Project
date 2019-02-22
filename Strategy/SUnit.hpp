#pragma once

#include "SCombatReady.hpp"

#define PROMOTION_XP 100
using UNIT_ID = std::string;

class SUnit : public SCombatReady
{
  public:
  SUnit();
  SUnit(UNIT_ID unitID);
  SUnit(const SUnit& other);

  SUnit& operator=(const SUnit& other);

  void setPromotionUnit(SUnit* newPromotionUnit);

  virtual void copyStats(const SUnit& other);

  void addXP(int amount);
  int getXP();

  protected:
  SUnit* m_promotionUnit;
  void promote();

  int m_cXP;
};
