#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <filesystem>
#include <vector>

class LensNote
{
    public:
    LensNote();

    bool Init();

    void CleanUp();

    void ProcessImage(std::vector<std::filesystem::path> ImageQueue);

    private:

    SDL_Surface* LoadSurfaceFrom(const char* filename);

};