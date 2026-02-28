#include "skills.h"
#include "enemy.h"
#include "player.h"
#include <algorithm>
#include <iostream>

using namespace std;

// ─────────────────────────────────────────────
//  基类实现
// ─────────────────────────────────────────────
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

// 升级费用：随技能等级线性增加，前期便宜，后期贵
int skill::getlevelUpCost() const { return getskilllevel() * 150; }

void skill::reducecooldown() {
  if (currentcooldown > 0)
    currentcooldown--;
}
void skill::resetcooldown() { currentcooldown = getcooldown(); }
void skill::resetcurrentcooltime() { currentcooldown = 0; }

bool skill::canUse(player &hero) const {
  return hero.getlevel() >= levelrequired && hero.getmp() >= costmp &&
         currentcooldown == 0;
}

void skill::levelUpSkill(int playerlevel, int costcoins) {
  if (playerlevel >= getskilllevel() + 1 && costcoins >= getlevelUpCost()) {
    setskilllevelup(getskilllevel() + 1);
    skill::setdamage(getdamage() + 15); // 每级+15伤害（原+10，更有升级动力）
    skill::setcostmp(getcostmp() + 3);  // 每级+3MP消耗（原+2）
    resetcurrentcooltime();
    cout << "【技能升级】" << getskillname() << " 升至 Lv." << getskilllevel()
         << "！伤害+" << getdamage() << " MP消耗+" << getcostmp() << endl;
  }
}

void skill::learnSkill(int playerlevel) {
  if (playerlevel >= getlevelrequired()) {
    cout << "习得技能：" << getskillname() << endl;
  } else {
    cout << "需要 " << getlevelrequired() << " 级才能学习 " << getskillname()
         << endl;
  }
}

// ─────────────────────────────────────────────
//  护盾伤害计算辅助（减少代码重复）
// ─────────────────────────────────────────────
static int calcShieldedDamage(int rawDmg, const enemy &target) {
  if (target.hasAffix(Affix::SHIELDING)) {
    int shield = 10 + target.getlevel() * 5;
    cout << "【护盾】吸收了 " << shield << " 点伤害！";
    int dmg = max(0, rawDmg - shield);
    if (dmg > 0)
      cout << "穿透护盾造成 " << dmg << " 点伤害！" << endl;
    else
      cout << "\n攻击被完全吸收！" << endl;
    return dmg;
  }
  return rawDmg;
}

// ══════════════════════════════════════════════
//  战士技能
//  普攻参考：初始18，10级≈58攻击
//  技能1【猛击】：直接伤害，低CD，战斗主力
//  技能2【战吼】：高伤害+自身回血，中CD
//  技能3【狂暴斩】：超高伤害，长CD，5回合冷却
// ══════════════════════════════════════════════

// skill(id, levelReq, costMP, cooldown, damage, level, name)
Warriorskill_1::Warriorskill_1() : skill(1, 1, 6, 2, 30, 1, "猛击") {}
bool Warriorskill_1::useSkill(player &hero, enemy &target) {
  if (!canUse(hero))
    return false;
  int raw = getdamage() * getskilllevel();
  int dmg = calcShieldedDamage(raw, target);
  cout << "【猛击】" << hero.getname() << " 奋力一击，造成 " << dmg
       << " 点伤害！" << endl;
  if (dmg > 0)
    target.takedamage(dmg);
  hero.setmp(hero.getmp() - getcostmp());
  resetcooldown();
  return true;
}

Warriorskill_2::Warriorskill_2() : skill(2, 8, 14, 3, 55, 1, "战吼斩") {}
bool Warriorskill_2::useSkill(player &hero, enemy &target) {
  if (!canUse(hero))
    return false;
  int raw = getdamage() * getskilllevel();
  int dmg = calcShieldedDamage(raw, target);
  cout << "【战吼斩】" << hero.getname() << " 怒吼而出，造成 " << dmg
       << " 点伤害！" << endl;
  if (dmg > 0)
    target.takedamage(dmg);
  // 特效：斩击后回复自身10%最大HP
  int healAmt = max(5, hero.getmaxhp() / 10);
  hero.sethp(min(hero.getmaxhp(), hero.gethp() + healAmt));
  cout << "战吼激励！恢复了 " << healAmt << " 点HP！" << endl;
  hero.setmp(hero.getmp() - getcostmp());
  resetcooldown();
  return true;
}

Warriorskill_3::Warriorskill_3() : skill(3, 18, 25, 5, 100, 1, "狂暴斩") {}
bool Warriorskill_3::useSkill(player &hero, enemy &target) {
  if (!canUse(hero))
    return false;
  int raw = getdamage() * getskilllevel();
  int dmg = calcShieldedDamage(raw, target);
  cout << "【狂暴斩】" << hero.getname() << " 释放全力，造成 " << dmg
       << " 点毁灭性伤害！" << endl;
  if (dmg > 0)
    target.takedamage(dmg);
  hero.setmp(hero.getmp() - getcostmp());
  resetcooldown();
  return true;
}

