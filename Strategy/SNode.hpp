#pragma once

#include <glm/vec2.hpp>

#include <list>
#include <memory>

struct SDL_Texture;

class SUnit;
class SBuilding;

class SNode {
public:
  SNode(int x, int y);
  ~SNode();

  std::pair<int, int> getPos();
  std::shared_ptr<SBuilding> getTileBuilding();
  std::list<std::shared_ptr<SUnit>> getTileUnits();

  std::list<std::shared_ptr<SUnit>> removeUnitsFromTile(std::list<std::shared_ptr<SUnit>> units);
  std::shared_ptr<SUnit> removeUnitFromTile(std::shared_ptr<SUnit> unit);

  void addUnitToTile(std::shared_ptr<SUnit> unit);

  void setTileBuilding(std::shared_ptr<SBuilding> newBuilding);

  static void setNodeTexture(SDL_Texture *newTexture);
  static struct SDL_Texture *getNodeTexture();

private:
  std::pair<int, int> pos;

  inline static SDL_Texture *nodeTexture;
  inline static int movementCost;

  std::shared_ptr<SBuilding> tileBuilding;
  std::list<std::shared_ptr<SUnit>> presentUnits;
};
