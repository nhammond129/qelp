#include <iostream>
#include <string>
#include <exception>
#include <SFML/Graphics.hpp>
#include <util.hpp>

sf::Text createText(const std::string& text, sf::Vector2f position, sf::Font& font, sf::RenderWindow& window, unsigned int size = 10, sf::Color color = sf::Color::White) {
    sf::Text textObject;
    textObject.setFont(font);
    textObject.setString(text);
    textObject.setCharacterSize(size);
    textObject.setFillColor(color);
    textObject.setPosition(position);
    return textObject;
}

class ShipEntity: public sf::Sprite {
public:
    ShipEntity(const sf::Texture& texture, sf::Sprite&& attached_sprite, sf::Vector2f offset): mAttached(attached_sprite) {
        setTexture(texture);
        setOrigin(sf::Vector2f((float)(texture.getSize().x / 2), (float)(texture.getSize().y / 2)));
        attached_sprite.setPosition(offset);
    }
    virtual ~ShipEntity() = default;

    sf::Sprite mAttached;
private:
    void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override {
        target.draw(static_cast<const sf::Sprite>(*this));

        sf::RenderStates statesCopy = states;
        statesCopy.transform *= getTransform();
        target.draw(mAttached, statesCopy);
    }
};  // class ShipEntity

int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "SFML works!");
    // window.setFramerateLimit(60);

    sf::Font font;
    // these paths are relative to the working directory the program is run from
    // in this case, the working directory is the build directory (qelp/build/)
    if (!font.loadFromFile("./assets/fonts/victor-pixel.ttf")) throw std::runtime_error("Error loading font");
    sf::Texture texture;
    if (!texture.loadFromFile("./assets/sprites/space_carrier_0.png")) throw std::runtime_error("Error loading texture");

    sf::Texture turret_texture = util::programmerArtTexture(16, 32, sf::Color::Red, sf::Color::Transparent);

    sf::Sprite sproot(turret_texture);
    sf::FloatRect attachedBounds = sproot.getLocalBounds();
    sproot.setOrigin({attachedBounds.width / 2.f, attachedBounds.height * 3.f / 4.f});

    // TODO: play with builder pattern that rust users are so fond of
    ShipEntity ship(
        texture,
        std::move(sproot),
        {100,50}
    );
    
    sf::CircleShape shape(100.f); // orange circle
    shape.setFillColor(sf::Color(255,140,0,255));
    
    sf::Text info_text = createText("", {0, 0}, font, window, 10, sf::Color::Green);
    sf::Text entry_text = createText("> ", {0,2*270/3}, font, window, 10, sf::Color::Green);

    sf::RenderTexture renderTexture;
    if (!renderTexture.create({1280, 720})) throw std::runtime_error("Error creating render texture");

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::TextEntered) {
                // if the key pressed is backspace, remove the last character
                if (event.text.unicode == 8) {
                    auto str = entry_text.getString();
                    if (str.getSize() > 0) {
                        str.erase(str.getSize() - 1);
                        entry_text.setString(str);
                    }
                } else if (event.text.unicode == 127) {  // ctrl-backspace to clear the entry
                    entry_text.setString("> ");
                } else {
                    // std::cout << "character typed: " << static_cast<uint16_t>(event.text.unicode) << std::endl;
                    entry_text.setString(entry_text.getString() + static_cast<char>(event.text.unicode));
                }
            }
        }
        float dt = clock.restart().asSeconds();
        info_text.setString(std::to_string(16-static_cast<int>(dt*1000))+" ms frame budget");

        // if keydown 'd' move sprite to the right
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) ship.rotate(sf::degrees(static_cast<float>(90.f*dt)));
        // if keydown 'a' move sprite to the left
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) ship.rotate(sf::degrees(static_cast<float>(-90.f*dt)));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            auto rotation = ship.getRotation();
            rotation += sf::degrees(-90.f); // rotate 'forward' vector ccw to match texture
            float x = static_cast<float>(std::cos(rotation.asRadians()) * 200.f * dt);
            float y = static_cast<float>(std::sin(rotation.asRadians()) * 200.f * dt);
            ship.move({x, y});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {  // s for spiiiin
            ship.mAttached.rotate(sf::degrees(static_cast<float>(360.f*dt)));
        }

        {
            sf::View view = renderTexture.getView();
            view.setCenter(ship.getPosition());
            renderTexture.setView(view);
        }
        
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            auto mouse_pos = sf::Mouse::getPosition(window);
            auto world_pos = window.mapPixelToCoords(mouse_pos);
            // draw a line between the sprite and the mouse
            sf::Vertex line[] = {
                sf::Vertex(ship.getPosition()),
                sf::Vertex(world_pos)
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
        }
        {
            {
                renderTexture.clear(sf::Color::Black);
                renderTexture.draw(shape);
                renderTexture.draw(ship);
                renderTexture.display();
            }
            window.clear();
            window.draw(sf::Sprite(renderTexture.getTexture()));
            window.draw(info_text);
            window.draw(entry_text);
            window.display();
        }
    }

    return 0;
}