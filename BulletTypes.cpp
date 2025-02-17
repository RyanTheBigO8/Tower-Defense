#include <allegro5/base.h>
#include <random>
#include <string>

#include "DirtyEffect.hpp"
#include "Enemy.hpp"
#include "BulletTypes.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"

class Turret;

///==========================================================================================================///
/// WoodBullet

WoodBullet::WoodBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent) :
    Bullet("play/bullet-8.png", 300, 3, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent, clock, 0, 0) {
    // TODO 3 (2/5): You can imitate the 2 files: 'WoodBullet.hpp', 'WoodBullet.cpp' to create a new bullet.
}
void WoodBullet::OnExplode(Enemy* enemy) {
    enemy->Slow(0.8, 2);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

///==========================================================================================================///
/// DualWoodBullet

DualWoodBullet::DualWoodBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent) :
    Bullet("play/bullet-8a.png", 300, 6, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent, clock, 0, 0) {
}
void DualWoodBullet::OnExplode(Enemy* enemy) {
    enemy->Slow(0.8, 2);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

///==========================================================================================================///
/// CannonFire

CannonFire::CannonFire(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent) :
    Bullet("play/bullet-1.png", 350, 10, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent, clock, 0, 0) {
}
void CannonFire::OnExplode(Enemy* enemy) {
    enemy->Slow(0.7, 3);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-2.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

///==========================================================================================================///
/// MachineGunFire

MachineGunFire::MachineGunFire(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent) :
    Bullet("play/bullet-2a.png", 350, 3, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent, clock, 0, 0) {
}
void MachineGunFire::OnExplode(Enemy* enemy) {
    enemy->Slow(0.8, 2);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-2.png", dist(rng), enemy->Position.x, enemy->Position.y));
}


///==========================================================================================================///
/// SpinBullet

SpinBullet::SpinBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent, float phase, float parent_x, float parent_y) :
    Bullet("play/bullet-7.png", 0, 3, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent, phase, parent_x, parent_y) {
}
void SpinBullet::OnExplode(Enemy* enemy) {
    enemy->Slow(0.8, 2);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

