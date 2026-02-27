#include "jobs.h"
#include "player.h"
#include "skills.h"
#include <cstdlib>
#include <iostream>
#include <memory>

using namespace std;
Warrior::Warrior(string name) : player(name, 150, 15, 10, 1, 0, 0, 10) {
  learnSkill(std::make_unique<Warriorskill_1>());
  learnSkill(std::make_unique<Warriorskill_2>());
  learnSkill(std::make_unique<Warriorskill_3>());
}
void Warrior::levelup() {
  level++;
  attack += 3;
  max_hp += 15;
  hp = max_hp;
  max_mp += 2;
  mp = max_mp;
  cout << getname() << "level up to " << level << endl;
}
string Warrior::getjob() const { return "Warrior"; }
Mage::Mage(string name) : player(name, 100, 10, 20, 1, 0, 0, 15) {
  learnSkill(std::make_unique<Mageskill_1>());
  learnSkill(std::make_unique<Mageskill_2>());
  learnSkill(std::make_unique<Mageskill_3>());
}
void Mage::levelup() {
  level++;
  attack += 2;
  max_hp += 10;
  mp += 5;
  hp = max_hp;
  max_mp += 5;
  mp = max_mp;
  cout << getname() << "level up to " << level << endl;
}
string Mage::getjob() const { return "Mage"; }
Assassin::Assassin(string name) : player(name, 120, 12, 15, 1, 0, 0, 20) {
  learnSkill(std::make_unique<Assassinskill_1>());
  learnSkill(std::make_unique<Assassinskill_2>());
  learnSkill(std::make_unique<Assassinskill_3>());
}
void Assassin::levelup() {
  level++;
  attack += 4;
  max_hp += 12;
  hp = max_hp;
  max_mp += 3;
  mp = max_mp;
  cout << getname() << "level up to " << level << endl;
}
string Assassin::getjob() const { return "Assassin"; }
void Assassin::takedamage(int damage) {
  // Assassin 有20%几率闪避攻击
  int dodgeChance = rand() % 100;
  if (dodgeChance < dodgeRate) {
    cout << "被动【闪避】" << endl;
    return; // 闪避成功，不受伤害
  }
  // 没有闪避成功，正常扣血
  player::takedamage(damage);
}