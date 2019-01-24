#pragma once

#include <memory>

struct SDL_Texture;

class SUnit {
public:
    SUnit();
    virtual ~SUnit() = 0;

    virtual bool isDead();
    virtual int restoreMoves();
    virtual int applyDamage(int amount);
    virtual int dealDamage(float chance);

    static SDL_Texture* getTexture();
    static void setTexture(SDL_Texture* texture);

protected:
    int currentHP;
    int currentMoves;

    inline static int maxHP;
    inline static int damage;
    inline static float accuracy;
    inline static int maxMoves;

    inline static SDL_Texture* texture;
};

inline SUnit::~SUnit() {}

class SSpearman : SUnit {
public:
        SSpearman();
        ~SSpearman();
};
