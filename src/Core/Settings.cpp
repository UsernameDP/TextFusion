#include "pch.hpp"
#include "Settings.hpp"

namespace TextFusion {
	Settings::Settings(const std::string& watchDirectoryPath) {
        fs::path settingsPath = static_cast<fs::path>(watchDirectoryPath) / "TextFusion.json";
        if (!exd::fileExists(settingsPath.string()))
        {
            THROW_RUNTIME_ERROR("Make sure to create TextFusion.json with all settings in " + watchDirectoryPath);
        }

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

        LOG_CONSTRUCTOR("TextFusion::Settings");
	}
    json& Settings::get(const std::string& key) {
        if (jsonSettings[key].is_null())
            THROW_RUNTIME_ERROR(key + " is not defined in jsonSettings");
           
        return jsonSettings[key];
    }

	std::unordered_map<std::string, json::value_t> Settings::requiredSettings;
}