#include "enemy.h"
#include "item.h"
#include "jobs.h"
#include "player.h"
#include "save.h" // ★ 存档系统
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// ─────────────────────────────────────────────
//  前置声明
// ─────────────────────────────────────────────
void showshop(player &hero, int floor, vector<Item> &shopItems);
Item generateRandomItem(int floor);
enemy generateEnemy(int floor);
bool battle(player &hero, enemy &monster);
bool hadEvent(player &hero, int floor);

// ─────────────────────────────────────────────
//  输入辅助：清除 cin 错误状态
// ─────────────────────────────────────────────
static int safeReadInt() {
  int v;
  while (!(cin >> v)) {
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "请输入一个数字：";
  }
  return v;
}

// ─────────────────────────────────────────────
//  随机事件枚举
// ─────────────────────────────────────────────
enum class eventType { NONE, TREASURE, TRAP, MERCHANT, HOT_SPRING, SHRINE };

eventType generateRandomEvent() {
  int r = rand() % 100;
  if (r < 20)
    return eventType::TREASURE;
  else if (r < 40)
    return eventType::TRAP;
  else if (r < 60)
    return eventType::MERCHANT;
  else if (r < 80)
    return eventType::HOT_SPRING;
  else
    return eventType::SHRINE;
}

// ─────────────────────────────────────────────
//  随机物品生成
// ─────────────────────────────────────────────
Item generateRandomItem(int floor) {
  int itemType = rand() % 3;
  int value = (floor / 10 + 1) * (rand() % 5 + 1);
  string name;
  switch (itemType) {
  case 0:
    name = "Potion of Floor " + to_string(floor);
    return Item(name, floor, Potion, value, 0, floor * 10, 1);
  case 1:
    name = "Weapon of Floor " + to_string(floor);
    return Item(name, floor, Weapon, value, floor * 2, floor * 10, 1);
  default:
    name = "Armor of Floor " + to_string(floor);
    return Item(name, floor, Armor, value, floor * 2, floor * 10, 1);
  }
}

// ─────────────────────────────────────────────
//  怪物生成
// ─────────────────────────────────────────────
enemy generateEnemy(int floor) {
  int hp = 20 + floor * 10;
  int attack = 4 + floor * 5;
  int exp_reward = 10 + floor * 100;
  int coin_reward = 5 + floor * 20;
  int level = floor;
  string name = "Monster of Floor " + to_string(floor);

  // Boss 关卡强化
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

  if (monster.hasAffix(Affix::STRONG))
    monster.resetattack(monster.getattack() + monster.getlevel() * 2);
  if (monster.hasAffix(Affix::TOUGH))
    monster.resethp(monster.gethp() + monster.getlevel() * 20);
  if (monster.hasAffix(Affix::QUICK))
    monster.setdogeRate(
        min(monster.getdogeRate() + monster.getlevel() * 2, 50));

  return monster;
}

// ─────────────────────────────────────────────
//  商店
// ─────────────────────────────────────────────
void showshop(player &hero, int floor, vector<Item> &shopItems) {
  cout << "\n========================================" << endl;
  cout << "欢迎来到商店！当前楼层：" << floor << endl;
  for (int i = 0; i < (int)shopItems.size(); i++) {
    cout << i + 1 << ". " << shopItems[i].getName() << " - 类型: "
         << (shopItems[i].getType() == Weapon  ? "武器"
             : shopItems[i].getType() == Armor ? "防具"
                                               : "药水")
         << "\n   价值: " << shopItems[i].getValue()
         << " 耐久: " << shopItems[i].getDurability()
         << "\n   价格: " << shopItems[i].getPrice()
         << " 数量: " << shopItems[i].getQuantity()
         << "\n   等级需求: " << shopItems[i].getLevelRequirement()
         << "\n   你的金币：" << hero.getcoin()
         << "\n=========================================" << endl;
  }
}

