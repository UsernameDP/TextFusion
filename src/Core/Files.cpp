#include "pch.hpp"
#include "Files.hpp"

namespace TextFusion {
	TextFile::TextFile(const std::string& path, const std::string& parent) {
        this->parent = static_cast<fs::path>(parent);
        this->path = static_cast<fs::path>(path);
		this->content = exd::readFile(path);
		this->exists = true;
		this->updated = false; // first time CompareFileTime will always say updated = true
        LOG_CONSTRUCTOR("TextFusion::TextFile path=" + this->path.string());
	}
	TextFile::~TextFile() {
		LOG_DESTRUCTOR("TextFusion::TextFile path=" + this->path.string());
	}
	bool TextFile::updateAttributes() {
        if (!GetFileAttributesExW(path.wstring().c_str(), GetFileExInfoStandard, &attributes))
        {
            DWORD errorCode = GetLastError();
            
            if (errorCode == ERROR_PATH_NOT_FOUND || errorCode == ERROR_FILE_NOT_FOUND) {
                exists = false;
            }

            return false;
        }
        currentState = attributes.ftLastWriteTime;
        if (CompareFileTime(&previousState, &currentState) != 0)
        { // they aren't the same (i.e updated)
            previousState = currentState;
            updated = true;

            auto currentTime = std::chrono::system_clock::now();
            std::time_t currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
            localtime_s(&lastUpdateTime, &currentTimeT);
            
        }
        return true; // for no errors
	}
    void TextFile::updateContent() {
        this->content = exd::readFile(this->path.string());
    }
}