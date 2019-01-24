#include "SNode.hpp"

#include <SDL2/SDL.h>

#include <algorithm>

SNode::SNode(int x, int y) {
  this->presentUnits = {};
  this->pos = std::make_pair(x, y);
}

SNode::~SNode() {}

std::pair<int, int> SNode::getPos() { return this->pos; }

std::shared_ptr<SBuilding> SNode::getTileBuilding() {
  return this->tileBuilding;
}

std::list<std::shared_ptr<SUnit>> SNode::getTileUnits(){
    return this->presentUnits;
}

void SNode::setTileBuilding(std::shared_ptr<SBuilding> newBuilding) {
  this->tileBuilding = newBuilding;
}

void SNode::addUnitToTile(std::shared_ptr<SUnit> unit){
    this->presentUnits.push_back(unit);
}

std::shared_ptr<SUnit> SNode::removeUnitFromTile(std::shared_ptr<SUnit> unit){
    auto it = std::find(this->presentUnits.begin(), this->presentUnits.end(), unit);
    if (it == presentUnits.end()){
        return nullptr;
    }
    return *it;
}

void SNode::setNodeTexture(SDL_Texture *newTexture) {
  SNode::nodeTexture = newTexture;
}

SDL_Texture *SNode::getNodeTexture() { return SNode::nodeTexture; }
