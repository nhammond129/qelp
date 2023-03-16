#pragma once

#include <SFML/Graphics.hpp>

struct Transformable {
    sf::Transformable& transformable;

    // Below: pass-through functions for sf::Transformable //

    /** @see sf::Transformable::setPosition */
    void setPosition(const sf::Vector2f& position) { transformable.setPosition(position); }
    /** @see sf::Transformable::setRotation */
    void setRotation(sf::Angle angle) { transformable.setRotation(angle); }
    /** @see sf::Transformable::setScale */
    void setScale(const sf::Vector2f& factors) { transformable.setScale(factors); }
    /** @see sf::Transformable::setOrigin */
    void setOrigin(const sf::Vector2f& origin) { transformable.setOrigin(origin); }
    /** @see sf::Transformable::getPosition */
    const sf::Vector2f& getPosition() const { return transformable.getPosition(); }
    /** @see sf::Transformable::getRotation */
    sf::Angle getRotation() const { return transformable.getRotation(); }
    /** @see sf::Transformable::getScale */
    const sf::Vector2f& getScale() const { return transformable.getScale(); }
    /** @see sf::Transformable::getOrigin */
    const sf::Vector2f& getOrigin() const { return transformable.getOrigin(); }
    /** @see sf::Transformable::move */
    void move(const sf::Vector2f& offset) { transformable.move(offset); }
    /** @see sf::Transformable::rotate */
    void rotate(sf::Angle angle) { transformable.rotate(angle); }
    /** @see sf::Transformable::scale */
    void scale(const sf::Vector2f& factors) { transformable.scale(factors); }
    /** @see sf::Transformable::getTransform */
    const sf::Transform& getTransform() const { return transformable.getTransform(); }
    /** @see sf::Transformable::getInverseTransform */
    const sf::Transform& getInverseTransform() const { return transformable.getInverseTransform(); }

    operator sf::Transformable&() { return transformable; }
};