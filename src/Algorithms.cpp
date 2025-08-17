#include "LensNote.h"

#define STB_IMAGE_IMPLEMENTATION
#include <STB-IMAGE/stb_image.h>

#include <cstring>

SDL_Surface* LensNote::LoadSurfaceFrom(const char* filename)
{

    int w = 0, h = 0, channels = 0;

    unsigned char* data = stbi_load(filename, &w, &h, &channels, STBI_rgb_alpha);

    if(!data)
    {
        SDL_Log("LensNote: encountered a empty data.");
        return nullptr;
    }
    
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);

    if(!surface)
    {
        stbi_image_free(data);
        SDL_Log("Failed to create surface.");
        return nullptr;
    }
    
    const int srcStride = w * 4;
    Uint8* dst = static_cast<Uint8*>(surface->pixels);
    for (int y = 0; y < h; ++y) {
        std::memcpy(dst + y * surface->pitch, data + y * srcStride, srcStride);
    }

    stbi_image_free(data);

    return surface;
}