// ══════════════════════════════════════════════
//  法师技能
//  特点：高基础伤害，MP消耗较高（MP资源管理）
//  技能1【火球术】：稳定单体，2回合CD
//  技能2【冰霜新星】：伤害+使敌人下回合攻击力-30%（用文字体现）
//  技能3【闪电链】：最高伤害，5回合CD
// ══════════════════════════════════════════════
Mageskill_1::Mageskill_1() : skill(1, 1, 10, 2, 40, 1, "火球术") {}
bool Mageskill_1::useSkill(player &hero, enemy &target) {
  if (!canUse(hero))
    return false;
  int raw = getdamage() * getskilllevel();
  int dmg = calcShieldedDamage(raw, target);
  cout << "【火球术】烈焰轰击，对 " << target.getname() << " 造成 " << dmg
       << " 点火焰伤害！" << endl;
  if (dmg > 0)
    target.takedamage(dmg);
  hero.setmp(hero.getmp() - getcostmp());
  resetcooldown();
  return true;
}

Mageskill_2::Mageskill_2() : skill(2, 8, 18, 3, 70, 1, "冰霜新星") {}
bool Mageskill_2::useSkill(player &hero, enemy &target) {
  if (!canUse(hero))
    return false;
  int raw = getdamage() * getskilllevel();
  int dmg = calcShieldedDamage(raw, target);
  cout << "【冰霜新星】冰晶爆裂，对 " << target.getname() << " 造成 " << dmg
       << " 点冰霜伤害！" << endl;
  if (dmg > 0)
    target.takedamage(dmg);
  // 特效：冻结效果，减少敌方攻击力
  int freeze = max(3, target.getattack() / 5);
  target.resetattack(max(1, target.getattack() - freeze));
  cout << "冰封！" << target.getname() << " 的攻击力降低了 " << freeze
       << " 点！" << endl;
  hero.setmp(hero.getmp() - getcostmp());
  resetcooldown();
  return true;
}

Mageskill_3::Mageskill_3() : skill(3, 18, 35, 5, 130, 1, "闪电链") {}
bool Mageskill_3::useSkill(player &hero, enemy &target) {
  if (!canUse(hero))
    return false;
  int raw = getdamage() * getskilllevel();
  int dmg = calcShieldedDamage(raw, target);
  cout << "【闪电链】天雷降临，对 " << target.getname() << " 造成 " << dmg
       << " 点雷电伤害！" << endl;
  if (dmg > 0)
    target.takedamage(dmg);
  hero.setmp(hero.getmp() - getcostmp());
  resetcooldown();
  return true;
}

// ══════════════════════════════════════════════
//  刺客技能
//  特点：单次高爆发，结合25%暴击率
//  技能1【穿刺】：快速刺击，2回合CD
//  技能2【影袭】：高伤害，自身进入隐蔽(下回合暴击必发，用文字体现)
//  技能3【爆炸箭雨】：超高伤害，5回合CD
// ══════════════════════════════════════════════
Assassinskill_1::Assassinskill_1() : skill(1, 1, 8, 2, 35, 1, "穿刺") {}
bool Assassinskill_1::useSkill(player &hero, enemy &target) {
  if (!canUse(hero))
    return false;
  int raw = getdamage() * getskilllevel();
  int dmg = calcShieldedDamage(raw, target);
  cout << "【穿刺】暗影一刺，对 " << target.getname() << " 造成 " << dmg
       << " 点伤害！" << endl;
  if (dmg > 0)
    target.takedamage(dmg);
  hero.setmp(hero.getmp() - getcostmp());
  resetcooldown();
  return true;
}

Assassinskill_2::Assassinskill_2() : skill(2, 8, 16, 3, 65, 1, "影袭") {}
bool Assassinskill_2::useSkill(player &hero, enemy &target) {
  if (!canUse(hero))
    return false;
  int raw = getdamage() * getskilllevel();
  // 特效：影袭必定暴击
  int dmg = calcShieldedDamage(raw * 2, target);
  cout << "【影袭·必定暴击】从暗影中突刺！对 " << target.getname() << " 造成 "
       << dmg << " 点暴击伤害！" << endl;
  if (dmg > 0)
    target.takedamage(dmg);
  hero.setmp(hero.getmp() - getcostmp());
  resetcooldown();
  return true;
}

Assassinskill_3::Assassinskill_3() : skill(3, 18, 30, 5, 110, 1, "死亡标记") {}
bool Assassinskill_3::useSkill(player &hero, enemy &target) {
  if (!canUse(hero))
    return false;
  int raw = getdamage() * getskilllevel();
  int dmg = calcShieldedDamage(raw, target);
  cout << "【死亡标记】致命一击！对 " << target.getname() << " 造成 " << dmg
       << " 点伤害！" << endl;
  if (dmg > 0)
    target.takedamage(dmg);
  hero.setmp(hero.getmp() - getcostmp());
  resetcooldown();
  return true;
}