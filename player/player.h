#pragma once
#include "../character/character.h"
class player : public Character {
private:
  int level;
  int exp;
  int coin;
  int free_revives;
  int critRate;

public:
  player(std::string name, int hp, int attack, int mp, int level, int exp,
         int coin);
  int getlevel() const;
  int getexp() const;
  int getcoin() const;
  void gainexp(int exp);
  void gaincoin(int coin);
  void levelup();
  void costcoin(int amount);
  void revive();
  int getfreerevives() const;
  void decreaserevives();
  void resetFreeRevive();
  void printstats() const;
  void heal(int amount);                                 // 战后回血
  int getcritRate() const;                               // 获取暴击率
  virtual void attacktarget(Character &target) override; // 重写攻击函数
};
