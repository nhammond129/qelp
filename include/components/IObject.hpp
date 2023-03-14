#pragma once

#include <SFML/Graphics.hpp>

/*************************************
 **************** WIP ****************
 *************************************/

namespace components {

/**
 * @brief IObject base interface for most game objects
 * @note similar to sf::Sprite, it should never be constructed from a temp texture
 * 
 * basically an sf::Sprite with some extra functionality
 *  - animation
 *  - velocity
 **/
class IObject : public sf::Sprite {
    struct AnimFrame {
        const sf::Texture& texture;
        sf::IntRect rect;
        sf::Time duration;
    };
public:
    void updateAnimation(sf::Time dt);
private:
    std::forward_list<AnimFrame> mAnimationFrames;
    sf::Time mAnimationTime;  // current frame's time-spent
    sf::Vector2f mVelocity;
};

};  // namespace game