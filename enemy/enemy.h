#pragma once
#include "character.h"
#include <vector>
enum class Affix {
  STRONG,
  TOUGH,
  QUICK,
  EXPLOSIVE,
  POSIONOUS,
  HEALING,
  SHIELDING
};
class enemy : public Character {
private:
  int exp_reward;
  int coin_reward;
  int level;
  int dogeRate = 10;
  std::vector<Affix> affixes;

public:
  enemy(std::string name, int hp, int attack, int mp, int exp_reward,
        int coin_reward, int level);
  int getexp_reward() const;
  int getcoin_reward() const;
  int getlevel() const;
  int getdogeRate() const;
  void setdogeRate(int rate);
  Affix getRandomAffix() const;
  void setAffix(int floor);
  void resethp(int amount);
  void resetexp_reward(int amount);
  void resetcoin_reward(int amount);
  void resetattack(int amount);
  void resetall(int hp, int attack, int mp, int exp_reward, int coin_reward);
  virtual void takedamage(int damage) override;
  bool hasAffix(Affix affix) const;
  void displayAffixes() const;
  void heal(int amount);
};