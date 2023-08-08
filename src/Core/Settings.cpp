#include "pch.hpp"
#include "Settings.hpp"

namespace TextFusion {
	Settings::Settings(const std::string& watchDirectoryPath) {
        std::vector<std::string> files;
        std::vector<std::string> names = { "TextFusion.json" };
        exd::getAllFilesWithNames(files, watchDirectoryPath, names, true);
        if (files.size() > 1) {
            THROW_RUNTIME_ERROR("There can only be 1 TextFusion.json");
        }else if (files.empty())
        {
            THROW_RUNTIME_ERROR("Make sure to create TextFusion.json with all settings in " + watchDirectoryPath);
        }

        fs::path settingsPath = files.front();

        std::ifstream settingsFile(settingsPath);
        jsonSettings = json::parse(settingsFile);

        jsonSettings["WatchDirectory"] = watchDirectoryPath;

        for (const auto& pair : requiredSettings)
        {
            if (jsonSettings[pair.first].is_null() ||
                jsonSettings[pair.first].type() != pair.second)
            {
                std::string message = pair.first + " needs to be defined with the correct data type";
                THROW_RUNTIME_ERROR(message);
            }
        }
        /*Extra initialization*/

        //if "WriteFormatEncapsulation" is a path, then readFrom it and overwrite
        if (exd::fileExists(get("WriteFormatEncapsulation"))) {
            jsonSettings["WriteFormatEncapsulation"] = exd::readFile(get("WriteFormatEncapsulation"));
        }


        LOG_CONSTRUCTOR("TextFusion::Settings");
	}
    const json& Settings::get(const std::string& key) {
        if (jsonSettings[key].is_null())
            THROW_RUNTIME_ERROR(key + " is not defined in jsonSettings");
           
        return jsonSettings[key];
    }

	std::unordered_map<std::string, json::value_t> Settings::requiredSettings;
}