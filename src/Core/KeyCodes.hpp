#pragma once
#include "Files.hpp"
#include "Settings.hpp"

namespace TextFusion {
	class KeyCode {
	private:
		std::unordered_map<std::string, std::string> codes;
	public:
		inline void set(const std::string& key, const std::string& keyValue) {
			codes[key] = keyValue;
		}
		inline const std::string& get(const std::string& key) {
			if (codes.contains(key)) {
				return codes[key];
			}
			return "";
		}
			 
		inline std::unordered_map<std::string, std::string>::iterator begin() {
			return codes.begin();
		}
		inline std::unordered_map<std::string, std::string>::iterator end() {
			return codes.end();
		}
	};

	
	class FileCodes : public KeyCode {
	public:
		FileCodes(const TextFile& textFile);
	};

	class SettingsCodes : public KeyCode {
	public:
		SettingsCodes(Settings& settings);
	};
}