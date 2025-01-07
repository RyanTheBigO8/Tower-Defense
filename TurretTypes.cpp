#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "AudioHelper.hpp"
#include "BulletTypes.hpp"
#include "Group.hpp"
#include "TurretTypes.hpp"
#include "ShootingEffect.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"

///==========================================================================================================///
/// PlugGunTurret

const int PlugGunTurret::Price = 50;
PlugGunTurret::PlugGunTurret(float x, float y) :
    // TODO 3 (1/5): You can imitate the 2 files: 'PlugGunTurret.hpp', 'PlugGunTurret.cpp' to create a new turret.
    Turret("play/tower-base.png", "play/turret-6.png", x, y, 150, Price, 2, 0) {
    // Move center downward, since we the turret head is slightly biased upward
    Anchor.y += 8.0f / GetBitmapHeight();
}
void PlugGunTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new WoodBullet(Position + normalized * 36, diff, rotation, this));
    // TODO 4 (2/2): Add a ShootEffect here. Remember you need to include the class.
    getPlayScene()->EffectGroup->AddNewObject(new ShootingEffect(Position.x + normalized.x * 36, Position.y + normalized.y * 36));
    AudioHelper::PlayAudio("gun.wav");
}

///==========================================================================================================///
/// DualGunTurret

const int DualGunTurret::Price = 100;
DualGunTurret::DualGunTurret(float x, float y) :
    Turret("play/tower-base.png", "play/turret-7.png", x, y, 150, Price, 2, 1) {
    Anchor.y += 8.0f / GetBitmapHeight();
}
void DualGunTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    getPlayScene()->BulletGroup->AddNewObject(new DualWoodBullet(Position + normalized * 36, diff, rotation, this));
    getPlayScene()->EffectGroup->AddNewObject(new ShootingEffect(Position.x + normalized.x * 36, Position.y + normalized.y * 36));
    AudioHelper::PlayAudio("gun.wav");
}

///==========================================================================================================///
/// GreenTurret

const int GreenTurret::Price = 150;
GreenTurret::GreenTurret(float x, float y) :
    Turret("play/tower-base.png", "play/turret-1.png", x, y, 275, Price, 3, 2) {
    Anchor.y += 8.0f / GetBitmapHeight();
}
void GreenTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    getPlayScene()->BulletGroup->AddNewObject(new CannonFire(Position + normalized * 36, diff, rotation, this));
    getPlayScene()->EffectGroup->AddNewObject(new ShootingEffect(Position.x + normalized.x * 36, Position.y + normalized.y * 36));
    AudioHelper::PlayAudio("missile.wav");
}

///==========================================================================================================///
/// RedTurret

const int RedTurret::Price = 300;
RedTurret::RedTurret(float x, float y) :
    Turret("play/tower-base.png", "play/turret-2.png", x, y, 150, Price, 0.5, 3) {
    Anchor.y += 8.0f / GetBitmapHeight();
}
void RedTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    getPlayScene()->BulletGroup->AddNewObject(new MachineGunFire(Position + normalized * 36, diff, rotation, this));
    getPlayScene()->EffectGroup->AddNewObject(new ShootingEffect(Position.x + normalized.x * 36, Position.y + normalized.y * 36));
    AudioHelper::PlayAudio("missile.wav");
}

///==========================================================================================================///
/// Shovel

const int Shovel::Price = 0;
Shovel::Shovel(float x, float y) :
    Turret("play/shovel.png", "play/shovel.png", x, y, 50, Price, 0, 4) {
    //Anchor.y += 8.0f / GetBitmapHeight();
}

void Shovel::CreateBullet() {}

///==========================================================================================================///
/// Shifter

const int Shifter::Price = 10;
Shifter::Shifter(float x, float y) :
    Turret("play/move.png", "play/move.png", x, y, 50, Price, 0, 5) {
    //Anchor.y += 8.0f / GetBitmapHeight();
}

void Shifter::CreateBullet() {}

///==========================================================================================================///
/// SpinningTurret

const int SpinningTurret::Price = 100;
SpinningTurret::SpinningTurret(float x, float y) :
    Turret("play/tower-base.png", "play/turret-spin.png", x, y, 200, Price, 0.5, 6) {
    Anchor.y += 8.0f / GetBitmapHeight();
    BulletLeft = 0;
}
void SpinningTurret::CreateBullet() {
    float rotation = 0;
    Engine::Point diff1 = Engine::Point(0, 1);
    Engine::Point diff2 = Engine::Point(-1, 0);
    Engine::Point diff3 = Engine::Point(0, -1);
    Engine::Point diff4 = Engine::Point(1, 0);
    getPlayScene()->BulletGroup->AddNewObject(new SpinBullet( Engine::Point(Position.x+120, Position.y), diff1, 0, this, 0, Position.x, Position.y));
    getPlayScene()->BulletGroup->AddNewObject(new SpinBullet( Engine::Point(Position.x, Position.y-120), diff2, ALLEGRO_PI/2 , this, ALLEGRO_PI/2, Position.x, Position.y));
    getPlayScene()->BulletGroup->AddNewObject(new SpinBullet( Engine::Point(Position.x-120, Position.y), diff3, ALLEGRO_PI, this, ALLEGRO_PI, Position.x, Position.y));
    getPlayScene()->BulletGroup->AddNewObject(new SpinBullet( Engine::Point(Position.x, Position.y+120), diff4, ALLEGRO_PI*3/2, this, ALLEGRO_PI*3/2, Position.x, Position.y));
    AudioHelper::PlayAudio("laser.wav");
}

///==========================================================================================================///
/// Freeze

const int Freeze::Price = 50;
Freeze::Freeze(float x, float y) :
    Turret("play/snowflake.png", "play/snowflake.png", x, y, 1600, Price, 0, 7) {
    //Anchor.y += 8.0f / GetBitmapHeight();
}

void Freeze::CreateBullet() {}
