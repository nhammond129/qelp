#pragma once

#include <IScene.hpp>

namespace scenes {

class GameScene: public IScene {
public:
    GameScene();

    void draw() override;
    void update() override;
    void handleEvents() override;
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