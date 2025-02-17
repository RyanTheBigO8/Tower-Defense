#include <allegro5/color.h>

#include "GameEngine.hpp"
#include "IScene.hpp"
#include "PlayScene.hpp"
#include "ToolButton.hpp"

PlayScene* ToolButton::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
ToolButton::ToolButton(std::string img, std::string imgIn, Engine::Sprite Tool, float x, float y, int money) :
	ImageButton(img, imgIn, x, y), Tool(Tool), money(money) {
}

void ToolButton::Update(float deltaTime) {
	ImageButton::Update(deltaTime);
	if (getPlayScene()->GetMoney() >= money) {
		Tool.Tint = al_map_rgba(255, 255, 255, 255);
	} else {
		Tool.Tint = al_map_rgba(0, 0, 0, 160);
	}
}

void ToolButton::Draw() const {
	ImageButton::Draw();
	Tool.Draw();
}