// ─────────────────────────────────────────────
//  随机事件触发
// ─────────────────────────────────────────────
bool hadEvent(player &hero, int floor) {
  int eventChance = min(30 + floor, 80);
  if ((rand() % 100) >= eventChance)
    return false;

  eventType event = generateRandomEvent();
  switch (event) {

  case eventType::TREASURE: {
    cout << "你发现了一个宝箱！里面有一些金币和一个随机物品！" << endl;
    int coins = 50 + floor * 10;
    hero.gaincoin(coins);
    cout << "你获得了 " << coins << " 金币！" << endl;
    Item randomItem = generateRandomItem(floor);
    cout << "你获得了一个"
         << (randomItem.getType() == Potion   ? "药水"
             : randomItem.getType() == Weapon ? "武器"
                                              : "防具")
         << "：" << randomItem.getName() << "！" << endl;
    cout << "价值 " << randomItem.getValue() << "，耐久 "
         << randomItem.getDurability() << "，等级需求 "
         << randomItem.getLevelRequirement() << "！" << endl;
    cout << "[1] 加入背包  [2] 直接装备  [3] 丢弃" << endl;
    int c;
    c = safeReadInt();
    if (c == 1) {
      hero.addItem(randomItem);
      cout << "物品已加入背包！" << endl;
    } else if (c == 2) {
      hero.equipItem_T(randomItem);
      cout << "物品已装备！" << endl;
    } else
      cout << "物品被丢弃了。" << endl;
  } break;

  case eventType::TRAP: {
    cout << "你触发了一个陷阱！受到了一些伤害！" << endl;
    int trapDamage = 10 + floor * 5;
    hero.takedamage(trapDamage);
  } break;

  case eventType::MERCHANT: {
    cout << "你遇到了一个商人！他有一些物品出售！" << endl;
    vector<Item> shopItems;
    for (int i = 0; i < 5; i++)
      shopItems.push_back(generateRandomItem(floor));
    showshop(hero, floor, shopItems);
    cout << "[1] 购买物品  [2] 离开" << endl;
    int sc;
    sc = safeReadInt();
    if (sc == 1) {
      bool buying = true;
      while (buying) {
        if (shopItems.empty()) {
          cout << "商店已售罄！" << endl;
          break;
        }
        cout << "输入物品序号购买：";
        int bi;
        bi = safeReadInt();
        if (bi >= 1 && bi <= (int)shopItems.size()) {
          Item &item = shopItems[bi - 1];
          if (hero.getcoin() >= item.getPrice()) {
            hero.costcoin(item.getPrice());
            cout << "购买了 " << item.getName() << "！" << endl;
            cout << "[1] 背包  [2] 装备  [3] 丢弃" << endl;
            int cc;
            cc = safeReadInt();
            if (cc == 1)
              hero.addItem(item);
            else if (cc == 2)
              hero.equipItem_T(item);
            shopItems.erase(shopItems.begin() + bi - 1);
          } else
            cout << "金币不足！" << endl;
        } else
          cout << "无效序号！" << endl;
        showshop(hero, floor, shopItems);
        cout << "继续购买？[1] 是  [2] 否" << endl;
        int again;
        again = safeReadInt();
        if (again != 1)
          buying = false;
      }
    }
  } break;

  case eventType::HOT_SPRING: {
    cout << "你发现了一个温泉！休息一下，恢复一些 HP 和 MP！" << endl;
    int hpR = 20 + floor * 10, mpR = 10 + floor * 5;
    hero.sethp(hero.gethp() + hpR);
    hero.setmp(hero.getmp() + mpR);
    cout << "你恢复了 " << hpR << " 点 HP 和 " << mpR << " 点 MP！" << endl;
  } break;

  case eventType::SHRINE: {
    cout << "你发现了一个神社！许个愿吧！" << endl;
    int outcome = rand() % 3;
    if (outcome == 0) {
      cout << "神明赐予了你金币和经验！" << endl;
      int gc = 100 + floor * 20, ec = 50 + floor * 10;
      hero.gaincoin(gc);
      hero.gainexp(ec);
      cout << "获得 " << gc << " 金币，" << ec << " 经验！" << endl;
    } else if (outcome == 1) {
      cout << "神明拒绝了你！失去了一些 HP 和 MP！" << endl;
      int hl = 15 + floor * 5, ml = 10 + floor * 5;
      hero.takedamage(hl);
      hero.setmp(max(0, hero.getmp() - ml));
      cout << "失去了 " << hl << " 点 HP 和 " << ml << " 点 MP！" << endl;
    } else {
      cout << "神明沉默不语..." << endl;
    }
  } break;

  default:
    break;
  }

  if (hero.gethp() <= 0) {
    cout << "你在事件中不幸死亡了..." << endl;
    return true;
  }
  return false;
}

