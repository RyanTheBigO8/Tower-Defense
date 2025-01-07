#ifndef TURRETTYPES_HPP
#define TURRETTYPES_HPP
#include "Turret.hpp"

class PlugGunTurret : public Turret {
public:
    static const int Price;
    PlugGunTurret(float x, float y);
    void CreateBullet() override;
};

class DualGunTurret : public Turret {
public:
    static const int Price;
    DualGunTurret(float x, float y);
    void CreateBullet() override;
};

class GreenTurret : public Turret {
public:
    static const int Price;
    GreenTurret(float x, float y);
    void CreateBullet() override;
};

class RedTurret : public Turret {
public:
    static const int Price;
    RedTurret(float x, float y);
    void CreateBullet() override;
};

class Shovel : public Turret {
public:
    static const int Price;
    Shovel(float x, float y);
    void CreateBullet() override;
};

class Shifter : public Turret {
public:
    static const int Price;
    Shifter(float x, float y);
    void CreateBullet() override;
};

class SpinningTurret : public Turret {
public:
    static const int Price;
    SpinningTurret(float x, float y);
    void CreateBullet() override;
};

class Freeze : public Turret {
public:
    static const int Price;
    Freeze(float x, float y);
    void CreateBullet() override;
};
#endif
