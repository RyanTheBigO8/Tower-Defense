#ifndef BULLETTYPES_HPP
#define BULLETTYPES_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}  // namespace Engine

class WoodBullet : public Bullet {
public:
    explicit WoodBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
    void OnExplode(Enemy* enemy) override;
};

class DualWoodBullet : public Bullet {
public:
    explicit DualWoodBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
    void OnExplode(Enemy* enemy) override;
};

class CannonFire : public Bullet {
public:
    explicit CannonFire(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
    void OnExplode(Enemy* enemy) override;
};

class MachineGunFire : public Bullet {
public:
    explicit MachineGunFire(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
    void OnExplode(Enemy* enemy) override;
};


class SpinBullet : public Bullet {
public:
    explicit SpinBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent, float phase, float parent_x, float parent_y);
    void OnExplode(Enemy* enemy) override;
};


#endif
