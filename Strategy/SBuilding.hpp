#pragma once

#include <string>

class SBuilding {
public:
  SBuilding();
  SBuilding(const SBuilding &other);
  SBuilding(SBuilding &&other);
  ~SBuilding();

  SBuilding &operator=(const SBuilding &other);
  SBuilding &operator=(SBuilding &&other);

  void setTexturePath(const std::string &newTexturePath);
  std::string &getTexturePath();

protected:
  std::string texturePath;
};
