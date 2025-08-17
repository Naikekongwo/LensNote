#include "LensNote/LensNote.h"

LensNote::LensNote()
{
    // 构造函数
    Init();
}

bool LensNote::Init()
{
    if( SDL_Init(0) < 0)
    {
        SDL_Log("Graphics : Failed to init SDL.");
        return false;
    }

    if( TTF_Init() < 0)
    {
        SDL_Log("Graphics : Failed to init SDL_ttf.");
        return false;
    }

    SDL_Log("Graphics : successfully initialize SDL and SDL_ttf.");
    return true;
}

void LensNote::CleanUp()
{
    TTF_Quit();
    SDL_Quit();
    SDL_Log("Graphics : the SDL and SDL_ttf have been successfully destroyed.");
}

void LensNote::ProcessImage(std::vector<std::filesystem::path> ImageQueue)
{
    // 处理图像
    // 首先加载为表面
    std::vector<SDL_Surface*> Surfaces;

    for( auto &entry : ImageQueue)
    {
        // 加载表面
        Surfaces.push_back(LoadSurfaceFrom(entry.string().c_str()));
    }

    // 双重遍历
    for( int i = 0; i < Surfaces.size(); i++)
    {
        std::string filename = ImageQueue[i].stem().string();
        SDL_Log("Processing %s", ImageQueue[i].filename().c_str());

        
    }
}