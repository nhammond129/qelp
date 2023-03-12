#pragma once

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace data {

struct TextureLoader {
    virtual ~TextureLoader() = default;
    virtual sf::Texture& load(const std::string& path) = 0;
};

using TextureCache = entt::resource_cache<sf::Texture>;e

};  // namespace data