#define _USE_MATH_DEFINES
#include <exception>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <math.h>
#include <scenes.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <util.hpp>

int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            std::cout << "argv[" << i << "] = '"<< argv[i] << "'" << std::endl;
        }
    }

    scenes::SceneManager manager(new scenes::MenuScene());
    while (manager.isRunning()) {
        manager.handleEvents();
        manager.update();
        manager.draw();
    }

    return 0;
}