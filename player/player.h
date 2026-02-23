#pragma once
#include "../character/character.h"
class player : public Character
{
    int level;
    int exp;
    int coin;

public:
    player(std::string name, int hp, int attack, int mp, int level, int exp, int coin);
    int getlevel() const;
    int getexp() const;
    int getcoin() const;
    void gainexp(int exp);
    void gaincoin(int coin);
    void levelup();
    void costcoin(int amount);
};
