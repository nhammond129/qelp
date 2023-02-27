#pragma once

#include <IScene.hpp>

namespace scenes {

class MenuScene: public IScene {
public:
    MenuScene();
    virtual ~MenuScene() = default;

    void draw(sf::RenderWindow& window) override;
    void update() override;
    void handleEvents(sf::RenderWindow& window) override;
private:
    sf::Font mFont;  // TODO: `AssetManager&` -- injected ?
    sf::Text mText;
    sf::Clock mClock;
};

};  // namespace scenes