// ─────────────────────────────────────────────
//  战斗逻辑
// ─────────────────────────────────────────────
bool battle(player &hero, enemy &monster) {
  cout << "\n>>> " << monster.getname() << " 出现了！<<<" << endl;

  while (hero.gethp() > 0 && monster.gethp() > 0) {
    hero.setmp(hero.getmp() + 5);
    hero.reduceallCooldowns();

    cout << ">>> 你的回合！What do you want to do!" << endl;

    // 剧毒伤害
    if (monster.hasAffix(Affix::POSIONOUS)) {
      cout << ">>> " << monster.getname() << "【剧毒】<<<" << endl;
      int pDmg = 5 + monster.getlevel() * 2;
      hero.takedamage(pDmg);
      cout << "你受到了 " << pDmg << " 点毒伤！(HP: " << hero.gethp() << "/"
           << hero.getmaxhp() << ")" << endl;
      if (hero.gethp() <= 0) {
        cout << ">>> " << hero.getname() << " 被毒死了！" << endl;
        cout << ">>> YOU LOSE..." << endl;
        return false;
      }
    }

  post_skill_decision:
    cout << "[1] 普通攻击  [2] 使用技能" << endl;
    int ac;
    ac = safeReadInt();

    auto doAttack = [&]() {
      if (monster.hasAffix(Affix::SHIELDING)) {
        cout << ">>> " << monster.getname() << "【护盾】<<<" << endl;
        int shield = 10 + monster.getlevel() * 5;
        int dealt = max(0, hero.getattack() - shield);
        if (dealt > 0) {
          cout << "你突破了护盾，造成 " << dealt << " 点伤害！" << endl;
          hero.attacktargetDealt(monster, dealt);
        } else {
          cout << "攻击无法穿透护盾！" << endl;
        }
      } else {
        hero.attacktarget(monster);
      }
    };

    if (ac == 1) {
      doAttack();
    } else if (ac == 2) {
      hero.showSkills(hero);
      cout << "选择技能 (1-" << hero.getSkillCount() << "): ";
      int sc;
      sc = safeReadInt();
      if (!hero.useSkill(sc, monster)) {
        cout << "技能使用失败！" << endl;
        goto post_skill_decision;
      }
    } else {
      cout << "无效选择，默认普通攻击。" << endl;
      doAttack();
    }

    // 判断怪物是否死亡
    if (monster.gethp() <= 0) {
      if (monster.hasAffix(Affix::EXPLOSIVE)) {
        cout << ">>> " << monster.getname() << "【爆炸】<<<" << endl;
        int expDmg = 20 + monster.getlevel() * 5;
        hero.takedamage(expDmg);
        cout << "你受到了 " << expDmg << " 点爆炸伤害！(HP: " << hero.gethp()
             << "/" << hero.getmaxhp() << ")" << endl;
        if (hero.gethp() <= 0) {
          cout << ">>> " << hero.getname() << " 被爆炸击败！" << endl;
          cout << ">>> YOU LOSE..." << endl;
          return false;
        }
      }
      cout << ">>> " << monster.getname() << " 被击败了！" << endl;
      hero.gainexp(monster.getexp_reward());
      hero.gaincoin(monster.getcoin_reward());
      hero.heal(hero.getmaxhp() / 5);
      cout << ">>> YOU WIN!" << endl;
      // 掉落
      if (rand() % 100 < 50) {
        Item dropped = generateRandomItem(monster.getlevel());
        cout << "掉落了"
             << (dropped.getType() == Potion   ? "药水"
                 : dropped.getType() == Weapon ? "武器"
                                               : "防具")
             << "：" << dropped.getName() << endl;
        hero.addItem(dropped);
      }
      return true;
    }

    // 怪物回合
    cout << ">>> " << monster.getname() << " 的回合！<<<" << endl;
    monster.attacktarget(hero);
    if (monster.hasAffix(Affix::HEALING)) {
      cout << ">>> " << monster.getname() << "【治疗】<<<" << endl;
      int healAmt = 10 + monster.getlevel() * 5;
      monster.heal(healAmt);
      cout << monster.getname() << " 恢复了 " << healAmt
           << " 点 HP！(HP: " << monster.gethp() << "/" << monster.getmaxhp()
           << ")" << endl;
    }
    if (hero.gethp() <= 0) {
      cout << ">>> " << hero.getname() << " 被击败了！" << endl;
      cout << ">>> YOU LOSE..." << endl;
      return false;
    }
  }
  return false;
}

