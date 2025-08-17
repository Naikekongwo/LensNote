#include "LensNote/LensNote.h"

struct JSONDATA
{
    double DOC_BorderWidth = 0;
    double DOC_ExtraHeight = 0;

    double LOGO_Height = 0;
    double LOGO_PaddingTop = 0;

    double Text_Height = 0;
    double Text_PaddingTop = 0;
    std::string Text_Content;
};

bool ReplaceItem(std::string &text, std::string orgin, std::string targ)
{
    size_t pos = text.find(orgin);
    if (pos != std::string::npos) {
        text.replace(pos, orgin.size(), targ);
        return true;
    }

    return false;
}


JSONDATA ParsingJSON(rapidjson::Document &doc)
{
    using namespace rapidjson;

    JSONDATA DATA;

    if (doc.HasMember("Documentation"))
        {
            const Value &docu = doc["Documentation"];
            if (docu.HasMember("BorderWidth"))
                DATA.DOC_BorderWidth = docu["BorderWidth"].GetDouble();
            if (docu.HasMember("ExtraHeight"))
                DATA.DOC_ExtraHeight = docu["ExtraHeight"].GetDouble();
        }

        // --- 5. 读取 Infos ---
        if (doc.HasMember("Infos"))
        {
            const Value &infos = doc["Infos"];

            // Logo
            if (infos.HasMember("Logo"))
            {
                const Value &logo = infos["Logo"];
                if (logo.HasMember("Height"))
                    DATA.LOGO_Height = logo["Height"].GetDouble();
                if (logo.HasMember("PaddingTop"))
                    DATA.LOGO_PaddingTop = logo["PaddingTop"].GetDouble();
            }

            // Text
            if (infos.HasMember("Text"))
            {
                const Value &text = infos["Text"];
                if (text.HasMember("Content"))
                    DATA.Text_Content = text["Content"].GetString();
                if (text.HasMember("Height"))
                    DATA.Text_Height = text["Height"].GetDouble();
                if (text.HasMember("PaddingTop"))
                    DATA.Text_PaddingTop = text["PaddingTop"].GetDouble();
            }
        }
    return DATA;
}

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
    // 加载LOGO文件
    SDL_Surface* LOGO = LoadSurfaceFrom("Panasonic.png");
    // 加载字体文件
    TTF_Font* font = TTF_OpenFont("standard.ttf", 24);
    if(!font)
    {
        SDL_Log("Failed to laod font.");
        return;
    }

    // 接着加载JSON文件
    rapidjson::Document portrait;
    rapidjson::Document landscape;

    std::ifstream land("landscape.json");

    if(!land.is_open())
    {
        SDL_Log("Failed to openn landscape xml.");
        return;
    }

    std::stringstream landscape_buffer;
    landscape_buffer << land.rdbuf();
    std::string landscape_target_buffer = landscape_buffer.str();

    if(landscape.Parse(landscape_target_buffer.c_str()).HasParseError())
    {
        SDL_Log("Encountered a fatal error in parsing json.");
        return;
    }

    std::ifstream port("portrait.json");

    if(!port.is_open())
    {
        SDL_Log("Failed to open portrait xml.");
        return;
    }

    std::stringstream portrait_buffer;
    portrait_buffer << port.rdbuf();
    std::string portrait_target_buffer = portrait_buffer.str();

    if(portrait.Parse(portrait_target_buffer.c_str()).HasParseError())
    {
        SDL_Log("Encountered a fatal error in parsing json.");
        return;
    }

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
        if(!Surfaces[i]) continue;

        std::string filename = ImageQueue[i].stem().string();
        SDL_Log("Processing %s", ImageQueue[i].filename().c_str());

        FILE *fp = fopen(ImageQueue[i].string().c_str(), "rb");

        if(!fp)
        {
            SDL_Log("Failed to check image.");
            break;
        }

        fseek(fp, 0, SEEK_END);
        unsigned long fsize = ftell(fp);
        
        rewind(fp);

        unsigned char * buf = new unsigned char[fsize];

        if(fread(buf, 1, fsize, fp) != fsize)
        {
            SDL_Log("Failed to read image exif.");
            delete[] buf;
            break;
        }

        fclose(fp);

        // 开始解析 EXIF

        

        easyexif::EXIFInfo result;

        int code = result.parseFrom(buf, fsize);

        delete[] buf;

        if(code) {
            SDL_Log("Failed to parse EXIF.");
            break;
        }

        // 此时result已经完全装载了信息
        int width = Surfaces[i]->w;
        int height = Surfaces[i]->h;

        // 判断照片是横幅还是竖幅
        bool isLandscape = (width > height)?true:false;

        JSONDATA DATA;

        if(isLandscape)
        {
            DATA = ParsingJSON(landscape);
        }
        else
        {
            DATA = ParsingJSON(portrait);
        }

        SDL_Surface* Canvas = SDL_CreateRGBSurfaceWithFormat(0, width * (1 + DATA.DOC_BorderWidth), height + width*(DATA.DOC_BorderWidth + DATA.DOC_ExtraHeight), 32, SDL_PIXELFORMAT_RGBA32);
        
        if(!Canvas)
        {
            SDL_Log("Failed to create Canvas.");
            break;
        }

        SDL_FillRect(Canvas, nullptr, SDL_MapRGBA(Canvas->format, 255, 255,255,255));
        // 将画布变成白色

        SDL_Rect dstRect;

        // 照片本身
        dstRect.x = 0.5 * width * DATA.DOC_BorderWidth;
        dstRect.y = dstRect.x;
        dstRect.w = width;
        dstRect.h = height;

        SDL_BlitSurface(Surfaces[i], NULL, Canvas, &dstRect);

        // LOGO 本身
        dstRect.h = DATA.LOGO_Height * width;
        int WN = static_cast<int>(static_cast<float>(LOGO->w) / LOGO->h * dstRect.h + 0.5f);
        dstRect.w = WN;
        dstRect.x = (Canvas->w - dstRect.w) / 2;
        dstRect.y+= Surfaces[i]->h + width * DATA.LOGO_PaddingTop;
        int Gap = Canvas->w - dstRect.w;

        SDL_BlitSurface(LOGO, NULL, Canvas, &dstRect);

        // 文字替换逻辑
        std::ostringstream oss;
        if (result.ExposureTime > 0.0) {
            if (result.ExposureTime < 1.0) {
                // 小于1秒的快门，显示为 1/XXX s
                int denom = static_cast<int>(1.0 / result.ExposureTime + 0.5); // 四舍五入
                oss << "1/" << denom << "s";
            } else {
                // >=1秒的快门，直接显示秒数
                oss << std::fixed << std::setprecision(1) << result.ExposureTime << "s";
            }
        } else {
            oss << "Unknown";
        }
        
        ReplaceItem(DATA.Text_Content,"{ExTime}", oss.str());

        std::ostringstream fstop;
        fstop<<std::fixed<<std::setprecision(1);
        if(result.FNumber == 0.0)
        {
            fstop<<"Manual Lens";
        }
        else
        {
            fstop << "F" << result.FNumber<<" ";
        }
        ReplaceItem(DATA.Text_Content, "{FStop}", fstop.str());

        std::ostringstream iso;
        iso << "ISO " << result.ISOSpeedRatings;
        ReplaceItem(DATA.Text_Content, "{ISO}", iso.str());

        while(ReplaceItem(DATA.Text_Content, "  ", " "));

        // 文字本
        SDL_Color textColor = {150, 150, 150, 255};
        std::cout<<"文字大小当前为: "<<DATA.Text_Height*width<<std::endl;
        TTF_SetFontSize(font, static_cast<int>(DATA.Text_Height*width));
        SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, DATA.Text_Content.c_str(), textColor);

        dstRect.y+= dstRect.h + width * DATA.Text_PaddingTop;
        dstRect.h = textSurface->h;
        dstRect.w = textSurface ->w;
        dstRect.x = 0.5 * (Canvas->w - dstRect.w);

        SDL_BlitSurface(textSurface, NULL, Canvas, &dstRect);

        std::string files = "MARK_" + filename + ".jpg";

        SaveSurfaceAsJPG(Canvas, files.c_str(), 100);

        SDL_FreeSurface(textSurface);

        SDL_FreeSurface(Canvas);
    }

    for(auto& entry : Surfaces)
    {
        SDL_FreeSurface(entry);
    }

    SDL_FreeSurface(LOGO);

    TTF_CloseFont(font);
}