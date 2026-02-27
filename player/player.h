#pragma once
#include "../character/character.h"
#include "../item/item.h"
#include <memory>
#include <vector>
class skill;
class player : public Character {
protected:
  int level;
  int exp;
  int coin;
  int free_revives;
  int critRate;
  std::vector<Item> backpack;
  std::vector<Item> equipments;
  std::vector<std::unique_ptr<skill>> skills;

public:
  player(std::string name, int hp, int attack, int mp, int level, int exp,
         int coin, int critRate);
  virtual ~player();
  int getlevel() const;
  int getexp() const;
  int getcoin() const;
  void gainexp(int exp);
  void gaincoin(int coin);
  virtual void levelup();
  virtual std::string getjob() const = 0;
  void costcoin(int amount);
  void revive();
  int getfreerevives() const;
  void decreaserevives();
  void resetFreeRevive();
  void printstats() const;
  void heal(int amount);
  int getcritRate() const;
  virtual void attacktarget(Character &target) override;
  virtual void takedamage(int damage) override;
  void addItem(const Item &item);
  void printBackpack() const;
  void useItem(int index);
  void equipItem(int index);
  void unequipItem(int index);
  void showEquippedItems() const;
  void showSkills(player &hero) const;
  bool useSkill(int index, Character &target);
  void learnSkill(std::unique_ptr<skill> newSkill);
  skill *getSkill(int index) const;
  int getSkillCount() const;
  void reduceallCooldowns();
  void levelUpSkill(int index);
  void equipItem_T(const Item &item);
};
