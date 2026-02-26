#include "player.h"
#include "character.h"
#include <ctime>
#include <iostream>

using namespace std;
player::player(std::string name, int hp, int attack, int mp, int level, int exp,
               int coin, int critRate)
    : Character(name, hp, attack, mp), level(level), exp(exp), coin(coin),
      free_revives(3), critRate(critRate) {}
int player::getlevel() const { return level; }
int player::getexp() const { return exp; }
int player::getcoin() const { return coin; }
int player::getfreerevives() const { return free_revives; }
int player::getcritRate() const { return critRate; }
void player::decreaserevives() {
  if (free_revives > 0)
    free_revives--;
}
void player::resetFreeRevive() { free_revives = 3; }
void player::gainexp(int amount) {
  this->exp += amount;
  cout << getname() << "gain" << amount << "points of exp!" << endl;
  // 改成 level * 50，升级速度提升一倍！
  while (this->exp >= level * 50) {
    exp -= level * 50;
    levelup();
  }
}
void player::gaincoin(int coin) {
  this->coin += coin;
  cout << getname() << "gain" << coin << "coins!" << endl;
}
void player::levelup() {
  level++;
  attack += 2;
  max_hp += 10;
  hp = max_hp;
  cout << getname() << "level up to" << level << endl;
}
void player::costcoin(int amount) {
  if (coin >= amount) {
    coin -= amount;
    cout << getname() << "cost" << amount << "coins" << endl;
  } else {
    cout << getname() << "doesn't have enough coins!" << endl;
  }
}
void player::revive() {
  hp = max_hp;
  cout << getname() << "revived with " << hp << " HP!" << endl;
}
void player::printstats() const {
  cout << "=== " << getname() << "'s Stats ===" << endl;
  cout << "Level: " << level << endl;
  cout << "HP: " << hp << "/" << max_hp << endl;
  cout << "Attack: " << attack << endl;
  cout << "MP: " << mp << endl;
  cout << "EXP: " << exp << endl;
  cout << "Coins: " << coin << endl;
  cout << "Free Revives Left: " << getfreerevives() << endl;
  cout << "Crit Rate: " << critRate << "%" << endl;
}
void player::heal(int amount) {
  hp += amount;
  if (hp > max_hp)
    hp = max_hp; // 别加爆了
  cout << "【被动】击杀回复！HP 恢复了 " << amount << " 点。" << endl;
}
void player::attacktarget(Character &target) {
  int damage = attack;
  bool isCrit = (rand() % 100 < critRate);
  if (isCrit) {
    damage *= 2;
    cout << "【暴击】" << endl;
  }
  cout << getname() << " attacks " << target.getname() << " for " << damage
       << " damage!" << endl;
  target.takedamage(damage);
}