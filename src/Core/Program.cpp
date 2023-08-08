#include "pch.hpp"
#include "Program.hpp"
#include <conio.h>

namespace TextFusion
{
	bool Program::addFile(const std::string &path)
	{
		std::unique_lock lock(filesMtx);
		if (exd::fileExists(path))
		{
			files[path] = std::make_shared<TextFile>(path, settings->get("WatchDirectory"));
		}
		else
		{
			WARNING(path + "\tdoes not exist");
			return false;
		}

		return true;
	}
	
	void Program::watchDirectory_addNewFiles(
		const std::vector<std::string> &relevantDirectories,
		std::vector<std::string> &foundFiles,
		const std::vector<std::string> &extensions)
	{
		if (relevantDirectories.empty())
		{
			exd::filesWithExtensions(foundFiles, settings->get("WatchDirectory"), extensions, true);
		}
		else
		{
			for (std::string relevantDirectory : relevantDirectories)
			{
				exd::filesWithExtensions(foundFiles, relevantDirectory, extensions, true);
			}
		}

		for (std::string &path : foundFiles)
		{
			if (!exd::mapHas(files, path))
			{
				addFile(path);
			}
		}

		foundFiles.clear();
		std::this_thread::sleep_for(std::chrono::milliseconds(settings->get("WatchDirectoryDelay")));
	}
	void Program::updateTextFileMembers(std::vector<std::string> &filesToRemove)
	{
		std::unique_lock lock(filesMtx);
		for (const auto &pair : files)
		{
			pair.second->updateAttributes();
			if (!pair.second->exists)
			{
				filesToRemove.push_back(pair.second->path.string());
			}
			else if (pair.second->updated)
			{
				fileFusion_cv.notify_one();
			}
		}

		for (const std::string &path : filesToRemove)
		{
			files.erase(path);
		}
		if (!filesToRemove.empty())
		{
			fileFusion_cv.notify_one();
		}

		filesToRemove.clear();

		std::this_thread::sleep_for(std::chrono::milliseconds(settings->get("FilesStateDelay")));
	}
	
