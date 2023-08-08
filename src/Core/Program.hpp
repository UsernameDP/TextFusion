#pragma once
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Settings.hpp"
#include "Files.hpp"

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

		std::thread consoleHandler; //deleted in gui version
		std::thread fileFusionHandler;

	private:
		// Helper Functions
		bool addFile(const std::string &path);

		// Thread Functions
		void WatchDirectoryFunction(const std::vector<std::string> &relevantDirectories,
									std::vector<std::string> &foundFiles,
									const std::vector<std::string> &extensions);
		void FilesStateFunction(std::vector<std::string> &filesToRemove);
		void FileFusionThread();
		void KeyInputThread();
		void ConsoleThread();

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