#pragma once

#include <IScene.hpp>

namespace scenes {

class GameScene: public IScene {
public:
    GameScene();

    void draw(sf::RenderWindow& window) override;
    void update(const sf::Time& dt) override;
    void handleEvent(const sf::Event& event) override;
private:
    sf::Texture  mShipTexture;
    sf::Texture  mShipTurretTexture;
    sf::Sprite   mShipSprite;
    sf::Sprite   mTurretSprite;
    sf::Vector2f mTurretOffset;  // offset from ship origin to turret origin

    sf::RenderTexture mWorldRT;

    sf::Font mFont;
};

};  // namespace scenes