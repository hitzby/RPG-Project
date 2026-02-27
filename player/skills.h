#pragma once
#include "character.h"
#include <string>
class player;
class skill {
  int skillid;
  int levelrequired;
  int level;
  int costmp;
  int cooldown;
  int currentcooldown;
  int damage;
  std::string skillname;

public:
  skill(int skillid, int levelrequired, int costmp, int cooldown, int damage,
        int level, std::string skillname);
  int getskillid() const;
  int getlevelrequired() const;
  int getcostmp() const;
  int getcooldown() const;
  int getdamage() const;
  int getlevelUpCost() const;
  int getskilllevel() const;
  std::string getskillname() const;
  void setskillid(int skillid);
  int getcurrentcooldown() const;
  void setlevelrequired(int levelrequired);
  void setcostmp(int costmp);
  void setcooldown(int cooldown);
  void setdamage(int damage);
  void resetcurrentcooltime();
  void setskillname(std::string skillname);
  void reducecooldown();
  void setskilllevelup(int level);
  void resetcooldown();
  bool canUse(player &hero) const;
  virtual ~skill() = default;
  virtual bool useSkill(player &hero, Character &target) = 0;
  void levelUpSkill(int playerLevel, int costcoins);
  void learnSkill(int playerlevel);
};
class Warriorskill_1 : public skill {
public:
  Warriorskill_1();
  bool useSkill(player &hero, Character &target) override;
};
class Warriorskill_2 : public skill {
public:
  Warriorskill_2();
  bool useSkill(player &hero, Character &target) override;
};
class Warriorskill_3 : public skill {
public:
  Warriorskill_3();
  bool useSkill(player &hero, Character &target) override;
};
class Mageskill_1 : public skill {
public:
  Mageskill_1();
  bool useSkill(player &hero, Character &target) override;
};
class Mageskill_2 : public skill {
public:
  Mageskill_2();
  bool useSkill(player &hero, Character &target) override;
};
class Mageskill_3 : public skill {
public:
  Mageskill_3();
  bool useSkill(player &hero, Character &target) override;
};
class Assassinskill_1 : public skill {
public:
  Assassinskill_1();
  bool useSkill(player &hero, Character &target) override;
};
class Assassinskill_2 : public skill {
public:
  Assassinskill_2();
  bool useSkill(player &hero, Character &target) override;
};
class Assassinskill_3 : public skill {
public:
  Assassinskill_3();
  bool useSkill(player &hero, Character &target) override;
};