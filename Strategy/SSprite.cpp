#include "SSprite.hpp"

#include <cmath>

SSprite::SSprite(std::string p_texturePath, int p_numTiles, int p_size, int p_renderPriority){
    m_texturePath = p_texturePath;
    m_numTiles = p_numTiles;
    m_size = p_size;
    m_tileSize = m_size / std::sqrt(m_numTiles);
    m_renderPriority = p_renderPriority;
}

SSprite::SSprite(const SSprite &other){
    m_texturePath = other.m_texturePath;
    m_numTiles = other.m_numTiles;
    m_size = other.m_size;
    m_tileSize = other.m_tileSize;
    m_renderPriority = other.m_renderPriority;
}

//SSprite::SSprite(SSprite &&other){
//    m_texturePath = other.m_texturePath;
//    m_numTiles = other.m_numTiles;
//    m_size = other.m_size;
//    m_tileSize = other.m_tileSize;
//    m_renderPriority = other.m_renderPriority;
//}

SSprite::~SSprite(){

}

SSprite& SSprite::operator=(const SSprite& other){
    m_texturePath = other.m_texturePath;
    m_numTiles = other.m_numTiles;
    m_size = other.m_size;
    m_tileSize = other.m_tileSize;
    m_renderPriority = other.m_renderPriority;
    return *this;
}

//SSprite& SSprite::operator=(SSprite&& other){
//    m_texturePath = other.m_texturePath;
//    m_numTiles = other.m_numTiles;
//    m_size = other.m_size;
//    m_tileSize = other.m_tileSize;
//    m_renderPriority = other.m_renderPriority;
//}

//void SSprite::setAnimationFrame(int p_frame){
//    m_animationFrame = p_frame % m_numTiles;
//}

//int SSprite::getAnimzationFrame(){
//    return m_animationFrame;
//}
