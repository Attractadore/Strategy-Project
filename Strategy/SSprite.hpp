#pragma once

#include <string>

struct SSprite {
  SSprite(std::string p_texturePath, int p_numTiles, int p_size,
          int p_renderPriority);
  SSprite(const SSprite &other);
  //    SSprite(SSprite&& other);

  ~SSprite();

  SSprite &operator=(const SSprite &other);
  //    SSprite& operator=(SSprite&& other);

  std::string m_texturePath;
  int m_numTiles;
  int m_numTilesAxis;
  int m_size;
  int m_tileSize;
  int m_renderPriority;
};