// ─────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────
int main() {
  srand((unsigned)time(nullptr));

  cout << "=== Link Start! 欢迎来到 Aincrad! ===" << endl;
  cout << "规则提醒：\n"
       << "  - 10楼前死亡且有免费复活次数→免费复活回第1层\n"
       << "  - 有金币死亡→损失一半金币，退后一层\n"
       << "  - 请珍惜生命、金币与免费复活机会！\n"
       << endl;

  // ────────────────────────────
  //  ★ 存档检测：询问是否读档
  // ────────────────────────────
  unique_ptr<player> ptr;
  int floor = 1;

  if (SaveSystem::hasSaveFile()) {
    cout << "检测到存档文件！\n"
         << "[1] 读取存档继续游戏\n"
         << "[2] 开始新游戏（将覆盖存档）\n"
         << "请选择：";
    int loadChoice;
    loadChoice = safeReadInt();
    if (loadChoice == 1) {
      ptr = SaveSystem::loadGame(floor);
      if (!ptr) {
        cout << "读档失败，将开始新游戏。" << endl;
      }
    } else {
      SaveSystem::deleteSave();
    }
  }

  // ────────────────────────────
  //  未读档 → 新建角色
  // ────────────────────────────
  if (!ptr) {
    cout << "请输入你的名字：";
    string player_name;
    cin >> player_name;
    cout << "选择职业：[1] 战士  [2] 法师  [3] 刺客" << endl;
    int job_choice;
    job_choice = safeReadInt();
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
      cout << "无效职业，默认选择战士。" << endl;
      ptr = make_unique<Warrior>(player_name);
    }
  }

  // ────────────────────────────
  //  主循环
  // ────────────────────────────
  while (floor <= 100) {
    vector<Item> shopItems;
    for (int i = 0; i < 5; i++)
      shopItems.push_back(generateRandomItem(floor));

    // 内层：本层刷怪
    while (true) {
      enemy monster = generateEnemy(floor);
      monster.displayAffixes();

      cout << "\n========================================" << endl;
      cout << "楼层：" << floor << endl;
      cout << "玩家：Lv." << ptr->getlevel() << " | HP：" << ptr->gethp()
           << " | 金币：" << ptr->getcoin() << " | MP：" << ptr->getmp()
           << endl;
      cout << "敌人：Lv." << monster.getlevel() << " | HP：" << monster.gethp()
           << endl;
      cout << "----------------------------------------" << endl;
      cout << "[1] 战斗  [2] 背包  [3] 装备  [4] 退出\n"
           << "[5] 存档  [6] 查看状态" << endl; // ★ 新增选项 5 / 6

      int choice;
      choice = safeReadInt();

      // ── 存档 ──────────────────────────────────────────
      if (choice == 5) {
        SaveSystem::saveGame(*ptr, floor);
        continue;
      }

      if (choice == 6) {
        ptr->printstats();
        continue;
      }

      if (choice == 2) {
        ptr->printBackpack();
        continue;
      }

      if (choice == 3) {
        ptr->showEquippedItems();
        continue;
      }

      if (choice == 4) {
        // ── 退出前询问是否存档 ──────────────────────
        cout << "退出前是否保存游戏？[1] 是  [2] 否" << endl;
        int sc;
        sc = safeReadInt();
        if (sc == 1)
          SaveSystem::saveGame(*ptr, floor);
        cout << "再见！" << endl;
        return 0;
      }

      // ── 战斗 ─────────────────────────────────────────
      if (choice == 1) {
        // 事件
        bool eventKilled = hadEvent(*ptr, floor);
        if (eventKilled) {
          if (ptr->getfreerevives() > 0 && floor < 10) {
            cout << "你有 " << ptr->getfreerevives()
                 << " 次免费复活机会，是否使用？[1] 是  [2] 否" << endl;
            int rc;
            rc = safeReadInt();
            if (rc == 1) {
              ptr->decreaserevives();
              ptr->revive();
              floor = 1;
              break;
            } else {
              cout << "游戏结束。" << endl;
              return 0;
            }
          } else if (ptr->getcoin() >= 20) {
            cout << "是否花费 20 金币复活？[1] 是  [2] 否" << endl;
            int rc;
            rc = safeReadInt();
            if (rc == 1) {
              ptr->costcoin(20);
              ptr->revive();
              if (floor > 1)
                floor--;
              break;
            } else {
              cout << "游戏结束。" << endl;
              return 0;
            }
          } else {
            cout << "金币不足，无法复活，游戏结束！" << endl;
            return 0;
          }
        }

        bool won = battle(*ptr, monster);

        if (won) {
          // ── 战后菜单 ────────────────────────────
        post_battle_decision:
          cout << "\n战后选择：\n"
               << "[1] 继续刷怪  [2] 前往下一层  [3] 退出\n"
               << "[4] 查看状态  [5] 背包        [6] 装备\n"
               << "[7] 商店      [8] 技能        [9] 存档" << endl; // ★ 选项 9

          int pbc;
          pbc = safeReadInt();

          if (pbc == 1) {
            continue;
          }

          else if (pbc == 2) {
            if (floor == 100)
              cout << "恭喜通关！" << endl;
            else
              cout << "前往下一层..." << endl;
            shopItems.clear();
            floor++;
            break;
          }

          else if (pbc == 3) {
            cout << "退出前是否保存游戏？[1] 是  [2] 否" << endl;
            int sc;
            sc = safeReadInt();
            if (sc == 1)
              SaveSystem::saveGame(*ptr, floor);
            cout << "再见！" << endl;
            return 0;
          }

          else if (pbc == 4) {
            ptr->printstats();
            goto post_battle_decision;
          }

          else if (pbc == 5) {
            ptr->printBackpack();
            cout << "[1] 使用物品  [2] 装备物品  [3] 返回" << endl;
            int bc;
            bc = safeReadInt();
            if (bc == 1) {
              cout << "输入物品序号：";
              int idx;
              idx = safeReadInt();
              ptr->useItem(idx);
            } else if (bc == 2) {
              cout << "输入物品序号：";
              int idx;
              idx = safeReadInt();
              ptr->equipItem(idx);
            }
            goto post_battle_decision;
          }

          else if (pbc == 6) {
            ptr->showEquippedItems();
            cout << "[1] 卸下装备  [2] 返回" << endl;
            int ec;
            ec = safeReadInt();
            if (ec == 1) {
              cout << "输入装备序号：";
              int idx;
              idx = safeReadInt();
              ptr->unequipItem(idx);
            }
            goto post_battle_decision;
          }

          else if (pbc == 7) {
            showshop(*ptr, floor, shopItems);
          post_shop_decision:
            cout << "[1] 购买  [2] 返回  [3] 刷新商店（20金币）" << endl;
            int sc;
            sc = safeReadInt();
            if (sc == 1) {
              bool buying = true;
              while (buying) {
                if (shopItems.empty()) {
                  cout << "商店已售罄！是否刷新（20金币）？[1] 是  [2] 否"
                       << endl;
                  int rc;
                  rc = safeReadInt();
                  if (rc == 1 && ptr->getcoin() >= 20) {
                    ptr->costcoin(20);
                    for (int i = 0; i < 5; i++)
                      shopItems.push_back(generateRandomItem(floor));
                    showshop(*ptr, floor, shopItems);
                  } else
                    break;
                }
                cout << "输入购买序号：";
                int bi;
                bi = safeReadInt();
                if (bi >= 1 && bi <= (int)shopItems.size()) {
                  Item item = shopItems[bi - 1];
                  if (ptr->getcoin() >= item.getPrice()) {
                    ptr->costcoin(item.getPrice());
                    ptr->addItem(item);
                    shopItems.erase(shopItems.begin() + bi - 1);
                    cout << "购买了 " << item.getName() << "！" << endl;
                  } else
                    cout << "金币不足！" << endl;
                } else
                  cout << "无效序号！" << endl;
                showshop(*ptr, floor, shopItems);
                cout << "继续购买？[1] 是  [2] 否" << endl;
                int again;
                again = safeReadInt();
                if (again != 1)
                  buying = false;
              }
            } else if (sc == 3) {
              if (ptr->getcoin() >= 20) {
                ptr->costcoin(20);
                shopItems.clear();
                for (int i = 0; i < 5; i++)
                  shopItems.push_back(generateRandomItem(floor));
                showshop(*ptr, floor, shopItems);
              } else
                cout << "金币不足，无法刷新商店！" << endl;
              goto post_shop_decision;
            }
            goto post_battle_decision;
          }

          else if (pbc == 8) {
          post_skill_up:
            ptr->showSkills(*ptr);
            cout << "[1] 升级技能  [2] 返回" << endl;
            int su;
            su = safeReadInt();
            if (su == 1) {
              cout << "输入技能序号：";
              int si;
              si = safeReadInt();
              ptr->levelUpSkill(si);
              goto post_skill_up;
            }
            goto post_battle_decision;
          }

          // ★ 战后存档选项
          else if (pbc == 9) {
            SaveSystem::saveGame(*ptr, floor);
            goto post_battle_decision;
          }

        } else {
          // ── 死亡处理 ─────────────────────────────
          // 优先使用免费复活（无楼层限制）
          if (ptr->getfreerevives() > 0) {
            cout << "你还有 " << ptr->getfreerevives()
                 << " 次免费复活机会，是否使用？[1] 是  [2] 否" << endl;
            int rc;
            rc = safeReadInt();
            if (rc == 1) {
              ptr->decreaserevives();
              ptr->revive();
              if (floor > 1) {
                floor--;
              }
              cout << "已免费复活！退回到第 " << floor << " 层。" << endl;
              break;
            }
          }
          // 无免费复活：尝试花金币复活
          if (ptr->getcoin() == 0) {
            cout << "没有金币，也没有免费复活次数！游戏结束！" << endl;
            return 0;
          }
          int coinLoss = max(1, ptr->getcoin() / 2);
          cout << "\n是否花费 " << coinLoss << " 金币复活？"
               << "[1] 复活（退后一层）  [2] 放弃" << endl;
          int rc;
          rc = safeReadInt();
          if (rc == 1) {
            ptr->revive();
            ptr->costcoin(coinLoss);
            cout << "已复活！损失 " << coinLoss << " 金币。" << endl;
            if (floor > 1) {
              floor--;
              cout << "退回到第 " << floor << " 层。" << endl;
            } else
              cout << "在第 1 层复活。" << endl;
            break;
          } else {
            cout << "游戏结束！" << endl;
            return 0;
          }
        }
      } // end choice == 1
    } // end inner while
  } // end outer while

  cout << "游戏结束！感谢游玩！" << endl;
  return 0;
}