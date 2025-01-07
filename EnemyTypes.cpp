#include <string>
#include "EnemyTypes.hpp"

RedNormalEnemy::RedNormalEnemy(int x, int y) : Enemy("play/enemy-10.png", x, y, 25, 50, 8, 15, 1) {}

// TODO 2 (1/3): You can imitate the 2 files: 'RedNormalEnemy.hpp', 'RedNormalEnemy.cpp' to create a new enemy.

BlueNormalEnemy::BlueNormalEnemy(int x, int y) : Enemy("play/enemy-9.png", x, y, 25, 80, 15, 30, 2) {}

GreenNormalEnemy::GreenNormalEnemy(int x, int y) : Enemy("play/enemy-8.png", x, y, 25, 50, 20, 40, 3) {}

MiniBoss1::MiniBoss1(int x, int y) : Enemy("play/enemy-4.png", x, y, 25, 70, 25, 50, 4) {}


Dice1::Dice1(int x,int y) : Enemy("play/dice-1.png", x, y, 25, 105, 2, 10, 5) {}

Dice2::Dice2(int x,int y) : Enemy("play/dice-2.png", x, y, 25, 105, 4, 10, 6) {}

Dice3::Dice3(int x,int y) : Enemy("play/dice-3.png", x, y, 25, 90, 6, 10, 7) {}

Dice4::Dice4(int x,int y) : Enemy("play/dice-4.png", x, y, 25, 75, 8, 10, 8) {}

Dice5::Dice5(int x,int y) : Enemy("play/dice-5.png", x, y, 25, 60, 10, 10, 9) {}

Dice6::Dice6(int x,int y) : Enemy("play/dice-6.png", x, y, 25, 45, 12, 10, 10) {}
