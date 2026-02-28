#include "player.h"
#include "character.h"
#include "item.h"
#include "skills.h"
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
  while (this->exp >= level * 100) {
    exp -= level * 100;
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
  // 处理武器耐久（从后向前遍历）
  for (int i = equipments.size() - 1; i >= 0; --i) {
    auto &item = equipments[i];
    if (item.getType() == Weapon) {
      item.setDurability(item.getDurability() - 1);
      if (item.getDurability() <= 0) {
        cout << "【装备破损】" << item.getName() << "已破损，攻击力减少"
             << item.getValue() << "点！" << endl;
        attack -= item.getValue();
        equipments.erase(equipments.begin() + i);
      }
    }
  }
}
void player::addItem(const Item &item) {
  for (auto &existing : backpack) {
    if (existing.getName() == item.getName() &&
        existing.getLevelRequirement() == item.getLevelRequirement() &&
        existing.getType() == item.getType() &&
        existing.getValue() == item.getValue() &&
        existing.getDurability() == item.getDurability() &&
        existing.getPrice() == item.getPrice()) {
      existing.setQuantity(existing.getQuantity() + item.getQuantity());
      return;
    }
  }
  backpack.push_back(item);
}
void player::printBackpack() const {
  if (backpack.empty()) {
    cout << "背包空空如也！" << endl;
    return;
  }
  cout << "=== " << getname() << "的背包 ===" << endl;
  for (int i = 0; i < backpack.size(); i++) {
    const Item &item = backpack[i];
    cout << i + 1 << ". " << item.getName();
    cout << " x" << item.getQuantity(); // 药水堆叠数 {
    if (item.getType() == Weapon || item.getType() == Armor)
      cout << " 耐久度: " << item.getDurability(); // 装备耐久

    cout << "\n   类型: "
         << (item.getType() == Weapon  ? "武器"
             : item.getType() == Armor ? "防具"
                                       : "药水")
         << "\n   价值: " << item.getValue() << "\n   价格: " << item.getPrice()
         << "\n   等级需求: " << item.getLevelRequirement() << endl;
    cout << "-----------------------------\n";
  }
}
void player::useItem(int index) {
  if (index < 1 || index > backpack.size()) {
    cout << "无效的物品索引！" << endl;
    return;
  }
  Item &item = backpack[index - 1];
  if (item.getType() == Potion) {
    hp += item.getValue();
    if (hp > max_hp)
      hp = max_hp; // 别加爆了
    cout << getname() << "使用了" << item.getName() << "，恢复了"
         << item.getValue() << " HP!" << endl;
    item.setQuantity(item.getQuantity() - 1);
    if (item.getQuantity() <= 0) {
      backpack.erase(backpack.begin() + index - 1);
    }
  } else {
    cout << "只能使用药水类物品！" << endl;
  }
}
void player::equipItem(int index) {
  if (index < 1 || index > (int)backpack.size()) {
    cout << "无效的物品索引！" << endl;
    return;
  }
  Item itemToEquip = backpack[index - 1];
  if (itemToEquip.getLevelRequirement() > level) {
    cout << "等级不足，无法装备" << itemToEquip.getName() << "！" << endl;
    return;
  }
  if (itemToEquip.getType() == Weapon || itemToEquip.getType() == Armor) {
    // 先卸下同类型装备（从后向前遍历）
    for (int i = equipments.size() - 1; i >= 0; --i) {
      if (equipments[i].getType() == itemToEquip.getType()) {
        cout << "自动卸下 " << equipments[i].getName() << endl;
        if (equipments[i].getType() == Weapon) {
          attack -= equipments[i].getValue();
        } else {
          max_hp -= equipments[i].getValue();
          if (hp > max_hp)
            hp = max_hp;
        }
        bool merged = false;
        for (auto &existing : backpack) {
          if (existing.getName() == equipments[i].getName() &&
              existing.getLevelRequirement() ==
                  equipments[i].getLevelRequirement() &&
              existing.getType() == equipments[i].getType() &&
              existing.getValue() == equipments[i].getValue() &&
              existing.getDurability() == equipments[i].getDurability() &&
              existing.getPrice() == equipments[i].getPrice()) {
            existing.setQuantity(existing.getQuantity() + 1);
            merged = true;
            break;
          }
        }
        if (!merged) {
          backpack.push_back(equipments[i]); // 旧装备放回背包
        }
        equipments.erase(equipments.begin() + i);
        break;
      }
    }
    // 装备新物品
    cout << getname() << "装备了" << itemToEquip.getName() << "！" << endl;
    if (itemToEquip.getType() == Weapon) {
      attack += itemToEquip.getValue();
    } else {
      max_hp += itemToEquip.getValue();
      hp += itemToEquip.getValue();
    }
    itemToEquip.setQuantity(1);
    equipments.push_back(itemToEquip);

    // 从背包中移除（减少数量）
    Item &backpackItem = backpack[index - 1];
    backpackItem.setQuantity(backpackItem.getQuantity() - 1);
    if (backpackItem.getQuantity() <= 0) {
      backpack.erase(backpack.begin() + index - 1);
    }
  } else {
    cout << "只能装备武器或防具！" << endl;
  }
}
void player::unequipItem(int index) {
  if (index < 1 || index > equipments.size()) {
    cout << "无效的装备索引！" << endl;
    return;
  }
  Item &item = equipments[index - 1];
  cout << getname() << "卸下了" << item.getName() << "！" << endl;
  if (item.getType() == Weapon) {
    attack -= item.getValue();
  } else if (item.getType() == Armor) {
    max_hp -= item.getValue();
    if (hp > max_hp)
      hp = max_hp; // 卸下防具后如果当前 HP 超过最大 HP 就调整回来
  }
  for (auto &existing : backpack) {
    if (existing.getName() == item.getName() &&
        existing.getLevelRequirement() == item.getLevelRequirement() &&
        existing.getType() == item.getType() &&
        existing.getValue() == item.getValue() &&
        existing.getDurability() == item.getDurability() &&
        existing.getPrice() == item.getPrice()) {
      existing.setQuantity(existing.getQuantity() + 1);
      equipments.erase(equipments.begin() + index - 1);
      return;
    }
  }
  backpack.push_back(item); // 卸下的装备放回背包
  equipments.erase(equipments.begin() + index - 1);
  return;
}
void player::showEquippedItems() const {
  if (equipments.empty()) {
    cout << "没有装备任何物品！" << endl;
    return;
  }
  cout << "=== " << getname() << "的装备 ===" << endl;
  for (const auto &item : equipments) {
    cout << "- " << item.getName() << " (";
    if (item.getType() == Weapon) {
      cout << "武器, 攻击+" << item.getValue();
    } else if (item.getType() == Armor) {
      cout << "防具, HP+" << item.getValue();
    }
    cout << ")" << endl;
  }
}
void player::takedamage(int damage) {
  hp -= damage;
  if (hp < 0)
    hp = 0;
  // 处理防具耐久（从后向前遍历）
  for (int i = equipments.size() - 1; i >= 0; --i) {
    auto &item = equipments[i];
    if (item.getType() == Armor) {
      item.setDurability(item.getDurability() - 1);
      if (item.getDurability() <= 0) {
        cout << "【装备破损】" << item.getName() << "已破损，HP减少"
             << item.getValue() << "点！" << endl;
        max_hp -= item.getValue();
        if (hp > max_hp)
          hp = max_hp;
        equipments.erase(equipments.begin() + i);
      }
    }
  }
  cout << getname() << " takes " << damage << " damage! (HP: " << hp << "/"
       << max_hp << ")" << endl;
}
void player::showSkills(player &hero) const {
  if (skills.empty()) {
    cout << "没有任何技能！" << endl;
    return;
  }
  cout << "=== " << getname() << "的技能 ===" << endl;
  for (int i = 0; i < skills.size(); i++) {
    const skill *s = skills[i].get();
    cout << i + 1 << ". " << s->getskillname()
         << " (等级: " << s->getskilllevel() << ", MP消耗: " << s->getcostmp()
         << ", 冷却: " << s->getcooldown()
         << "s, 当前冷却: " << s->getcurrentcooldown()
         << "s, 伤害: " << s->getdamage() << ")" << endl
         << "等级需求: " << s->getlevelrequired() << endl
         << "当前技能状态: " << (s->canUse(hero) ? "可用" : "不可用") << endl
         << "===============================" << endl;
  }
}
bool player::useSkill(int index, enemy &target) {
  if (index < 1 || index > skills.size()) {
    cout << "无效的技能索引！" << endl;
    return false;
  }
  bool result = skills[index - 1]->useSkill(*this, target);
  return result;
}
void player::learnSkill(std::unique_ptr<skill> newSkill) {
  skills.push_back(std::move(newSkill));
}
skill *player::getSkill(int index) const {
  if (index < 1 || index > skills.size()) {
    return nullptr;
  }
  return skills[index - 1].get();
}
int player::getSkillCount() const { return skills.size(); }
void player::reduceallCooldowns() {
  for (auto &s : skills) {
    s->reducecooldown();
  }
}
player::~player() = default;
void player::levelUpSkill(int index) {
  if (index < 1 || index > skills.size()) {
    cout << "无效的技能索引！" << endl;
    return;
  }
  skill *s = skills[index - 1].get();
  int costcoins = s->getlevelUpCost();
  if (coin >= costcoins) {
    costcoin(costcoins);
    s->levelUpSkill(level, costcoins);
  } else {
    cout << "金币不足，无法升级技能！" << endl;
  }
}
void player::equipItem_T(const Item &item) {
  if (item.getType() == Weapon || item.getType() == Armor) {
    // 先卸下同类型装备（从后向前遍历）
    for (int i = equipments.size() - 1; i >= 0; --i) {
      if (equipments[i].getType() == item.getType()) {
        cout << "自动卸下 " << equipments[i].getName() << endl;
        if (equipments[i].getType() == Weapon) {
          attack -= equipments[i].getValue();
        } else {
          max_hp -= equipments[i].getValue();
          if (hp > max_hp)
            hp = max_hp;
        }
        bool merged = false;
        for (auto &existing : backpack) {
          if (existing.getName() == equipments[i].getName() &&
              existing.getLevelRequirement() ==
                  equipments[i].getLevelRequirement() &&
              existing.getType() == equipments[i].getType() &&
              existing.getValue() == equipments[i].getValue() &&
              existing.getDurability() == equipments[i].getDurability() &&
              existing.getPrice() == equipments[i].getPrice()) {
            existing.setQuantity(existing.getQuantity() + 1);
            merged = true;
            break;
          }
        }
        if (!merged) {
          backpack.push_back(equipments[i]); // 旧装备放回背包
        }
        equipments.erase(equipments.begin() + i);
        break;
      }
    }
    // 装备新物品
    cout << getname() << "装备了" << item.getName() << "！" << endl;
    if (item.getType() == Weapon) {
      attack += item.getValue();
    } else {
      max_hp += item.getValue();
      hp += item.getValue();
    }
    Item newItem = item;
    newItem.setQuantity(1);
    equipments.push_back(newItem);
  } else {
    cout << "只能装备武器或防具！" << endl;
  }
}
void player::attacktargetDealt(Character &target, int damage) {
  bool isCrit = (rand() % 100 < critRate);
  if (isCrit) {
    damage *= 2;
    cout << "【暴击】" << endl;
  }
  cout << getname() << " attacks " << target.getname() << " for " << damage
       << " damage!" << endl;
  target.takedamage(damage);
  // 处理武器耐久（从后向前遍历）
  for (int i = equipments.size() - 1; i >= 0; --i) {
    auto &item = equipments[i];
    if (item.getType() == Weapon) {
      item.setDurability(item.getDurability() - 1);
      if (item.getDurability() <= 0) {
        cout << "【装备破损】" << item.getName() << "已破损，攻击力减少"
             << item.getValue() << "点！" << endl;
        attack -= item.getValue();
        equipments.erase(equipments.begin() + i);
      }
    }
  }
}