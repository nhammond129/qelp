#pragma once

#include <IScene.hpp>
#include <entt/entt.hpp>

namespace scenes {

class GameScene: public IScene {
public:
    GameScene(SceneManager& manager);

    void draw(sf::RenderWindow& window) override;
    void update(const sf::Time& dt) override;
    void handleEvent(const sf::Event& event) override;
private:
    sf::RenderTexture mWorldRT;
    sf::Font mFont;
    entt::registry mRegistry;
};

};  // namespace scenes