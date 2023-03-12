#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <config.hpp>
#include <exception>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <math.h>
#include <scenes.hpp>
#include <string>
#include <util.hpp>

#ifdef __linux__
#include <unistd.h>
#elif __APPLE__
#include <mach-o/dyld.h>    // Not sure if this is correct. I don't have a mac to test this on.
#elif _WIN32
#include <Windows.h>
#endif

namespace aaa {

const size_t PATH_MAX = 4096;
const std::string executable_path() {
#ifdef __linux__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
#elif __APPLE__
    char result[PATH_MAX];
    uint32_t size = sizeof(result);
    if (_NSGetExecutablePath(result, &size) == 0)
        return std::string(result);
    else
        return std::string();
#elif _WIN32
    char result[PATH_MAX];
    if (!GetModuleFileNameA(NULL, result, PATH_MAX))
        throw std::runtime_error("Error getting executable path: " + std::to_string(GetLastError()));
    return std::string(result);
#endif
}

};  // namespace aaa
int main() {


    std::cout << "Executable path: " << aaa::executable_path() << std::endl;

    scenes::SceneManager manager(sf::VideoMode({config::SCREEN_WIDTH, config::SCREEN_HEIGHT}), config::WINDOW_TITLE);
    manager.pushScene(new scenes::MenuScene(manager));
    while (manager.isRunning()) {
        manager.handleEvents();
        manager.update();
        manager.draw();
    }

    return 0;
}
