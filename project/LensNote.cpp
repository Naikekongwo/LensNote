#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <cctype>
#include <vector>


#include "LensNote/LensNote.h"


bool isSupportedImage(const std::filesystem::path &p) {
    std::string ext = p.extension().string();
    for (auto &c : ext) c = std::tolower(static_cast<unsigned char>(c));
    return (ext == ".jpg" || ext == ".jpeg" || ext == ".png");
}


int main(int argc, char *args[])
{
    system("chcp 65001");
    
    using namespace std;
    using namespace std::filesystem;

    

    if( argc < 2 )
    {
        cout << "请将图片或者图片目录拖到应用上运行。"<<std::endl;
        
        return EXIT_FAILURE;
    }

    std::vector<path> ProcessQueue;

    for( int i = 1; i < argc ; i++)
    {
        // 遍历输入的所有参数
        path curFile(args[i]);

        if(!exists(curFile))
        {
            cout<< "图片不存在 : " << curFile.filename().string() <<endl;
            continue;
        }

        if(is_regular_file(curFile))
        {
            // 判断部分
            if( isSupportedImage(curFile) )
            {
                ProcessQueue.push_back(curFile);
            }
        }
        else if (is_directory(curFile))
        {
            for( auto &entry : directory_iterator(curFile))
            {
                if(is_regular_file(entry))
                {
                    // 判断部分
                    if( isSupportedImage(entry.path()) )
                    {
                        ProcessQueue.push_back(entry.path());
                    }
                }
            }
        }
        else
        {
            cout << " 未知路径 : " << curFile.string() << endl;
            continue;
        }
    }

    if (ProcessQueue.empty())
    {
        cout<<"未搜索到有效的文件。"<<endl;
    }
    else
    {
        cout<<"接下来将会处理的文件有:"<<endl;
        for ( auto &entry : ProcessQueue)
        {
            cout<< entry.string() <<endl;
        }
    }

    

    LensNote lensNote;

    lensNote.Init();

    lensNote.ProcessImage(ProcessQueue);

    
    // 成功退出
    return EXIT_SUCCESS;
}