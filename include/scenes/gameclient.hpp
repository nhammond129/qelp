#pragma once

#include <data/cached.hpp>
#include <entt/entt.hpp>
#include <gamestate.hpp>
#include <IScene.hpp>

namespace scenes {

class GameClient : public IScene {
public:
    GameClient(SceneManager& manager);

    void draw(sf::RenderWindow& window) override;
    void update(const sf::Time& dt) override;
    void handleEvent(const sf::Event& event) override;

    void fixed_update(const sf::Time& dt);
private:
    game::State mGameState;
    sf::RenderTexture mWorldRT;
    
};

};  // namespace scenes