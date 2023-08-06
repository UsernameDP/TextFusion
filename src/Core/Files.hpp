#pragma once
#include <Windows.h>

namespace TextFusion {
    struct TextFile
    {
        fs::path path;
        std::string content;
        FILETIME previousState;
        FILETIME currentState;
        WIN32_FILE_ATTRIBUTE_DATA attributes;

        std::tm lastUpdateTime;

        bool updated;
        bool exists;

        TextFile(const std::string& path);
        ~TextFile();

        bool updateAttributes();
        void updateContent();
    };
}