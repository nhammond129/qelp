#pragma once

#include <IScene.hpp>
#include <data/cached.hpp>

namespace scenes {

class MenuScene: public IScene {
public:
    MenuScene(SceneManager& manager);

    void draw(sf::RenderWindow& window) override;
    void update(const sf::Time& dt) override;
    void handleEvent(const sf::Event& event) override;
private:
    const sf::Font& mFont = data::Fonts["fonts/victor-pixel.ttf"];
    sf::Text mText;
    sf::Text mText2;
};

};  // namespace scenes