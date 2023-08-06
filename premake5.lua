workspace "TextFusion"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

    filter {"configurations:Debug"}
        buildoptions "/MTd"
        runtime "Debug"
        symbols "on"
    
    filter {"configurations:Release"}
        buildoptions "/MT"
        runtime "Release"
        optimize "on"
        defines {"VERSION_RELEASE"}
    
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}" 

project "TextFusion"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"
    location "Generated"

    pchheader "pch.hpp"
    pchsource "src/pch.cpp"

    targetdir("bin/" .. outputdir .. "/%{prj.name}") 
    objdir("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**"
    }

    includedirs {
        "src",
        "vendor/nlohmann/include"
    }

newaction {
    trigger = "clean",
    description = "Remove all binaries and intermediate binaries, and vs files.",
    execute = function()
        print("Removing binaries")
        os.rmdir("bin")
        
        print("Removing binaries")
        os.rmdir("bin-int")

        print("Removing Project Files")
        os.rmdir("Generated")
        os.remove("**.sln")
        os.remove("**.vcxproj")
        os.remove("**.vcxproj.filters")
        os.remove("**.vcxproj.user")
        os.remove("**.vs")

        print("Done")
    end
}