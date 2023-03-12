#pragma once

#include <SFML/Graphics.hpp>
#ifdef __linux__
#include <unistd.h>
#elif __APPLE__
#include <mach-o/dyld.h>    // Not sure if this is correct. I don't have a mac to test this on.
#elif _WIN32
#include <Windows.h>
#endif

namespace {

const size_t PATH_MAX = 4096;
const std::string executable_path() {
#ifdef __linux__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count < 0)
        throw std::runtime_error("Error getting executable path");
    return std::string(result, count);
#elif __APPLE__
    char result[PATH_MAX];
    uint32_t size = sizeof(result);
    if (_NSGetExecutablePath(result, &size) != 0)
        throw std::runtime_error("Error getting executable path");
    return std::string(result);
#elif _WIN32
    char result[PATH_MAX];
    if (!GetModuleFileNameA(NULL, result, PATH_MAX))
        throw std::runtime_error("Error getting executable path: " + std::to_string(GetLastError()));
    return std::string(result);
#endif
}

};  // namespace anonymous
namespace data {

template <typename T>
struct Cache {
    using Ptr = std::shared_ptr<T>;
    using Map = std::unordered_map<std::string, Ptr>;

    Map mCache;

    /**
     * @brief Load a resource from the given filepath.
     * @param path The path to the file.
     * @return A reference to the loaded file's resource.
     * @throws std::runtime_error if the file could not be loaded.
     * @note If the resource has already been loaded, the cached version will be returned.
     * @note The path is relative to the executable's directory.
     * 
     * Example:
     * @code
     * // ${CMAKE_BINARY_DIR}
     * // ├─ main
     * // ├─ textures
     * // │  └─ player.png
     * Cache<sf::Texture> Textures;
     * const auto& texture = Textures.load("textures/player.png");
     * @endcode
     **/
    const T& load(const std::string& path) {
        auto it = mCache.find(path);
        if (it != mCache.end())
            return *(it->second);
        Ptr ptr = std::make_shared<T>();
        if (!ptr->loadFromFile(executable_path() + "/../" + path))
            throw std::runtime_error("Error loading file: " + path);
        mCache[path] = ptr;
        return *ptr;
    }
    void clear() {
        mCache.clear();
    }

    const T& operator[](const std::string& path) {
        return load(path);
    }
};

inline static Cache<sf::Texture> Textures;
inline static Cache<sf::Font> Fonts;

};  // namespace data