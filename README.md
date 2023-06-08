# Dumpster fire 0.0.2

This is an old game project that's on hiatus since I wasn't sure which direciton to take it in.\
It's roughly just messy entt/box2d/sfml/imgui boilerplate at this point.

Last I left off was somewhere in networking code, which really illuminated the need for a proper design pass. This ... never happened, lol.

I'm not sure if I'll ever pick this up again, but I'm keeping it around since it's a good example of my code style and I might want to reference it in the future.

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
 * [An Introduction to Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
 * [box2d documentation](https://box2d.org/documentation/)
 * [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
 * [CMake projects in Visual Studio](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170)
 * [Doxygen](https://www.doxygen.nl/manual/index.html)
 * [entt wiki](https://github.com/skypjack/entt/wiki/)
   * [entt wiki: Resource Management](https://github.com/skypjack/entt/wiki/Crash-Course:-resource-management)
   * [entt wiki: Views](https://github.com/skypjack/entt/wiki/Crash-Course:-entity-component-system#views)
 * [imgui manual](https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html)
 * [luacpp](https://github.com/jordanvrtanoski/luacpp)
 * [SFML Documentation](https://www.sfml-dev.org/documentation/2.5.1/modules.php)

## Dependencies
 * Multimedia Library:  [SFML](https://github.com/SFML/SFML)
 * Immediate-mode UI:   [imgui](https://github.com/ocornut/imgui)
 * ImGui backend:       [imgui-sfml](https://github.com/SFML/imgui-sfml)
 * ECS Framework:       [entt](https://github.com/skypjack/entt)
 * Documentation theme: [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css)

## Asset Attribution
[`assets/fonts/victor-pixel.ttf`](https://www.dafont.com/victors-pixel-font.font)