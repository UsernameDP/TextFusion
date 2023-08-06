#pragma once
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace TextFusion
{
    class Settings
    {
    private:
        json jsonSettings;

    private:
        static std::unordered_map<std::string, json::value_t> requiredSettings;

    public:
        Settings(const std::string &watchDirectoryPath);

        json &get(const std::string &key);

    public:
        static inline void INIT()
        {
            requiredSettings["WatchDirectory"] = json::value_t::string;
            // The following needs to be added in a TextFusion.json
            requiredSettings["Extensions"] = json::value_t::array;
            requiredSettings["RelevantDirectoryNames"] = json::value_t::array;
            requiredSettings["FileToWrite"] = json::value_t::string;
            requiredSettings["WriteFormat"] = json::value_t::string;
            requiredSettings["WriteFormatEncapsulation"] = json::value_t::string;
            // Customization
            requiredSettings["WatchDirectoryDelay"] = json::value_t::number_unsigned;
            requiredSettings["FilesStateDelay"] = json::value_t::number_unsigned;
            requiredSettings["ConsoleDelay"] = json::value_t::number_unsigned;
        }
    };
}
