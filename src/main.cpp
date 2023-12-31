#include "pch.hpp"
#include "Core/Program.hpp"

using namespace TextFusion;

// main
int main()
{
    TextFusion::Program::INIT();
    std::string watchDirectory = "";
    while (!exd::dirExists(watchDirectory))
    {
        std::cout << "Enter WatchDirectory : ";
        std::getline(std::cin, watchDirectory);
    }
    try
    {
        std::unique_ptr<Program> program = std::make_unique<Program>(watchDirectory);
        program->init();
        program->run();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}