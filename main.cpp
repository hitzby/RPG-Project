#include "enemy/enemy.h"
#include "item/item.h"
#include "player.h"
#include "player/jobs.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

using namespace std;
void showshop(player &hero, int floor, vector<Item> &shopItems);
Item generateRandomItem(int floor);
enum class eventType { NONE, TREASURE, TRAP, MERCHANT, HOT_SPRING, SHRINE };
eventType generateRandomEvent() {
  int randNum = rand() % 100;
  if (randNum < 20) {
    return eventType::TREASURE; // 20% 概率
  } else if (randNum < 40) {
    return eventType::TRAP; // 20% 概率
  } else if (randNum < 60) {
    return eventType::MERCHANT; // 20% 概率
  } else if (randNum < 80) {
    return eventType::HOT_SPRING; // 20% 概率
  } else {
    return eventType::SHRINE; // 20% 概率
  }
}
// 事件生成器
bool hadEvent(player &hero, int floor) {
  int eventChance = min(30 + floor, 80);
  int randomNum = rand() % 100;
  if (eventChance > randomNum) {
    eventType event = generateRandomEvent();
    switch (event) {
    case eventType::TREASURE: {
      cout << "你发现了一个宝箱！里面有一些金币和一个随机物品！" << endl;
      hero.gaincoin(50 + floor * 10);
      cout << "你获得了" << 50 + floor * 10 << "金币！" << endl;
      Item randomItem = generateRandomItem(floor);
      cout << "你获得了一个"
           << (randomItem.getType() == Potion
                   ? "药水"
                   : (randomItem.getType() == Weapon ? "武器" : "防具"))
           << "：" << randomItem.getName() << "！" << endl;
      cout << "=========================================" << endl;
      cout << "价值为" << randomItem.getValue() << "，耐久度为"
           << randomItem.getDurability() << "，等级需求为"
           << randomItem.getLevelRequirement() << "！" << endl;
      cout << "=========================================" << endl;
      cout << "What do you want to do with this item?" << endl;
      cout << "[1] add to backpack [2] equip it [3] discard" << endl;
      int choice;
      cin >> choice;
      if (choice == 1) {
        hero.addItem(randomItem);
        cout << "物品已添加到背包！" << endl;
      } else if (choice == 2) {
        hero.equipItem_T(randomItem);
        cout << "物品已装备！" << endl;
      } else if (choice == 3) {
        cout << "物品被丢弃了。" << endl;
      }
    } break;
    case eventType::TRAP: {
      cout << "你触发了一个陷阱！受到了一些伤害！" << endl;
      int trapDamage = 10 + floor * 5;
      hero.takedamage(trapDamage);
    } break;
    case eventType::MERCHANT: {
      cout << "你遇到了一个商人！他有一些物品出售！" << endl;
      vector<Item> shopItems;
      for (int i = 0; i < 5; i++) {
        shopItems.push_back(generateRandomItem(floor));
      }
      showshop(hero, floor, shopItems);
      cout << "What do you want to do?" << endl;
      cout << "[1] buy item [2] leave" << endl;
      int shop_choice;
      cin >> shop_choice;
      if (shop_choice == 1) {
        bool bought = true;
        while (bought) {
          if (shopItems.empty()) {
            cout << "The shop is out of items! Come back later." << endl;
            break;
          }
          cout << "Enter the index of the item you want to buy: " << endl;
          int buy_index;
          cin >> buy_index;
          if (buy_index >= 1 && buy_index <= 5) {
            Item item = shopItems[buy_index - 1]; // 获取商店中的物品
            if (hero.getcoin() >= item.getPrice()) {
              hero.costcoin(item.getPrice());
              cout << "You bought " << item.getName() << " for "
                   << item.getPrice() << " coins!" << endl;
              cout << "What do you want to do with this item?" << endl;
              cout << "[1] add to backpack [2] equip it [3] discard" << endl;
              int choice;
              cin >> choice;
              if (choice == 1) {
                hero.addItem(item);
                cout << "物品已添加到背包！" << endl;
              } else if (choice == 2) {
                hero.equipItem_T(item);
                cout << "物品已装备！" << endl;
              } else if (choice == 3) {
                cout << "物品被丢弃了。" << endl;
              }
              shopItems.erase(shopItems.begin() + buy_index -
                              1); // 从商店中移除

            } else {
              cout << "You don't have enough coins to buy this item!" << endl;
            }
          } else {
            cout << "Invalid item index!" << endl;
          }
          showshop(hero, floor, shopItems);
          cout << "Do you want to buy another item? [1] Yes [2] No" << endl;
          int buy_again;
          cin >> buy_again;
          if (buy_again != 1) {
            bought = false;
          }
        }
      } else {
        cout << "You decided to leave the merchant." << endl;
      }
    } break;
    case eventType::HOT_SPRING: {
      cout << "你发现了一个温泉！休息一下，恢复一些HP和MP！" << endl;
      int hpRecovery = 20 + floor * 10;
      int mpRecovery = 10 + floor * 5;
      hero.sethp(hero.gethp() + hpRecovery);
      hero.setmp(hero.getmp() + mpRecovery);
      cout << "你恢复了" << hpRecovery << "点HP和" << mpRecovery << "点MP！"
           << endl;
    } break;
    case eventType::SHRINE: {
      cout << "你发现了一个神社！许个愿吧！" << endl;
      int wishOutcome = rand() % 3;
      if (wishOutcome == 0) {
        cout << "你的愿望实现了！获得了一些金币和经验！" << endl;
        hero.gaincoin(100 + floor * 20);
        hero.gainexp(50 + floor * 10);
        cout << "你获得了" << 100 + floor * 20 << "金币和" << 50 + floor * 10
             << "经验！" << endl;
      } else if (wishOutcome == 1) {
        cout << "你的愿望被神明拒绝了！失去了一些HP和MP！" << endl;
        int hpLoss = 15 + floor * 5;
        int mpLoss = 10 + floor * 5;
        hero.takedamage(hpLoss);
        hero.setmp(max(0, hero.getmp() - mpLoss));
        cout << "你失去了" << hpLoss << "点HP和" << mpLoss << "点MP！" << endl;
      } else {
        cout << "你的愿望神明不屑于听(?)" << endl;
      }
    } break;
    case eventType::NONE: {
      cout << "没有发生任何事件，继续前进..." << endl;
    } break;
    }
  }
  if (hero.gethp() <= 0) {
    cout << "你在事件中不幸死亡了..." << endl;
    return true; // 事件导致死亡，返回 true
  }
  return false; // 事件发生但未死亡，返回 false
}
// 怪物生成工厂
enemy generateEnemy(int floor) {
  int hp = 20 + floor * 10;
  int attack = 4 + floor * 5;
  int exp_reward = 10 + floor * 100;
  int coin_reward = 5 + floor * 20;
  int level = floor;
  string name = "Monster of Floor " + to_string(floor);
  // Boss 关卡增强
  if (floor % 10 == 0) {
    hp += 100;
    attack += 10;
    exp_reward += 50;
    coin_reward += 20;
    level += 5;
    name = "Boss of Floor " + to_string(floor);
  }
  enemy monster(name, hp, attack, 0, exp_reward, coin_reward, level);
  monster.setAffix(floor);
  if (monster.hasAffix(Affix::STRONG)) {
    monster.resetattack(monster.getattack() + monster.getlevel() * 2);
  }
  if (monster.hasAffix(Affix::TOUGH)) {
    monster.resethp(monster.gethp() + monster.getlevel() * 20);
  }
  if (monster.hasAffix(Affix::QUICK)) {
    monster.setdogeRate(
        min(monster.getdogeRate() + monster.getlevel() * 2, 50));
  }
  return monster;
}
// 随机掉落
Item generateRandomItem(int floor) {
  int itemType = rand() % 3; // 0: Potion, 1: Weapon, 2: Armor
  int value = (floor / 10 + 1) * (rand() % 5 + 1); // 随机数乘以层数加成
  string name;
  switch (itemType) {
  case 0: // 药水
    name = "Potion of Floor " + to_string(floor);
    return Item(name, floor, Potion, value, 0, floor * 10, 1);
  case 1: // 武器
    name = "Weapon of Floor " + to_string(floor);
    return Item(name, floor, Weapon, value, floor * 2, floor * 10, 1);
  case 2: // 防具
    name = "Armor of Floor " + to_string(floor);
    return Item(name, floor, Armor, value, floor * 2, floor * 10, 1);
  }
  return Item("Normal Potion", 1, Potion, 1, 0, 0, 1);
}
// 引入商店
void showshop(player &hero, int floor, vector<Item> &shopItems) {
  cout << "\n========================================" << endl;
  cout << "欢迎来到商店！当前楼层为：" << floor << endl;
  for (int i = 0; i < shopItems.size(); i++) {
    cout << i + 1 << "." << shopItems[i].getName() << " - 类型: "
         << (shopItems[i].getType() == Weapon
                 ? "武器"
                 : (shopItems[i].getType() == Armor ? "防具" : "药水"))
         << endl
         << " - 价值: " << shopItems[i].getValue()
         << " - 耐久度: " << shopItems[i].getDurability() << endl
         << " - 价格: " << shopItems[i].getPrice() << "- 数量："
         << shopItems[i].getQuantity() << endl
         << " - 等级需求: " << shopItems[i].getLevelRequirement() << endl
         << "你的金币为：" << hero.getcoin() << endl
         << "=========================================" << endl;
  }
}
// 战斗逻辑
bool battle(player &hero, enemy &monster) {
  cout << "\n>>> A " << monster.getname() << " appears! <<<" << endl;

  while (hero.gethp() > 0 && monster.gethp() > 0) {
    // 1. 玩家攻击
    hero.setmp(hero.getmp() + 5);
    hero.reduceallCooldowns(); // 每回合开始时减少技能冷却时间
    cout << ">>>" << "你的回合！What do you want to do!" << endl;
    if (monster.hasAffix(Affix::POSIONOUS)) {
      cout << ">>> " << monster.getname() << "【剧毒】 <<<" << endl;
      int poisonDamage = 5 + monster.getlevel() * 2;
      hero.takedamage(poisonDamage);
      cout << "You take " << poisonDamage
           << " poison damage! (HP: " << hero.gethp() << "/" << hero.getmaxhp()
           << ")" << endl;
      if (hero.gethp() <= 0) {
        cout << ">>> " << hero.getname() << " is defeated by the poison!"
             << endl;
        cout << ">>> YOU LOSE..." << endl;
        return false; // 输了立刻返回
      }
    }
  post_skill_decision:
    cout << "[1] Attack [2] Use Skill" << endl;
    int action_choice;
    cin >> action_choice;
    if (action_choice == 1) {
      if (monster.hasAffix(Affix::SHIELDING)) {
        cout << ">>> " << monster.getname() << "【护盾】 <<<" << endl;
        int shieldAmount = 10 + monster.getlevel() * 5;
        cout << monster.getname() << " absorbs " << shieldAmount
             << " damage with its shield!" << endl;
        int damageDealt = max(0, hero.getattack() - shieldAmount);
        if (damageDealt > 0) {
          cout << "Your attack breaks through the shield and deals "
               << damageDealt << " damage!" << endl;
          hero.attacktargetDealt(monster, damageDealt);
        } else {
          cout << "Your attack couldn't penetrate the shield!" << endl;
        }
      } else {
        hero.attacktarget(monster);
      }
    } else if (action_choice == 2) {
      hero.showSkills(hero);

      cout << "Choose a skill to use (1-" << hero.getSkillCount()
           << "): " << endl;
      int skill_choice;
      cin >> skill_choice;
      bool skill_used = hero.useSkill(skill_choice, monster);
      if (!skill_used) {
        cout << "技能使用失败！" << endl;
        goto post_skill_decision;
      }
    } else {
      cout << "Invalid choice, defaulting to normal attack." << endl;
      if (monster.hasAffix(Affix::SHIELDING)) {
        cout << ">>> " << monster.getname() << "【护盾】 <<<" << endl;
        int shieldAmount = 10 + monster.getlevel() * 5;
        cout << monster.getname() << " absorbs " << shieldAmount
             << " damage with its shield!" << endl;
        int damageDealt = max(0, hero.getattack() - shieldAmount);
        if (damageDealt > 0) {
          cout << "Your attack breaks through the shield and deals "
               << damageDealt << " damage!" << endl;
          hero.attacktargetDealt(monster, damageDealt);
        } else {
          cout << "Your attack couldn't penetrate the shield!" << endl;
        }
      } else {
        hero.attacktarget(monster);
      }
    }
    if (monster.gethp() <= 0) {
      if (monster.hasAffix(Affix::EXPLOSIVE)) {
        cout << ">>> " << monster.getname() << " 【爆炸】 <<<" << endl;
        int explosionDamage = 20 + monster.getlevel() * 5;
        hero.takedamage(explosionDamage);
        cout << "You take " << explosionDamage
             << " damage from the explosion! (HP: " << hero.gethp() << "/"
             << hero.getmaxhp() << ")" << endl;
        if (hero.gethp() <= 0) {
          cout << ">>> " << hero.getname() << " is defeated by the explosion!"
               << endl;
          cout << ">>> YOU LOSE..." << endl;
          return false; // 输了立刻返回
        }
      }
      cout << ">>> " << monster.getname() << " is defeated!" << endl;
      hero.gainexp(monster.getexp_reward());
      hero.gaincoin(monster.getcoin_reward());
      hero.heal(hero.getmaxhp() / 5); // 战后回血，恢复最大HP的20%
      cout << ">>> YOU WIN!" << endl;
      // 掉落物品
      if (rand() % 100 < 50) {
        Item droppedItem = generateRandomItem(monster.getlevel());
        cout << "掉落了"
             << (droppedItem.getType() == Potion
                     ? "药水"
                     : (droppedItem.getType() == Weapon ? "武器" : "防具"))
             << ":" << droppedItem.getName() << endl;
        hero.addItem(droppedItem);
      }
      return true;
    }

    // 2. 怪物反击
    cout << ">>> " << monster.getname() << "的回合！<<<" << endl;
    monster.attacktarget(hero);
    if (monster.hasAffix(Affix::HEALING)) {
      cout << ">>> " << monster.getname() << "【治疗】 <<<" << endl;
      int healAmount = 10 + monster.getlevel() * 5;
      monster.heal(healAmount);
      cout << monster.getname() << " heals for " << healAmount
           << " HP! (HP: " << monster.gethp() << "/" << monster.getmaxhp()
           << ")" << endl;
    }
    if (hero.gethp() <= 0) {
      cout << ">>> " << hero.getname() << " is defeated!" << endl;
      cout << ">>> YOU LOSE..." << endl;
      return false; // 输了立刻返回
    }
  }
  return false;
}

