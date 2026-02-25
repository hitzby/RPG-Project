#include <iostream>
#include "character.h"
using namespace std;
Character::Character(string name, int hp, int attack, int mp)
{
    this->name = name;
    this->hp = hp;
    this->attack = attack;
    this->mp = mp;
    this->max_hp = hp;
    this->max_mp = mp;
}
string Character::getname() const
{
    return name;
}
int Character::gethp() const
{
    return hp;
}
int Character::getattack() const
{
    return attack;
}
int Character::getmp() const
{
    return mp;
}
void Character::takedamage(int damage)
{
    hp -= damage;
    if (hp < 0)
        hp = 0;
    cout << getname() << "受到了" << damage << "点伤害，剩余HP:" << hp << endl;
}
void Character::attacktarget(Character &target)
{
    cout << getname() << "攻击了" << target.getname() << "！" << endl;
    target.takedamage(attack);
}
bool Character::isalive() const
{
    return hp > 0;
}
int Character::getmaxhp() const
{
    return max_hp;
}
int Character::getmaxmp() const
{
    return max_mp;
}
