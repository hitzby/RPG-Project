#include "enemy.h"
using namespace std;
enemy::enemy(string name, int hp, int attack, int mp, int exp_reward, int coin_reward, int level)
    : Character(name, hp, attack, mp), exp_reward(exp_reward), coin_reward(coin_reward), level(level) {}
int enemy::getexp_reward() const
{
    return exp_reward;
}
int enemy::getcoin_reward() const
{
    return coin_reward;
}
int enemy::getlevel() const
{
    return level;
}
