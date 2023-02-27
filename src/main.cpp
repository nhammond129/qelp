#define _USE_MATH_DEFINES
#include <exception>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <math.h>
#include <scenes.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <util.hpp>

class ShipEntity: public sf::Sprite {
public:
    ShipEntity(const sf::Texture& texture, sf::Sprite&& attached_sprite, sf::Vector2f offset): mAttached(attached_sprite), mOffset(offset) {
        setTexture(texture);
        setOrigin(sf::Vector2f((float)(texture.getSize().x / 2), (float)(texture.getSize().y / 2)));
        mAttached.setPosition(offset);
    }
    virtual ~ShipEntity() = default;

    void pointAttachmentAt(sf::Vector2f target) {
        sf::Vector2f offset = target - mAttached.getPosition();
        float angle_rad = std::atan2(offset.y, offset.x);
        mAttached.setRotation(sf::radians(angle_rad));
    }

    void update(float dt) {
        mAttached.setPosition(getPosition() + mOffset.rotatedBy(getRotation()));
    }

    sf::Sprite mAttached;
    sf::Vector2f mOffset;
private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override {
        target.draw(static_cast<const sf::Sprite>(*this), states);
        target.draw(mAttached, states);
    }
};  // class ShipEntity

int main(int argc, char* argv[]) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            std::cout << "argv[" << i << "] = '"<< argv[i] << "'" << std::endl;
        }
    }

    scenes::SceneManager manager(new scenes::MenuScene());

    while (manager.isRunning()) {
        manager.handleEvents();
        manager.update();
        manager.draw();
    }

    return 0;
}