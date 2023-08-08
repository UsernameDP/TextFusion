# TextFusion

TextFusion is an automation console app "tool" that fuses file information from multiple textfiles (e.g .cpp, .py, .js, .hpp) and pastes them into one ultimate file with the specified format.

# Settings

## KeyCodes

### FileCodes

- `${Path}` : pastes in the file's path.
- `${RelativePath}` : pastes in the file's path relative to the WatchDirectory
- `${Content}` : pastes in the raw content of the file.

### SettingsCodes

- All the settings below that are strings can be specified (e.g ${FileToWrite}, ${WriteFormat}, .etc).

Within the `WatchDirectory` there must directly be a TextFusion.json file with all of the settings below configured.

- `Extensions`|string array| : The program will recursively search through the `WatchDirectory` to search for any file ending with the file extensions (e.g .cpp, .hpp, .txt) you specify
- `RelevantDirectoryNames`|string array| : adds extra specification on top of `Extensions` such that the program will only search for `Extensions` within the directories with the following names.
- `FileToWrite`|string| : this is the file you will write your file information with the given formats.
- `WriteFormat`|string| : specifying the format to write each file information in.
  - All [FileCodes](#filecodes) can be used here
- `WriteFormatEncapsulation`|string| : all of the files formatted in `WriteFormat` is pasted into _${here}_ along with other text surounding it. You can also put the path for where to find WriteFormatEncapsulation instead of putting the literal content (feature added because json doesn't allow you to break lines in "").
- `ConsoleCallBacks`|string array| : any functions can be ran in the system terminal can be specified to run here. These callbacks are ran right after the program writes to `FileToWrite`. These terminal commands are ran in ORDER.

  - All [SettingsCodes](#settingscodes) can be used here.

- `WatchDirectoryDelay`|unsigned int| : delay in **milliseconds** for Watching the Directory to see if any additional files have been added.
- `FilesStateDelay`|unsigned int| : delay in **milliseconds** for Updating the states of files (update & existence).
- `ConsoleDelay`|unsigned int| : delay in **milliseconds** for printing the files' update information to the console.
- `WatchDirectoryDelay`|unsigned int| : delay in **milliseconds** for Watching the Directory to see if any additional files have been added.
- `FilesStateDelay`|unsigned int| : delay in **milliseconds** for Updating the states of files (update & existence).
- `ConsoleDelay`|unsigned int| : delay in **milliseconds** for printing the files' update information to the console.

## Example

```json
{
  "Extensions": [".vert", ".frag"],
  "RelevantDirectoryNames": ["assets"],
  "FileToWrite": "C:\\Users\\Devin\\Downloads\\Code\\VS\\Cpp\\OpenGL\\OpenGL-Examples\\src\\Examples\\Basic\\BasicAssetPoolINIT_GLSLSrcs.hpp",
  "WriteFormat": "util::AssetPool::setGLSLSrc(\"${RelativePath}\", \"${Content}\");\n",
  "WriteFormatEncapsulation": "#include <iostream>\n#include<string>\nvoid someFunction(std::string str1, std::string str2){ std::cout << str1; };\n int main(){ ${here} \n return 0;}",
  "ConsoleCallBacks": ["clang-format -i ${FileToWrite}"],
  "RelevantDirectoryNames": ["assets"],
  "FileToWrite": "C:\\Users\\Devin\\Downloads\\Code\\VS\\Cpp\\OpenGL\\OpenGL-Examples\\src\\Examples\\Basic\\BasicAssetPoolINIT_GLSLSrcs.hpp",
  "WriteFormat": "util::AssetPool::setGLSLSrc(\"${RelativePath}\", \"${Content}\");\n",
  "WriteFormatEncapsulation": "#include <iostream>\n#include<string>\nvoid someFunction(std::string str1, std::string str2){ std::cout << str1; };\n int main(){ ${here} \n return 0;}",
  "ConsoleCallBacks": ["clang-format -i ${FileToWrite}"],

  "WatchDirectoryDelay": 1000,
  "FilesStateDelay": 1000,
  "ConsoleDelay": 1000
}
```

## Example Result

```cpp
#pragma once
#include "GLCoreAssetPool.hpp"
using namespace GLCore;
class BasicAssetPoolINIT_GLSLSrcs : public util::AssetPoolINIT_GLSLSrcs {
public:
  void configure() override {
    util::AssetPool::setGLSLSrc("assets\\ohTesting.frag", "");
    util::AssetPool::setGLSLSrc("OpenGL-Examples\\assets\\shaders\\Square.vert",
                                "#version 330 core\n"
                                "layout (location = 0) in vec2 aPos;\n"
                                "layout (location = 1) in vec3 aColor;\n"
                                "\n"
                                "out vec3 fColor;\n"
                                "\n"
                                "void main()\n"
                                "{\n"
                                "    fColor = aColor;\n"
                                "    gl_Position = vec4(aPos, 1.0, 1.0);\n"
                                "}");
    util::AssetPool::setGLSLSrc("OpenGL-Examples\\assets\\shaders\\Square.frag",
                                "#version 330 core\n"
                                "\n"
                                "in vec3 fColor;\n"
                                "out vec4 color;\n"
                                "\n"
                                "void main()\n"
                                "{\n"
                                "	color = vec4(fColor, 1.0);\n"
                                "}");
  }
};
#pragma once
#include "GLCoreAssetPool.hpp"
using namespace GLCore;
class BasicAssetPoolINIT_GLSLSrcs : public util::AssetPoolINIT_GLSLSrcs {
public:
  void configure() override {
    util::AssetPool::setGLSLSrc("assets\\ohTesting.frag", "");
    util::AssetPool::setGLSLSrc("OpenGL-Examples\\assets\\shaders\\Square.vert",
                                "#version 330 core\n"
                                "layout (location = 0) in vec2 aPos;\n"
                                "layout (location = 1) in vec3 aColor;\n"
                                "\n"
                                "out vec3 fColor;\n"
                                "\n"
                                "void main()\n"
                                "{\n"
                                "    fColor = aColor;\n"
                                "    gl_Position = vec4(aPos, 1.0, 1.0);\n"
                                "}");
    util::AssetPool::setGLSLSrc("OpenGL-Examples\\assets\\shaders\\Square.frag",
                                "#version 330 core\n"
                                "\n"
                                "in vec3 fColor;\n"
                                "out vec4 color;\n"
                                "\n"
                                "void main()\n"
                                "{\n"
                                "	color = vec4(fColor, 1.0);\n"
                                "}");
  }
};
```

# Menu / KeyInputs

```txt
Commands :
q - Quit
v - View Logs
c - Clear Console
m - Command List
etc - Nothing
```

# Build

## Method 1

1. `git clone -b main https://github.com/UsernameDP/TextFusion.git`
2. `cd scripts`
3. `command.bat` - for help
   -- `command.bat compile` - for debug build
   -- `command.bat release` -- for release build (run this for use)
4. Check within bin in _TextFusion_ to find the .exe executable.

## Method 2

1. Open File Explorer
2. Go to the _TextFusion_ directory
3. Go to _scripts_ under _TextFusion_
4. Double click _create.bat_ for debug, _release.bat_ for release version
5. Check within bin in _TextFusion_ to find the .exe executable.

# TODO

- Test out having WatchDirectoryFunction and FileStateFunction into their own threads and see if it improves performance.
- Create the GUI version of this using IMGUI (doesn't watch multiple files, just one)

# Lessons Learned

- Do not try to make code organization perfect, it can turn a 2 hour project into a 1 week project (that is what happened this project).
- Unless you are trying to create a template or you already know all of your technologies, always try writing your code in a main.cpp file then separate them into different classes later.
