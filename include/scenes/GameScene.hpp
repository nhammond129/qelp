#pragma once

#include <IScene.hpp>
#include <entt/entt.hpp>

namespace scenes {

class GameScene: public IScene {
    const sf::Time mFixedUpdateInterval = sf::seconds(1.f / 60.f);
public:
    GameScene(SceneManager& manager);

    void draw(sf::RenderWindow& window) override;
    void update(const sf::Time& dt) override;
    void handleEvent(const sf::Event& event) override;

    void fixed_update(const sf::Time& dt );
private:
    bool mViewTracking = true;
    sf::RenderTexture mWorldRT;
    sf::Font mFont;
    entt::registry mRegistry;
};

};  // namespace scenes