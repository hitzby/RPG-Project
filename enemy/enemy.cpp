#include "enemy.h"
#include "character.h"
#include <cstdlib>
#include <ostream>
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
  // 重新设计概率权重：危险词缀稀有但恐怖，普通词缀常见
  int r = rand() % 100;
  if (r < 25)
    return Affix::STRONG; // 25% 力量：攻击提升
  else if (r < 45)
    return Affix::TOUGH; // 20% 坚韧：HP提升
  else if (r < 60)
    return Affix::QUICK; // 15% 迅捷：闪避提升
  else if (r < 73)
    return Affix::POSIONOUS; // 13% 剧毒：持续伤害（危险）
  else if (r < 85)
    return Affix::HEALING; // 12% 治疗：战斗中回血（烦人）
  else if (r < 93)
    return Affix::EXPLOSIVE; //  8% 爆炸：死亡时反伤（惊喜）
  else
    return Affix::SHIELDING; //  7% 护盾：吸收伤害（最难缠）
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
  // 每次有 40% 基础概率触发词缀，高楼层概率更高
  for (int i = 0; i < generate_chance; i++) {
    int threshold = min(40 + floor / 5, 70); // 40%~70%，随楼层增加
    if ((rand() % 100) < threshold) {
      affixes.push_back(getRandomAffix());
      // 有词缀：经验和金币额外奖励
      resetexp_reward(getexp_reward() + getlevel() * 3);
      resetcoin_reward(getcoin_reward() + getlevel() * 2);
    }
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
  cout << "敌人被加持了！！！【词缀】" << endl;
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