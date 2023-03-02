#pragma once

#include <IScene.hpp>

namespace scenes {

class MenuScene: public IScene {
public:
    MenuScene(SceneManager& manager);

    void draw(sf::RenderWindow& window) override;
    void update(const sf::Time& dt) override;
    void handleEvent(const sf::Event& event) override;
private:
    sf::Font mFont;  // TODO: `AssetManager&` -- injected ?
    sf::Text mText;
};

};  // namespace scenes