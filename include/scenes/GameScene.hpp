#pragma once

#include <data/cached.hpp>
#include <entt/entt.hpp>
#include <IScene.hpp>

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
    const sf::Font& mFont = data::Fonts["fonts/victor-pixel.ttf"];
    bool mViewTracking = true;
    sf::RenderTexture mWorldRT;
    entt::registry mRegistry;
};

};  // namespace scenes