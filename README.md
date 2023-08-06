# TextFusion

TextFusion is an automation console app "tool" that fuses file information from multiple textfiles (e.g .cpp, .py, .js, .hpp) and pastes them into one ultimate file with the specified format.

# Settings

Within the `WatchDirectory` there must directly be a TextFusion.json file with all of the settings below configured.

- `Extensions` : The program will recursively search through the `WatchDirectory` to search for any file ending with the file extensions (e.g .cpp, .hpp, .txt) you specify
- `RelevantDirectoryNames` : adds extra specification on top of `Extensions` such that the program will only search for `Extensions` within the directories with the following names.
- `FileToWrite` : this is the file you will write your file information with the given formats.
  --`${path}` - pastes in the file's path.
  -- `${content}` - pastes in the raw content of the file.

- `WriteFormat` : specifying the format to write each file information in.
- `WriteFormatEncapsulation` : all of the files formatted in `WriteFormat` is pasted into _${here}_ along with other text surounding it.

- `WatchDirectoryDelay` : delay in **milliseconds** for Watching the Directory to see if any additional files have been added.
- `FilesStateDelay` : delay in **milliseconds** for Updating the states of files (update & existence).
- `ConsoleDelay` : delay in **milliseconds** for printing the files' update information to the console.

## Example

```json
{
  "Extensions": [".vert", ".frag"],
  "RelevantDirectoryNames": [],
  "FileToWrite": "C:\\Users\\Devin\\Downloads\\Code\\VS\\Cpp\\OpenGL\\test.cpp",
  "WriteFormat": "someFunction(\"${path}\",\n\"${content}\");\n",
  "WriteFormatEncapsulation": "//C:\\Users\\Devin\\Downloads\\Code\\VS\\Cpp\\OpenGL\\test.cpp\n#include <iostream>\n#include<string>\nvoid someFunction(std::string str1, std::string str2){ std::cout << str1; };\n int main(){ ${here} \n return 0;}",

  "WatchDirectoryDelay": 50,
  "FilesStateDelay": 50,
  "ConsoleDelay": 1000
}
```

## Example Result

```cpp
//C:\Users\Devin\Downloads\Code\VS\Cpp\OpenGL\test.cpp
#include <iostream>
#include<string>
void someFunction(std::string str1, std::string str2){ std::cout << str1; };
 int main(){ someFunction("C:\\Users\\Devin\\Downloads\\Code\\VS\\Cpp\\OpenGL\\OpenGL-Examples\\assets\\shaders\\Square.frag",
"#version 330 core\n"
"\n"
"in vec3 fColor;\n"
"out vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"	color = vec4(fColor, 1.0);\n"
"}");
someFunction("C:\\Users\\Devin\\Downloads\\Code\\VS\\Cpp\\OpenGL\\OpenGL-Examples\\assets\\shaders\\Square.vert",
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

 return 0;}
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
