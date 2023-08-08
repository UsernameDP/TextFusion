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
			files[path] = std::make_shared<TextFile>(path);
		}
		else
		{
			WARNING(path + "\\tdoes not exist");
			return false;
		}

		return true;
	}
	void Program::WatchDirectoryFunction(
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
	void Program::FilesStateFunction(std::vector<std::string> &filesToRemove)
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
	void Program::FileFusionThread()
	{
		json fileToWrite_json = settings->get("FileToWrite");
		json writeFormat_json = settings->get("WriteFormat");
		json writeFormatEncap_json = settings->get("WriteFormatEncapsulation");
		std::string fileToWrite = fileToWrite_json.get<std::string>();
		std::string writeFormat = writeFormat_json.get<std::string>();
		std::string writeFormatEncp = writeFormatEncap_json.get<std::string>();

		std::vector<std::string> filesToRemove;
		std::unordered_map<std::string, std::string> contentofFiles;
		while (!STOP)
		{
			std::unique_lock lock(filesMtx);
			fileFusion_cv.wait(lock, [&]
							   {
					for (const auto& pair : files) {
						if (pair.second->updated) {
							return true;
						}
					}
					for (const auto& pair : contentofFiles) {
						if (!exd::mapHas(files, pair.first)) {
							return true;
						}
					}
					return STOP; });

			// Check if any file was deleted
			for (const auto &pair : contentofFiles)
			{
				if (!exd::mapHas(files, pair.first))
				{
					filesToRemove.push_back(pair.first);
				}
			}
			for (const std::string &path : filesToRemove)
			{
				contentofFiles.erase(path);
			}
			filesToRemove.clear();

			// Writing to fileToWrite
			for (auto &pair : files)
			{
				std::string path = pair.second->path.string();
				if (pair.second->updated)
				{
					pair.second->updated = false;
					pair.second->updateContent();
				}

				std::string copy_writeFormat = writeFormat;
				exd::replaceWith(copy_writeFormat, "${path}", exd::getReplaceAll(path, "\\", "\\\\"));
				exd::replaceWith(copy_writeFormat, "${relativePath}", exd::getReplaceAll(exd::getRelative(path, settings->get("WatchDirectory")), "\\", "\\\\"));
				exd::replaceWith(copy_writeFormat, "${content}", exd::getReplaceAll(exd::trim(pair.second->content), "\n", "\\n\"\n\""));
				contentofFiles[path] = copy_writeFormat;
			}

			std::string data = "";
			std::string writeFormatEncap_copy = writeFormatEncp;
			for (auto &pair : contentofFiles)
			{
				data += pair.second;
			}
			exd::replaceWith(writeFormatEncap_copy, "${here}", data);
			exd::writeFile(fileToWrite, writeFormatEncap_copy);
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
		fileFusionHandler.join();

		LOG_DESTRUCTOR("Program");
	}

	void Program::init()
	{
	}
	void Program::run()
	{
		// Needed Settings for Thread Functions
		json relevantDirectoryNames_json = settings->get("RelevantDirectoryNames");
		json extensions_json = settings->get("Extensions");

		const std::vector<std::string>& extensions = extensions_json; 

		std::vector<std::string> relevantDirectoryNames = relevantDirectoryNames_json.get<std::vector<std::string>>();

		// Extra Variables
		std::vector<std::string> foundFiles;
		std::vector<std::string> relevantDirectories;
		std::vector<std::string> filesToRemove;
		exd::getAllDirectoryWithNames(relevantDirectories, settings->get("WatchDirectory"), relevantDirectoryNames, true);

		// Disbatched Threads
		consoleHandler = std::thread(&Program::ConsoleThread, this);
		fileFusionHandler = std::thread(&Program::FileFusionThread, this);

		while (!STOP)
		{
			WatchDirectoryFunction(relevantDirectories, foundFiles, extensions);
			FilesStateFunction(filesToRemove);
		}
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