	void Program::fileElementsThread() {
		//Get settings
		const std::vector<std::string>& extensions = settings->get("Extensions");
		const std::vector<std::string>& relevantDirectoryNames = settings->get("RelevantDirectoryNames");
		const std::string& watchDirectory = settings->get("WatchDirectory");

		// Extra Variables
		std::vector<std::string> foundFiles;
		std::vector<std::string> relevantDirectories;
		std::vector<std::string> filesToRemove;

		while (!STOP)
		{
			exd::getAllDirectoryWithNames(relevantDirectories, watchDirectory, relevantDirectoryNames, true);
			watchDirectory_addNewFiles(relevantDirectories, foundFiles, extensions);
			updateTextFileMembers(filesToRemove);

			relevantDirectories.clear(); //because getAllDirectoryWithNames always appends
		}
	}
	void Program::fileFusionThread()
	{
		const std::string& fileToWrite = settings->get("FileToWrite");
		const std::string& writeFormat = settings->get("WriteFormat");
		const std::string& writeFormatEncp = settings->get("WriteFormatEncapsulation");
		const std::vector<std::string>& consoleCallBacks = settings->get("ConsoleCallBacks");

		std::vector<std::string> filesToRemove;
		std::unordered_map<std::string, std::string> filesContentHistory;
		while (!STOP)
		{
			std::unique_lock lock(filesMtx);
			fileFusion_cv.wait(lock, [&] {
				//if updated, or added
				if (files.size() != filesContentHistory.size()) {
					return true;
				}
				else {
					for (const auto& pair : files) {
						if (pair.second->updated) {
							return true;
						}
					}
				}
				return STOP; });


			// Check if any file was deleted
			for (const auto &pair : filesContentHistory)
			{
				if (!exd::mapHas(files, pair.first))
				{
					filesToRemove.push_back(pair.first);
				}
			}

			for (const std::string &path : filesToRemove)
			{
				filesContentHistory.erase(path);
			}
			filesToRemove.clear();

			// Writing to fileToWrite
			for (auto &pair : files)
			{
				std::string path = pair.second->path.string();
				if (pair.second->updated && exd::fileExists(path))
				{
					pair.second->updated = false;
					pair.second->updateContent();
				}

				std::string copy_writeFormat = writeFormat;
				FileCodes fileCodes(*pair.second);
				for (const auto& fileCode : fileCodes) {
					exd::replaceWith(copy_writeFormat, fileCode.first, fileCode.second);
				}

				filesContentHistory[path] = copy_writeFormat;
			}

			std::string data = "";
			std::string writeFormatEncap_copy = writeFormatEncp;
			for (auto &pair : filesContentHistory)
			{
				data += pair.second;
			}
			exd::replaceWith(writeFormatEncap_copy, "${here}", data);
			exd::writeFile(fileToWrite, writeFormatEncap_copy);

			//Console CallBacks called here
			SettingsCodes settingsCodes(*settings);
			for (const std::string& callBack : consoleCallBacks) {
				std::string callBack_copy = callBack;
				for (const auto& settingsCode : settingsCodes) {
					exd::replaceWith(callBack_copy, settingsCode.first, settingsCode.second);
				}
				system(callBack_copy.c_str());
			}
		};
	}
	void Program::KeyInputThread()
	{
		while (!STOP && key.load() != 'q')
		{
			if (_kbhit())
			{ // if you hear some key input
				key.store(_getch());
			}
		}
	}
	void Program::ConsoleThread()
	{
		std::thread keyListener(&Program::KeyInputThread, this);

		std::stringstream menu;
		menu << "Commands : " << std::endl;
		menu << "q - Quit" << std::endl;
		menu << "v - View Logs" << std::endl;
		menu << "c - Clear Console" << std::endl;
		menu << "m - Command List" << std::endl;
		menu << "etc - Nothing" << std::endl;

		std::atomic<char> currentKey;
		std::atomic<char> lastKey;

		while (!STOP)
		{
			std::unique_lock lock(consoleMtx);

			if (key.load() == 'q')
			{
				STOP_PROGRAM();
				fileFusion_cv.notify_one();
			}
			else if (key.load() == 'v')
			{
				std::stringstream viewFiles;
				system("cls");
				for (const auto &pair : files)
				{
					std::tm *timeInfo = &pair.second->lastUpdateTime;
					if (timeInfo->tm_hour < 0)
					{
						viewFiles << pair.second->path << "\t\t"
								  << ":::" << std::endl;
					}
					else
					{
						viewFiles << pair.second->path << "\t\t" << std::put_time(timeInfo, "%H:%M:%S") << std::endl;
					}
				}
				std::cout << viewFiles.str();
			}
			else if (key.load() == 'c')
			{
				system("cls");
			}
			else if (key.load() == 'm')
			{
				// menu
				system("cls");
				std::cout << menu.str();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(settings->get("ConsoleDelay")));
		}

		keyListener.join();
	}

	Program::Program(const std::string &watchDirectory)
	{
		settings = std::make_shared<Settings>(watchDirectory);
	}
	Program::~Program() {
		consoleHandler.join();
		filesElementsHandler.join();
		fileFusionHandler.join();

		LOG_DESTRUCTOR("Program");
	}

	void Program::init()
	{
	}
	void Program::run()
	{
		// Disbatched Threads
		consoleHandler = std::thread(&Program::ConsoleThread, this);
		fileFusionHandler = std::thread(&Program::fileFusionThread, this);
		filesElementsHandler = std::thread(&Program::fileElementsThread, this);
	}

	void Program::INIT()
	{
		Settings::INIT();
	}
	void Program::STOP_PROGRAM()
	{
		STOP = true;
	}

	bool Program::STOP = false;
}