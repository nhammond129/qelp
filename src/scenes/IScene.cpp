#include <exception>
#include <imgui-SFML.h>
#include <imgui.h>
#include <scenes/IScene.hpp>

namespace scenes {

sf::Clock IScene::mImguiClock;

sf::Clock IScene::mClock;
sf::RenderWindow IScene::mWindow;

void IScene::init_imgui() {
    if (!ImGui::SFML::Init(IScene::mWindow)) {
		throw std::runtime_error("Failed to initialize ImGui");
	}
}

void IScene::handleEvent_imgui(const sf::Event& event) {
    ImGui::SFML::ProcessEvent(mWindow, event);
}

void IScene::update_imgui() {
    ImGui::SFML::Update(IScene::mWindow, IScene::mImguiClock.restart());
}

void IScene::draw_imgui() {
    ImGui::SFML::Render(IScene::mWindow);
}

void IScene::shutdown_imgui() {
    ImGui::SFML::Shutdown(IScene::mWindow);
}

};  // namespace scenes
