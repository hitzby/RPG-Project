#include "skills.h"
#include "character.h"
#include "player.h"
#include <iostream>

using namespace std;
skill::skill(int skillid, int levelrequired, int costmp, int cooldown,
             int damage, int level, std::string skillname)
    : skillid(skillid), levelrequired(levelrequired), costmp(costmp),
      cooldown(cooldown), damage(damage), level(level), skillname(skillname),
      currentcooldown(0) {}
int skill::getskillid() const { return skillid; }
int skill::getlevelrequired() const { return levelrequired; }
int skill::getcostmp() const { return costmp; }
int skill::getcooldown() const { return cooldown; }
int skill::getcurrentcooldown() const { return currentcooldown; }
int skill::getdamage() const { return damage; }
int skill::getskilllevel() const { return level; }
std::string skill::getskillname() const { return skillname; }
void skill::setskillid(int skillid) { this->skillid = skillid; }
void skill::setlevelrequired(int levelrequired) {
  this->levelrequired = levelrequired;
}
void skill::setcostmp(int costmp) { this->costmp = costmp; }
void skill::setskilllevelup(int level) { this->level = level; }
void skill::setcooldown(int cooldown) { this->cooldown = cooldown; }
void skill::setdamage(int damage) { this->damage = damage; }
void skill::setskillname(std::string skillname) { this->skillname = skillname; }

int skill::getlevelUpCost() const { return getskilllevel() * 100; }
void skill::reducecooldown() {
  if (currentcooldown > 0) {
    currentcooldown--;
  }
}
void skill::resetcooldown() { currentcooldown = getcooldown(); }
bool skill::canUse(player &hero) const {
  return hero.getlevel() >= levelrequired && hero.getmp() >= costmp &&
         currentcooldown == 0;
}
void skill::resetcurrentcooltime() { currentcooldown = 0; }
void skill::levelUpSkill(int playerlevel, int costcoins) {
  if (playerlevel >= getskilllevel() + 1 && costcoins >= getlevelUpCost()) {
    setskilllevelup(getskilllevel() + 1);
    skill::setdamage(getdamage() + 10); // 每级增加10点伤害
    skill::setcostmp(getcostmp() + 2);  // 每级增加2点魔法消耗
    resetcurrentcooltime();             // 升级后重置冷却时间
    cout << getskillname() << " leveled up to level " << getskilllevel()
         << endl;
  }
}
void skill::learnSkill(int playerlevel) {
  if (playerlevel >= getlevelrequired()) {
    cout << "You have learned the skill: " << getskillname() << endl;
  } else {
    cout << "You need to be at least level " << getlevelrequired()
         << " to learn this skill." << endl;
  }
}
Warriorskill_1::Warriorskill_1() : skill(1, 10, 5, 3, 20, 1, "猛击") {}
bool Warriorskill_1::useSkill(player &hero, Character &target) {
  if (canUse(hero)) {
    cout << "You used " << getskillname() << " on " << target.getname() << endl;
    target.takedamage(getdamage() * getskilllevel());
    hero.setmp(hero.getmp() - getcostmp());
    resetcooldown();
    return true;
  }
  return false;
}

Warriorskill_2::Warriorskill_2() : skill(2, 15, 10, 4, 30, 1, "重击") {}
bool Warriorskill_2::useSkill(player &hero, Character &target) {
  if (canUse(hero)) {
    cout << "You used " << getskillname() << " on " << target.getname() << endl;
    target.takedamage(getdamage() * getskilllevel());
    hero.setmp(hero.getmp() - getcostmp());
    resetcooldown();
    return true;
  }
  return false;
}
Warriorskill_3::Warriorskill_3() : skill(3, 20, 15, 5, 40, 1, "狂暴") {}
bool Warriorskill_3::useSkill(player &hero, Character &target) {
  if (canUse(hero)) {
    cout << "You used " << getskillname() << " on " << target.getname() << endl;
    target.takedamage(getdamage() * getskilllevel());
    hero.setmp(hero.getmp() - getcostmp());
    resetcooldown();
    return true;
  }
  return false;
}
Mageskill_1::Mageskill_1() : skill(1, 10, 8, 3, 25, 1, "火球术") {}
bool Mageskill_1::useSkill(player &hero, Character &target) {
  if (canUse(hero)) {
    cout << "You used " << getskillname() << " on " << target.getname() << endl;
    target.takedamage(getdamage() * getskilllevel());
    hero.setmp(hero.getmp() - getcostmp());
    resetcooldown();
    return true;
  }
  return false;
}
Mageskill_2::Mageskill_2() : skill(2, 15, 12, 4, 35, 1, "冰霜新星") {}
bool Mageskill_2::useSkill(player &hero, Character &target) {
  if (canUse(hero)) {
    cout << "You used " << getskillname() << " on " << target.getname() << endl;
    target.takedamage(getdamage() * getskilllevel());
    hero.setmp(hero.getmp() - getcostmp());
    resetcooldown();
    return true;
  }
  return false;
}
Mageskill_3::Mageskill_3() : skill(3, 20, 18, 5, 50, 1, "闪电链") {}
bool Mageskill_3::useSkill(player &hero, Character &target) {
  if (canUse(hero)) {
    cout << "You used " << getskillname() << " on " << target.getname() << endl;
    target.takedamage(getdamage() * getskilllevel());
    hero.setmp(hero.getmp() - getcostmp());
    resetcooldown();
    return true;
  }
  return false;
}
Assassinskill_1::Assassinskill_1() : skill(1, 10, 6, 3, 15, 1, "穿刺") {}
bool Assassinskill_1::useSkill(player &hero, Character &target) {
  if (canUse(hero)) {
    cout << "You used " << getskillname() << " on " << target.getname() << endl;
    target.takedamage(getdamage() * getskilllevel());
    hero.setmp(hero.getmp() - getcostmp());
    resetcooldown();
    return true;
  }
  return false;
}
Assassinskill_2::Assassinskill_2() : skill(2, 15, 10, 4, 25, 1, "多重射击") {}
bool Assassinskill_2::useSkill(player &hero, Character &target) {
  if (canUse(hero)) {
    cout << "You used " << getskillname() << " on " << target.getname() << endl;
    target.takedamage(getdamage() * getskilllevel());
    hero.setmp(hero.getmp() - getcostmp());
    resetcooldown();
    return true;
  }
  return false;
}
Assassinskill_3::Assassinskill_3() : skill(3, 20, 14, 5, 35, 1, "爆炸箭") {}
bool Assassinskill_3::useSkill(player &hero, Character &target) {
  if (canUse(hero)) {
    cout << "You used " << getskillname() << " on " << target.getname() << endl;
    target.takedamage(getdamage() * getskilllevel());
    hero.setmp(hero.getmp() - getcostmp());
    resetcooldown();
    return true;
  }
  return false;
}