#pragma once

class SPlayer
{
public:
    SPlayer();
    SPlayer(int p_playerId);
    SPlayer(const SPlayer&) = delete;
    SPlayer(SPlayer&&) = delete;
    ~SPlayer();

    SPlayer& operator=(const SPlayer&) = delete ;
    SPlayer& operator=(SPlayer&& other);

    int getPlayerId();

    void addResources(int amount);
    bool hasResources(int amount);
    void removeResources(int amount);
    void setResources(int amount);

    void addSupply(int amount);
    void addMaxSupply(int amount);
    bool hasFreeSupply(int amount);
    void removeSupply(int amount);
    void setSupply(int amount);
    void setMaxSupply(int amount);


private:
    int m_playerId;
    int m_playerResources = 0;
    int m_playerSupply = 0;
    int m_playerMaxSupply = 0;
};