int main() {
  srand(time(0)); // 别忘了随机数种子！
  cout << "=== Link Start! Welcome to Aincrad! ===" << endl;
  cout << "if you do not have coins and you die before floor 10, you can still "
       << endl
       << "revive at floor 1!,but you only have 3 free revives,after that you "
       << endl
       << "will have to pay coins to revive,all in all please be careful with "
       << endl
       << "your coins,your revive time and the floor you are on!" << endl;
  cout << "if you have coins and you die,you will lose half of your coins and "
       << endl
       << "fall back one floor" << endl;
  // 初始化：等级设为 1，避免除零或死循环风险
  cout << "please enter your name: " << endl;
  string player_name;
  cin >> player_name;
  cout << "choose your job: [1] Warrior  [2] Mage  [3] Assassin" << endl;
  int job_choice;
  cin >> job_choice;
  unique_ptr<player> ptr;
  switch (job_choice) {
  case 1:
    ptr = make_unique<Warrior>(player_name);
    break;
  case 2:
    ptr = make_unique<Mage>(player_name);
    break;
  case 3:
    ptr = make_unique<Assassin>(player_name);
    break;
  default:
    cout << "Invalid choice, defaulting to Warrior." << endl;
    ptr = make_unique<Warrior>(player_name);
    break;
  }

  int floor = 1;

  // 修复：只要 <= 100 就继续，去掉 floor > 1 的限制
  while (floor <= 100) {
    vector<Item> shopItems;
    for (int i = 0; i < 5; i++) {
      shopItems.push_back(generateRandomItem(floor));
    }

    // 内层循环：本层刷怪
    while (true) {
      enemy monster = generateEnemy(floor);
      monster.displayAffixes(); // 显示词缀

      cout << "\n========================================" << endl;
      cout << "Floor: " << floor << endl;
      cout << "Player: Lv." << ptr->getlevel() << " | HP: " << ptr->gethp()
           << " | Coin: " << ptr->getcoin() << "| mp: " << ptr->getmp() << endl;
      cout << "Enemy : Lv." << monster.getlevel()
           << " | HP: " << monster.gethp() << endl;
      cout << "----------------------------------------" << endl;
      cout
          << "[1] Fight [2] show backpack [3] show equipped items [4] Quit Game"
          << endl; // 简化选项，战斗后再选去哪

      int choice;
      cin >> choice;

      if (choice == 1) {
        bool eventResult = hadEvent(*ptr, floor);
        if (eventResult) {
          // 事件导致死亡，处理复活逻辑
          if (ptr->getfreerevives() > 0 && floor < 10) {
            cout << "你有" << ptr->getfreerevives()
                 << "次免费复活机会，是否使用？[1] Yes [2] No" << endl;
            int revive_choice;
            cin >> revive_choice;
            if (revive_choice == 1) {
              ptr->decreaserevives();
              ptr->revive();
              floor = 1; // 继续当前层
              break;
            } else {
              cout << "你选择了不复活，游戏结束。" << endl;
              return 0;
            }
          } else if (ptr->getcoin() >= 20) {
            cout << "你有足够的金币支付20金币来复活。是否支付？[1] Yes [2] No"
                 << endl;
            int pay_revive_choice;
            cin >> pay_revive_choice;
            if (pay_revive_choice == 1) {
              ptr->costcoin(20);
              ptr->revive();
              if (floor > 1)
                floor--; // 继续当前层
              break;
            } else {
              cout << "你选择了不复活，游戏结束。" << endl;
              return 0;
            }
          } else {
            cout << "没有足够的金币支付复活费用，游戏结束。" << endl;
            return 0;
          }
        }
        // === 战斗 ===
        bool result = battle(*ptr, monster);

        if (result) {
          // === 赢了 ===
          cout << "\nWhat do you want to do?" << endl;
        post_battle_decision:
          cout
              << "[1] Grind (Stay here)  [2] Next Floor  [3] Quit\n"
              << "[4] Check Stats  [5] Show Backpack  [6] Show Equipped Items\n"
              << "[7] Visit Shop [8] show skills" << endl;

          int post_battle_choice;
          cin >> post_battle_choice;

          if (post_battle_choice == 1) {
            continue; // 继续内层循环，生成新怪
          } else if (post_battle_choice == 2) {
            if (floor != 100) {
              shopItems.clear();
              cout << "Heading to next floor..." << endl;
            } else {
              cout << "congratulations!" << endl;
            }
            floor++;
            break; // 跳出内层循环，进入下一层
          } else if (post_battle_choice == 4) {
            ptr->printstats();
            goto post_battle_decision; // 打印完状态后重新选择
          } else if (post_battle_choice == 5) {
            ptr->printBackpack();
            cout << "What do you what to do next?" << endl;
            cout << "[1] use item [2] equip item [3] nothing" << endl;
            int backpack_choice;
            cin >> backpack_choice;
            if (backpack_choice == 1) {
              cout << "Enter the index of the item you want to use: " << endl;
              int item_index;
              cin >> item_index;
              ptr->useItem(item_index);
            } else if (backpack_choice == 2) {
              cout << "Enter the index of the item you want to equip: " << endl;
              int equip_index;
              cin >> equip_index;
              ptr->equipItem(equip_index);
            } else {
              cout << "Back to previous menu." << endl;
            }
            goto post_battle_decision; // 打印完背包后重新选择
          } else if (post_battle_choice == 6) {
            ptr->showEquippedItems();
            cout << "What do you what to do next?" << endl;
            cout << "[1] unequip item [2] nothing" << endl;
            int equip_choice;
            cin >> equip_choice;
            if (equip_choice == 1) {
              cout << "Enter the index of the item you want to unequip: "
                   << endl;
              int unequip_index;
              cin >> unequip_index;
              ptr->unequipItem(unequip_index);
            } else {
              cout << "Back to previous menu." << endl;
            }
            goto post_battle_decision; // 打印完装备后重新选择
          } else if (post_battle_choice == 3) {
            cout << "See you next time!" << endl;
            return 0;
          } else if (post_battle_choice == 8) {
          post_skill_decision:
            ptr->showSkills(*ptr);
            cout << "What do you want to do next?" << endl;
            cout << "[1] upgrade skill [2] nothing" << endl;
            int skill_up_choice;
            cin >> skill_up_choice;
            if (skill_up_choice == 1) {
              cout << "Enter the index of the skill you want to upgrade: "
                   << endl;
              int skill_index;
              cin >> skill_index;
              ptr->levelUpSkill(skill_index);
              goto post_skill_decision;
            } else {
              cout << "Back to previous menu." << endl;
              goto post_battle_decision;
            }
          } else if (post_battle_choice == 7) {

            showshop(*ptr, floor, shopItems);
          post_shop_decision:
            cout << "What do you want to do next?" << endl;
            cout << "[1] buy item [2] nothing [3]refresh shop" << endl;
            int shop_choice;
            cin >> shop_choice;
            if (shop_choice == 1) {
              bool bought = true;
              while (bought) {
                if (shopItems.empty()) {
                  cout << "The shop is out of items! Come back later." << endl;
                  cout << "Do you want to pay 20 coins to refresh the shop "
                          "inventory? [1] Yes "
                          "[2] No"
                       << endl;
                  int refresh_choice;
                  cin >> refresh_choice;
                  if (refresh_choice == 1) {
                    if (ptr->getcoin() >= 20) {
                      ptr->costcoin(20);
                      cout << "Shop inventory refreshed!" << endl;
                    } else {
                      cout << "You don't have enough coins to refresh the shop!"
                           << endl;
                      break;
                    }
                    for (int i = 0; i < 5; i++) {
                      shopItems.push_back(generateRandomItem(floor));
                    }
                    showshop(*ptr, floor, shopItems);
                  } else {
                    cout << "Back to previous menu." << endl;
                    break;
                  }
                }
                cout << "Enter the index of the item you want to buy: " << endl;
                int buy_index;
                cin >> buy_index;
                if (buy_index >= 1 && buy_index <= 5) {
                  Item item = shopItems[buy_index - 1]; // 获取商店中的物品
                  if (ptr->getcoin() >= item.getPrice()) {
                    ptr->costcoin(item.getPrice());
                    ptr->addItem(item);
                    shopItems.erase(shopItems.begin() + buy_index -
                                    1); // 从商店中移除
                    cout << "You bought " << item.getName() << " for "
                         << item.getPrice() << " coins!" << endl;
                  } else {
                    cout << "You don't have enough coins to buy this item!"
                         << endl;
                  }
                } else {
                  cout << "Invalid item index!" << endl;
                }
                showshop(*ptr, floor, shopItems);
                cout << "Do you want to buy another item? [1] Yes [2] No"
                     << endl;
                int buy_again;
                cin >> buy_again;
                if (buy_again != 1) {
                  bought = false;
                }
              }
            } else if (shop_choice == 3) {
              if (ptr->getcoin() >= 20) {
                ptr->costcoin(20);
                cout << "Shop inventory refreshed!" << endl;
              } else {
                cout << "You don't have enough coins to refresh the shop!"
                     << endl;
                goto post_shop_decision;
              }
              shopItems.clear();
              for (int i = 0; i < 5; i++) {
                shopItems.push_back(generateRandomItem(floor));
              }
              showshop(*ptr, floor, shopItems);
              goto post_shop_decision; // 刷新商店后重新选择
            } else {
              cout << "Back to previous menu." << endl;
            }
            goto post_battle_decision;
          }
        } else {
          // === 输了 ===
          if (ptr->getcoin() == 0) {
            if (floor <= 10 && ptr->getfreerevives() != 0) {
              cout << "Although you don't have any coins, you can still "
                      "revive "
                      "at Floor 1!"
                   << endl;
              floor = 1;
              ptr->revive();
              ptr->decreaserevives();
              cout << "Revived! You have " << ptr->getfreerevives()
                   << " free revives left." << endl;
              break; // 跳出内层循环，重新加载楼层;
            }
            cout << "You have no coins to lose,or you have no free revives "
                    "left!"
                 << endl;
            cout << "GAME OVER" << endl;
            return 0;
          }
          int coin_loss = max(1, ptr->getcoin() / 2);
          cout << "\nDo you want to revive? Cost: " << coin_loss << " coins."
               << endl;
          cout << "[1] Revive & Back 1 Floor  [2] Die & Quit" << endl;

          int revive_choice;
          cin >> revive_choice;

          if (revive_choice == 1) {
            // 记得在 player 类里实现 revive() 哦！
            ptr->revive();
            // 如果还没实现，可以用 hero.gainexp(0); 这种临时方法或者 setHp

            ptr->costcoin(coin_loss);
            cout << "Revived! Lost " << coin_loss << " coins." << endl;

            // 只有在大于1层时才回退，否则在第1层原地复活
            if (floor > 1) {
              floor--;
              cout << "Fell back to Floor " << floor << "." << endl;
            } else {
              cout << "Revived at Floor 1." << endl;
            }
            break; // 跳出内层循环，重新加载楼层
          } else {
            cout << "GAME OVER" << endl;
            return 0;
          }
        }
      } else if (choice == 2) {
        ptr->printBackpack();
      } else if (choice == 3) {
        ptr->showEquippedItems();
      } else if (choice == 4) {
        cout << "Logged out." << endl;
        return 0;
      }
    }
  }

  return 0;
}