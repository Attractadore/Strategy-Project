#pragma once

#include <memory>
#include <string>

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

protected:
  std::shared_ptr<SSprite> m_sprite;
};
