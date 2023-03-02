# Dumpster fire 0.0.1

_idk what I'm doing edition_

## Table of Contents
1. [Building](#building)
    1. [Documentation](#documentation)
2. [See also](#see-also)
3. [Dependencies](#dependencies)
4. [Asset Attribution](#asset-attribution)

## Building
```bash
cmake -S . -B build     # generate the build files
cmake --build build     # build the project
./build/Debug/main.exe  # run the project
```

### Documentation
```bash
cmake --build build --target docs
```


## See also
 * [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
 * [CMake projects in Visual Studio](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170)
 * [An Introduction to Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
 * [SFML Documentation](https://www.sfml-dev.org/documentation/2.5.1/modules.php)
 * [Doxygen](https://www.doxygen.nl/manual/index.html)

## Dependencies
----
 * Multimedia Library:  [SFML](https://github.com/SFML/SFML)
 * Immediate-mode UI:   [imgui](https://github.com/ocornut/imgui)
 * ImGui backend:       [imgui-sfml](https://github.com/SFML/imgui-sfml)
 * ECS Framework:       [entt](https://github.com/skypjack/entt)
 * Documentation theme: [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css)

## Asset Attribution
[`assets/fonts/victor-pixel.ttf`](https://www.dafont.com/victors-pixel-font.font)\
[`assets/sprites/space_carrier_0.png`](https://opengameart.org/content/space-carrier)