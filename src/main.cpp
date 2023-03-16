#define _USE_MATH_DEFINES
#include <config.hpp>
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

int main() {
    scenes::SceneManager manager(sf::VideoMode({config::SCREEN_WIDTH, config::SCREEN_HEIGHT}), config::WINDOW_TITLE);
    manager.pushScene(new scenes::MenuScene(manager));
    while (manager.isRunning()) {
        manager.handleEvents();
        manager.update();
        manager.draw();
    }
    return 0;
}
