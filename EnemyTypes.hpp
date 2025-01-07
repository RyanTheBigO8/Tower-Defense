#ifndef ENEMYTYPES_HPP
#define ENEMYTYPES_HPP
#include "Enemy.hpp"

class RedNormalEnemy : public Enemy {
public:
    RedNormalEnemy(int x, int y);
};

class BlueNormalEnemy : public Enemy {
public:
    BlueNormalEnemy(int x, int y);
};

class GreenNormalEnemy : public Enemy {
public:
    GreenNormalEnemy(int x, int y);
};

class MiniBoss1 : public Enemy {
public:
    MiniBoss1(int x, int y);
};

class Dice1 : public Enemy {
public:
    Dice1(int x, int y);
};

class Dice2 : public Enemy {
public:
    Dice2(int x, int y);
};

class Dice3 : public Enemy {
public:
    Dice3(int x, int y);
};

class Dice4 : public Enemy {
public:
    Dice4(int x, int y);
};

class Dice5 : public Enemy {
public:
    Dice5(int x, int y);
};

class Dice6 : public Enemy {
public:
    Dice6(int x, int y);
};

#endif // REDNORMALENEMY_HPP
