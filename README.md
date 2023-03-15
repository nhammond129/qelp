# Dumpster fire 0.0.1

_idk what I'm doing edition_

## Table of Contents
1. [Building](#building)
    1. [Source](#source)
    2. [Documentation](#documentation)
2. [See also](#see-also)
3. [Dependencies](#dependencies)
4. [Asset Attribution](#asset-attribution)

## Building
### Source
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
 * [entt wiki](https://github.com/skypjack/entt/wiki/)
   * [entt wiki: Views](https://github.com/skypjack/entt/wiki/Crash-Course:-entity-component-system#views)
   * [entt wiki: Resource Management](https://github.com/skypjack/entt/wiki/Crash-Course:-resource-management)
 * [imgui manual](https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html)
 * [luacpp](https://github.com/jordanvrtanoski/luacpp)

## Dependencies
 * Multimedia Library:  [SFML](https://github.com/SFML/SFML)
 * Immediate-mode UI:   [imgui](https://github.com/ocornut/imgui)
 * ImGui backend:       [imgui-sfml](https://github.com/SFML/imgui-sfml)
 * ECS Framework:       [entt](https://github.com/skypjack/entt)
 * Documentation theme: [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css)

## Asset Attribution
[`assets/fonts/victor-pixel.ttf`](https://www.dafont.com/victors-pixel-font.font)