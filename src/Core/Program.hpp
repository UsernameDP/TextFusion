#pragma once
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Settings.hpp"
#include "Files.hpp"
#include "KeyCodes.hpp"

namespace TextFusion
{
	class Program
	{
	private:
		std::mutex filesMtx;
		std::condition_variable fileFusion_cv;
		std::unordered_map<std::string, std::shared_ptr<TextFile>> files;
		std::shared_ptr<Settings> settings = nullptr;

		std::mutex consoleMtx;
		std::atomic<char> key = ' ';

		std::thread consoleHandler; //will bedeleted in gui version
		std::thread filesElementsHandler;
		std::thread fileFusionHandler;

	private:
		// Helper Functions
		bool addFile(const std::string &path);

		// Portions of Thread Functions
		void watchDirectory_addNewFiles(const std::vector<std::string> &relevantDirectories,
									std::vector<std::string> &foundFiles,
									const std::vector<std::string> &extensions);
		void updateTextFileMembers(std::vector<std::string> &filesToRemove);
		// Thread Functions
		void fileElementsThread();
		void fileFusionThread();

		void KeyInputThread(); //will be deleted in gui version
		void ConsoleThread(); //will be delete in gui version

	public:
		static bool STOP;
	public:
		Program(const std::string &watchDirectory);
		~Program();

		void init();
		void run();

		static void INIT();
		static void STOP_PROGRAM();
	};
}