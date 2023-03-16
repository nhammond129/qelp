#pragma once

#include <data/cached.hpp>
#include <functional>
#include <IScene.hpp>

namespace scenes {

class MenuScene: public IScene {
    struct Option {
        bool hot = false;
        std::string name;
        sf::Text text;
        std::function<void()> callback;

        Option(const sf::Text& text, std::function<void()> callback)
            : text(text), callback(callback), name(text.getString()) {}
    };
public:
    MenuScene(SceneManager& manager);

    void draw(sf::RenderWindow& window) override;
    void update(const sf::Time& dt) override;
    void handleEvent(const sf::Event& event) override;
private:
    const sf::Font& mFont = data::Fonts["fonts/victor-pixel.ttf"];

    sf::Vector2f mAnchor = {100, 100};
    float mSpacing = 10;
    std::vector<Option> mOptions;
};

};  // namespace scenes