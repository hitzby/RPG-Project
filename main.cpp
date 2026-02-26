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

// 怪物生成工厂
enemy generateEnemy(int floor) {
  int hp = 20 + floor * 10;
  int attack = 4 + floor * 5;
  int exp_reward = 10 + floor * 10;
  int coin_reward = 5 + floor * 2;
  int level = floor;

  // Boss 关卡增强
  if (floor % 10 == 0) {
    hp += 100;
    attack += 10;
    exp_reward += 50;
    coin_reward += 20;
    level += 5;
    return enemy("【BOSS】Enemy " + to_string(floor), hp, attack, 0, exp_reward,
                 coin_reward, level);
  }
  return enemy("Wild Enemy " + to_string(floor), hp, attack, 0, exp_reward,
               coin_reward, level);
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
  for (int i = 0; i < 5; i++) {
    shopItems.push_back(generateRandomItem(floor));
  }
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
    hero.attacktarget(monster);
    if (monster.gethp() <= 0) {
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
    monster.attacktarget(hero);
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

    // 内层循环：本层刷怪
    while (true) {
      enemy monster = generateEnemy(floor);

      cout << "\n========================================" << endl;
      cout << "Floor: " << floor << endl;
      cout << "Player: Lv." << ptr->getlevel() << " | HP: " << ptr->gethp()
           << " | Coin: " << ptr->getcoin() << endl;
      cout << "Enemy : Lv." << monster.getlevel()
           << " | HP: " << monster.gethp() << endl;
      cout << "----------------------------------------" << endl;
      cout
          << "[1] Fight [2] show backpack [3] show equipped items [4] Quit Game"
          << endl; // 简化选项，战斗后再选去哪

      int choice;
      cin >> choice;

      if (choice == 1) {
        // === 战斗 ===
        bool result = battle(*ptr, monster);

        if (result) {
          // === 赢了 ===
          cout << "\nWhat do you want to do?" << endl;
        post_battle_decision:
          cout
              << "[1] Grind (Stay here)  [2] Next Floor  [3] Quit\n"
              << "[4] Check Stats  [5] Show Backpack  [6] Show Equipped Items\n"
              << "[7] Visit Shop" << endl;

          int post_battle_choice;
          cin >> post_battle_choice;

          if (post_battle_choice == 1) {
            continue; // 继续内层循环，生成新怪
          } else if (post_battle_choice == 2) {
            if (floor != 100) {
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
          } else if (post_battle_choice == 7) {
            vector<Item> shopItems;
            showshop(*ptr, floor, shopItems);
            cout << "What do you want to do next?" << endl;
            cout << "[1] buy item [2] nothing" << endl;
            int shop_choice;
            cin >> shop_choice;
            if (shop_choice == 1) {
              bool bought = true;
              while (bought) {
                if (shopItems.empty()) {
                  cout << "The shop is out of items! Come back later." << endl;
                  cout << "Do you want to refresh the shop inventory? [1] Yes "
                          "[2] No"
                       << endl;
                  int refresh_choice;
                  cin >> refresh_choice;
                  if (refresh_choice == 1) {
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
                cout << "Do you want to buy another item? [1] Yes [2] No"
                     << endl;
                int buy_again;
                cin >> buy_again;
                if (buy_again != 1) {
                  bought = false;
                }
              }
              shopItems.clear();
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