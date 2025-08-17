#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <rapidjson/document.h>
#include <easyexif/exif.h>

#include <iostream>
#include <string>
#include <fstream>
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

    bool SaveSurfaceAsJPG(SDL_Surface* surface, const char* filename, int quality);
    

};