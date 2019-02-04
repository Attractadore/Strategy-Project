#include "SPlayer.hpp"

//#include <algorithm>

SPlayer::SPlayer() {}

SPlayer::SPlayer(int p_playerId) { m_playerId = p_playerId; }

SPlayer::~SPlayer() {}

SPlayer &SPlayer::operator=(SPlayer &&other) {
  m_playerId = other.m_playerId;
  return *this;
}

int SPlayer::getPlayerId() { return m_playerId; }

void SPlayer::addResources(int amount) { m_playerResources += amount; }

bool SPlayer::hasResources(int amount) { return m_playerResources >= amount; }

void SPlayer::removeResources(int amount) { m_playerResources -= amount; }

void SPlayer::setResources(int amount) { m_playerResources = amount; }
