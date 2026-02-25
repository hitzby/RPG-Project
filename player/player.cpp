#include <iostream>
#include "player.h"
using namespace std;
player::player(std::string name, int hp, int attack, int mp, int level, int exp, int coin)
    : Character(name, hp, attack, mp), level(level), exp(exp), coin(coin) {}
int player::getlevel() const
{
    return level;
}
int player::getexp() const
{
    return exp;
}
int player::getcoin() const
{
    return coin;
}
void player::gainexp(int amount)
{
    this->exp += amount;
    cout << getname() << "获得了" << amount << "点经验值" << endl;
    while (this->exp >= level * 100)
    {
        exp -= level * 100;
        levelup();
    }
}
void player::gaincoin(int coin)
{
    this->coin += coin;
    cout << getname() << "获得了" << coin << "个金币" << endl;
}
void player::levelup()
{
    level++;
    attack += 2;
    max_hp += 10;
    hp = max_hp;
    cout << getname() << "升级了！当前等级:" << level << endl;
}
void player::costcoin(int amount)
{
    if (coin >= amount)
    {
        coin -= amount;
        cout << getname() << "花费了" << amount << "个金币" << endl;
    }
    else
    {
        cout << getname() << "的金币不足，无法完成操作" << endl;
    }
}
