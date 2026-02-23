#pragma once
#include "../character/character.h"
#include <iostream>
class enemy : public Character
{
private:
    int exp_reward;
    int coin_reward;
    int level;

public:
    enemy(std::string name, int hp, int attack, int mp, int exp_reward, int coin_reward, int level);
    int getexp_reward() const;
    int getcoin_reward() const;
    int getlevel() const;
};