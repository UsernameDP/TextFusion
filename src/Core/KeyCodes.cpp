#include "pch.hpp"
#include "KeyCodes.hpp"

namespace TextFusion {
	//FileCodes
	FileCodes::FileCodes(const TextFile& textFile) {
		set("${Path}", exd::getReplaceAll(textFile.path.string(), "\\", "\\\\"));
		
		set("${RelativePath}", exd::getReplaceAll(
			exd::getRelative(textFile.path.string(), textFile.parent.string()), 
			"\\", "\\\\"));
		set("${Content}", exd::getReplaceAll(
			exd::trim(textFile.content), 
			"\n", "\\n\"\n\""));

	}
	//SettingsCodes
	SettingsCodes::SettingsCodes(Settings& settings) {
		for (const auto& pair : Settings::requiredSettings) {
			if (pair.second == json::value_t::string) {
				set("${" + pair.first + "}", settings.get(pair.first));
			}
		}
	}
}