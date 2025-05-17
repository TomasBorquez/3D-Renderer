#define MATE_IMPLEMENTATION
#include "mate.h"

i32 main() {
  StartBuild();
  {
    String ninjaBuild = CreateExecutable((ExecutableOptions){.output = "main", .flags = "-Wall -Wextra -g"});

    AddFile("./src/main.c");
    AddFile("./src/model.c");
    AddFile("./src/texture.c");
    AddFile("./src/renderer.c");

    AddIncludePaths("./vendor/SDL3/include", "./vendor/SDL3_image/include", "./src");
    AddLibraryPaths("./vendor/SDL3/lib", "./vendor/SDL3_image/lib");

    LinkSystemLibraries("SDL3", "SDL3_image");
    LinkSystemLibraries("user32", "gdi32", "shell32", "winmm", "setupapi", "version", "imm32", "ole32");

    // TODO: Automatically copy vendor/*/bin/*.dll to build
    String exePath = InstallExecutable();
    RunCommand(exePath);
    CreateCompileCommands(ninjaBuild);
  }
  EndBuild();
}
