#pragma once

struct SDL_Texture;

class SBuilding
{
public:
    SBuilding();
    static void setBuildingTexture(SDL_Texture* newTexture);
    static SDL_Texture* getBuildingTexture();

protected:
    inline static SDL_Texture* buildingTexture;
};
