#pragma once

class SPlayer
{
  public:
  SPlayer();
  SPlayer(int p_playerId);
  SPlayer(const SPlayer&) = delete;
  SPlayer(SPlayer&&) = delete;
  ~SPlayer();

  SPlayer& operator=(const SPlayer&) = delete;
  SPlayer& operator=(SPlayer&& other);

  int getPlayerId();

  void addResources(int amount);
  bool hasResources(int amount);
  void removeResources(int amount);
  void setResources(int amount);
  int getCurrentManaAmount();

  private:
  int m_playerId;
  int m_playerManaAmount = 0;
};
