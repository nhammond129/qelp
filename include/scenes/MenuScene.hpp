#pragma once

#include <IScene.hpp>

namespace scenes {

class MenuScene: public IScene {
public:
    MenuScene();

    void draw() override;
    void update() override;
    void handleEvents() override;
private:
    sf::Font mFont;  // TODO: `AssetManager&` -- injected ?
    sf::Text mText;
};

};  // namespace scenes