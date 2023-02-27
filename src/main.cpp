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
}

int old_main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "SFML works!");
    // window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::Font font;
    // these paths are relative to the working directory the program is run from
    // in this case, the working directory is assumed to be the project root directory (where the top CMakeLists.txt is)
    if (!font.loadFromFile("./assets/fonts/victor-pixel.ttf")) throw std::runtime_error("Error loading font");
    sf::Texture texture;
    if (!texture.loadFromFile("./assets/sprites/space_carrier_0.png")) throw std::runtime_error("Error loading texture");

    sf::Texture turret_texture = util::programmerArtTexture(64, 24, sf::Color::Red, sf::Color::Transparent);

    sf::Sprite sproot(turret_texture);
    sf::FloatRect attachedBounds = sproot.getLocalBounds();
    sproot.setOrigin({attachedBounds.width * 0.25f, attachedBounds.height * 0.5f});

    ShipEntity ship(
        texture,
        std::move(sproot),
        {100,50}
    );
    
    sf::CircleShape shape(100.f); // orange circle
    shape.setFillColor(sf::Color(255,140,0,255));
    
    sf::Text info_text = createText("", {0, 0}, font, window, 10, sf::Color::Green);
    sf::Text entry_text = createText("> ", {0,720*0.8f}, font, window, 10, sf::Color::Green);

    sf::RenderTexture renderTexture;
    if (!renderTexture.create({1280, 720})) throw std::runtime_error("Error creating render texture");

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Time sf_dt = clock.restart();
        float dt = sf_dt.asSeconds();
        info_text.setString(std::to_string(16-static_cast<int>(dt*1000))+" ms frame budget");
        renderTexture.clear(sf::Color::Black);
        window.clear();

        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

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
        ImGui::SFML::Update(window, sf_dt);

        ImGui::ShowDemoWindow();

        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        ImGui::End();

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

        ship.update(dt);
        {
            sf::View view = renderTexture.getView();
            view.setCenter(ship.getPosition());
            renderTexture.setView(view);
        }
        
        sf::Vector2i mousepos = sf::Mouse::getPosition(window);
        ship.pointAttachmentAt(renderTexture.mapPixelToCoords(mousepos));

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            auto mouse_pos = sf::Mouse::getPosition(window);
            auto world_pos = renderTexture.mapPixelToCoords(mouse_pos);
            // draw a line between the sprite and the mouse
            sf::Vertex line[] = {
                sf::Vertex(ship.mAttached.getPosition()),
                sf::Vertex(world_pos)
            };
            renderTexture.draw(line, 2, sf::PrimitiveType::Lines);
        }
        {
            {
                renderTexture.draw(shape);
                renderTexture.draw(ship);
                renderTexture.display();
            }
            window.draw(sf::Sprite(renderTexture.getTexture()));
            window.draw(info_text);
            window.draw(entry_text);
            ImGui::SFML::Render(window);
            window.display();
        }
    }

    ImGui::SFML::Shutdown();

    return 0;
}