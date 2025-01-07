#ifndef TURRET_HPP
#define TURRET_HPP
#include <allegro5/base.h>
#include <list>
#include <string>

#include "Sprite.hpp"
#include "Label.hpp"

class Enemy;
class PlayScene;
class Label;

class Turret: public Engine::Sprite {
protected:
    int price;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    Sprite imgBase;
    std::list<Turret*>::iterator lockedTurretIterator;
    PlayScene* getPlayScene();
    // Reference: Design Patterns - Factory Method.
    virtual void CreateBullet() = 0;

public:
    bool Enabled = true;
    bool Preview = false;
    Enemy* Target = nullptr;
    int turret_id;
    //Engine::Label* UIPrice;
    Turret(std::string imgBase, std::string imgTurret, float x, float y, float radius, int price, float coolDown, int id);
    void Update(float deltaTime) override;
    void Draw() const override;
	int GetPrice() const;
	void dig();
	void upgrade();
	int BulletLeft = 0;
	int Period = 5;
};
#endif // TURRET_HPP
