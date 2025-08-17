#include "LensNote/LensNote.h"
#include <vector>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "STB-IMAGE/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB-IMAGE/stb_image_write.h"


bool LensNote::SaveSurfaceAsJPG(SDL_Surface* surface, const char* filename, int quality) {
    if (!surface || !surface->pixels) return false;

    SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB24, 0);
    if (!converted) return false;

    const int channels = 3;
    const int width  = converted->w;
    const int height = converted->h;

    if (SDL_MUSTLOCK(converted) && SDL_LockSurface(converted) != 0) {
        SDL_FreeSurface(converted);
        return false;
    }

    std::vector<unsigned char> buffer(static_cast<size_t>(width) * height * channels);
    unsigned char* srcPixels = static_cast<unsigned char*>(converted->pixels);

    for (int y = 0; y < height; ++y) {
        std::memcpy(buffer.data() + static_cast<size_t>(y) * width * channels,
                    srcPixels + y * converted->pitch,
                    static_cast<size_t>(width) * channels);
    }

    if (SDL_MUSTLOCK(converted)) SDL_UnlockSurface(converted);

    int ok = stbi_write_jpg(filename, width, height, channels, buffer.data(), quality);

    SDL_FreeSurface(converted);
    return ok != 0;
}

SDL_Surface* LensNote::LoadSurfaceFrom(const char* filename) {
    int w = 0, h = 0, channels = 0;
    unsigned char* data = stbi_load(filename, &w, &h, &channels, STBI_rgb_alpha);
    if (!data) {
        SDL_Log("LensNote: failed to load image: %s", filename);
        return nullptr;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        stbi_image_free(data);
        SDL_Log("LensNote: failed to create surface: %s", SDL_GetError());
        return nullptr;
    }

    if (SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0) {
        stbi_image_free(data);
        SDL_FreeSurface(surface);
        SDL_Log("LensNote: failed to lock surface: %s", SDL_GetError());
        return nullptr;
    }

    const int srcStride = w * 4; // STBI_rgb_alpha
    Uint8* dst = static_cast<Uint8*>(surface->pixels);
    for (int y = 0; y < h; ++y) {
        std::memcpy(dst + y * surface->pitch, data + y * srcStride, srcStride);
    }

    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    stbi_image_free(data);
    return surface;
}
