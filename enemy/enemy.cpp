#include "enemy.h"
#include "character.h"
#include <cstdlib>
using namespace std;
#include <iostream>
enemy::enemy(string name, int hp, int attack, int mp, int exp_reward,
             int coin_reward, int level)
    : Character(name, hp, attack, mp), exp_reward(exp_reward),
      coin_reward(coin_reward), level(level) {}
int enemy::getexp_reward() const { return exp_reward; }
void enemy::takedamage(int damage) {
  int dodgeChance = rand() % 100;
  if (dodgeChance < dogeRate) {
    cout << getname() << "闪避了攻击！" << endl;
    return; // 闪避成功，不受伤害
  }
  Character::takedamage(damage);
}
int enemy::getcoin_reward() const { return coin_reward; }
int enemy::getlevel() const { return level; }
void enemy::resethp(int amount) {
  hp = amount;
  max_hp = amount;
}
void enemy::resetexp_reward(int amount) { exp_reward = amount; }
void enemy::resetcoin_reward(int amount) { coin_reward = amount; }
void enemy::resetattack(int amount) { attack = amount; }
void enemy::resetall(int hp, int attack, int mp, int exp_reward,
                     int coin_reward) {
  resethp(hp);
  resetattack(attack);
  this->mp = mp;
  resetexp_reward(exp_reward);
  resetcoin_reward(coin_reward);
}
int enemy::getdogeRate() const { return dogeRate; }
void enemy::setdogeRate(int rate) { dogeRate = rate; }
Affix enemy::getRandomAffix() const {
  int randomNUm = rand() % 100;
  if (randomNUm < 20) {
    return Affix::STRONG; // 20% 概率
  } else if (randomNUm < 40) {
    return Affix::TOUGH; // 20% 概率
  } else if (randomNUm < 60) {
    return Affix::QUICK; // 20% 概率
  } else if (randomNUm < 80) {
    return Affix::EXPLOSIVE; // 20% 概率
  } else if (randomNUm < 90) {
    return Affix::POSIONOUS; // 10% 概率
  } else if (randomNUm < 95) {
    return Affix::HEALING; // 5% 概率
  } else {
    return Affix::SHIELDING; // 5% 概率
  }
}
void enemy::setAffix(int floor) {
  int generate_chance = 0;
  if (floor >= 1 && floor < 10) {
    generate_chance = 1;
  }
  if (floor >= 10 && floor < 20) {
    generate_chance = 2;
  }
  if (floor >= 20 && floor < 40) {
    generate_chance = 3;
  }
  if (floor >= 40 && floor < 60) {
    generate_chance = 4;
  }
  if (floor >= 60 && floor < 80) {
    generate_chance = 5;
  }
  if (floor >= 80 && floor <= 100) {
    generate_chance = 6;
  }
  for (int i = 0; i < generate_chance; i++) {
    int randomNum = (rand() % 100 + floor) % 100;
    int count = 0;
    if (randomNum < 20) {
      affixes.push_back(getRandomAffix());
      count++;
    }
    resetexp_reward(getexp_reward() + (getlevel()) * count);
    resetcoin_reward(getcoin_reward() + (getlevel()) * count);
  }
}
bool enemy::hasAffix(Affix affix) const {
  for (const auto &a : affixes) {
    if (a == affix) {
      return true;
    }
  }
  return false;
}
void enemy::displayAffixes() const {
  if (affixes.empty()) {
    cout << "[普通]";
    return;
  }
  cout << "【词缀】";
  for (auto aff : affixes) {
    switch (aff) {
    case Affix::STRONG:
      cout << "[强力] ";
      break;
    case Affix::TOUGH:
      cout << "[坚韧] ";
      break;
    case Affix::QUICK:
      cout << "[迅捷] ";
      break;
    case Affix::EXPLOSIVE:
      cout << "[爆炸] ";
      break;
    case Affix::POSIONOUS:
      cout << "[剧毒] ";
      break;
    case Affix::HEALING:
      cout << "[治疗] ";
      break;
    case Affix::SHIELDING:
      cout << "[护盾] ";
      break;
    }
  }
  cout << endl;
}
void enemy::heal(int amount) {
  hp += amount;
  if (hp > max_hp)
    hp = max_hp;
}