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

int main(int argc, char* argv[]) {
#if 0
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            std::cout << "argv[" << i << "] = '"<< argv[i] << "'" << std::endl;
        }
    }
#endif

    scenes::SceneManager manager(sf::VideoMode({config::SCREEN_WIDTH, config::SCREEN_HEIGHT}), config::WINDOW_TITLE);
    manager.pushScene(new scenes::MenuScene(manager));
    while (manager.isRunning()) {
        manager.handleEvents();
        manager.update();
        manager.draw();
    }

    return 0;
}
