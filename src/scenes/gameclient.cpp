#include <config.hpp>
#include <gameclient.hpp>
#include <scenes.hpp>
#include <SFML/Graphics.hpp>
#include <util.hpp>

namespace scenes {

GameClient::GameClient(SceneManager& manager):
        IScene(manager),
        mClient(sf::IpAddress{127, 0, 0, 1}/*localhost*/, config::SERVER_PORT),
        mServer(config::SERVER_PORT) {
    if (!mWorldRT.create(manager.window().getSize()))
        throw std::runtime_error("Error creating render texture");

    auto player = mGameState.createPlayer({0.f, 0.f});
    /*
    // add some random ents
    for (int i=0; i<100; i++) {
        mGameState.createPlayer(
            sf::Vector2f {
                (rand() % 100) / 100.f * 1000.f - 500.f,
                (rand() % 100) / 100.f * 1000.f - 500.f
            }
        );
    }
    */

    // center view on {0, 0}
    util::setViewCenter(mWorldRT, {0.f, 0.f});

    // start server thread
    mServer.serve_forever_nonblocking();

    // connect to server
        util::log("Connecting to server...");
    while (mClient.getState() != net::Client::State::Connected) {
        mClient.think(mGameState);
    }
    util::log("Connected to server!");
}

void GameClient::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::E) {
            // apply a tiny random offset force to some bodies
            auto& registry = mGameState.getRegistry();
            registry.view<game::b2BodyComponent>().each([&](auto entity, game::b2BodyComponent& body) {
                if (rand() % 100 > 10) return;  // 10% chance to wibble :)

                auto bodybounds = body.body->GetFixtureList()->GetAABB(0);
                body.body->ApplyForce(
                    100*body.body->GetMass()*b2Vec2(  // random force proportional to object's total mass
                        (rand() % 100) / 100.f - 0.5f,
                        (rand() % 100) / 100.f - 0.5f
                    ),
                    body.body->GetWorldPoint(  // get random force offset
                        b2Vec2(
                            (rand() % 100) / 100.f * (bodybounds.upperBound.x - bodybounds.lowerBound.x) + bodybounds.lowerBound.x,
                            (rand() % 100) / 100.f * (bodybounds.upperBound.y - bodybounds.lowerBound.y) + bodybounds.lowerBound.y
                        )
                    ),
                    true
                );
            });
        }
    }

    static struct {
        sf::Vector2i lastpos;
        bool active = false;
    } drag;
    if (mManager.window().hasFocus() == false) return;

    /*
    if (event.type == sf::Event::MouseMoved) {
        // set mCursorNearestEntity
        sf::Vector2f mouse_pos = mWorldRT.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
        float min_dist = std::numeric_limits<float>::max();
        mRegistry.view<Transformable>().each([&](auto entity, auto& transformable) {
            float dist = (transformable.getPosition() - mouse_pos).length();
            if (dist < min_dist) {
                min_dist = dist;
                mCursorNearestEntity = entity;
            }
        });
    }
    */

    std::optional<input::Action> maybe_action = input::getAction(event);
    if (maybe_action) {
        input::Action& action = *maybe_action;
        switch (action.type) {
            case input::Action::Type::ZoomIn: {
                auto view = mWorldRT.getView();
                view.zoom(1.f - action.zoom.amount * 0.1f);
                mWorldRT.setView(view);
                break;
            }
            case input::Action::Type::ZoomOut: {
                auto view = mWorldRT.getView();
                view.zoom(1.f + action.zoom.amount * 0.1f);
                mWorldRT.setView(view);
                break;
            }
            case input::Action::Type::ZoomReset: {
                auto view = mWorldRT.getView();
                view.setSize({(float)mManager.window().getSize().x, (float)mManager.window().getSize().y});
                mWorldRT.setView(view);
                break;
            }
            case input::Action::Type::MoveClick: {
                /*
                sf::Vector2f pos = mWorldRT.mapPixelToCoords(action.move.pos);
                mRegistry.view<PlayerActionQueue>().each([&](PlayerActionQueue& queue) {
                    if (!action.move.queue) queue.actions.clear();
                    queue.actions.push_back( {
                        .type = PlayerActionQueue::Action::Type::Move,
                        .move = {pos},
                    });
                    if (queue.actions.size() > PlayerActionQueue::MAX_DEPTH)
                        queue.actions.pop_front();
                });
                */
                break;
            }
            case input::Action::Type::ViewCenter: {
                /*
                mRegistry.view<ParentedView, sf::Sprite>().each([this](ParentedView& view, sf::Sprite& sprite) {
                    view.set_center(sprite.getPosition());
                });
                */
                break;
            }
        }
    }

    // drag-click logic
    // TODO: move this to input::getAction ?
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Middle) {
        drag.active = true;
        drag.lastpos = {event.mouseButton.x, event.mouseButton.y};
    } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Middle) {
        drag.active = false;
    } else if (event.type == sf::Event::MouseMoved && drag.active) {
        sf::Vector2f motionDelta = mWorldRT.mapPixelToCoords(drag.lastpos) - mWorldRT.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
        util::moveViewCenter(mWorldRT, motionDelta);
        drag.lastpos = {event.mouseMove.x, event.mouseMove.y};
    }
}

void GameClient::fixed_update(const sf::Time& dt) {
    // update b2world
    mGameState.update(dt);
}

void GameClient::update(const sf::Time& dt) {
    {   // fixed update
        sf::Time elapsed = dt;
        while (elapsed > sf::Time::Zero) {
            fixed_update(std::min(config::FIXED_UPDATE_INTERVAL, elapsed));
            elapsed -= config::FIXED_UPDATE_INTERVAL;
        }
    }
    // non-blocking network consumption
    mClient.think(mGameState);
}

void GameClient::draw(sf::RenderWindow& window) {
    mWorldRT.clear();
    {
        auto& registry = mGameState.getRegistry();
        registry.view<game::b2BodyComponent>().each([&](auto entity, auto& body) {
            body.draw(mWorldRT, sf::RenderStates::Default);
        });
    }
    mWorldRT.display();
    window.draw(sf::Sprite(mWorldRT.getTexture()));
}

};  // namespace scenes