#include "enemy/enemy.h"
#include "player/player.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

using namespace std;

// 怪物生成工厂
enemy generateEnemy(int floor) {
  int hp = 20 + floor * 5;
  int attack = 4 + floor * 1;
  int exp_reward = 10 + floor * 100;
  int coin_reward = 5 + floor * 2;
  int level = floor;

  // Boss 关卡增强
  if (floor % 10 == 0) {
    hp += 50;
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
  player hero("你", 100, 10, 10, 1, 0, 0);

  int floor = 1;

  // 修复：只要 <= 100 就继续，去掉 floor > 1 的限制
  while (floor <= 100) {

    // 内层循环：本层刷怪
    while (true) {
      enemy monster = generateEnemy(floor);

      cout << "\n========================================" << endl;
      cout << "Floor: " << floor << endl;
      cout << "Player: Lv." << hero.getlevel() << " | HP: " << hero.gethp()
           << " | Coin: " << hero.getcoin() << endl;
      cout << "Enemy : Lv." << monster.getlevel()
           << " | HP: " << monster.gethp() << endl;
      cout << "----------------------------------------" << endl;
      cout << "[1] Fight  [2] Quit Game" << endl; // 简化选项，战斗后再选去哪

      int choice;
      cin >> choice;

      if (choice == 1) {
        // === 战斗 ===
        bool result = battle(hero, monster);

        if (result) {
          // === 赢了 ===
          cout << "\nWhat do you want to do?" << endl;
        post_battle_decision:
          cout << "[1] Grind (Stay here)  [2] Next Floor  [3] Quit [4] Check "
                  "Stats"
               << endl;

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
            hero.printstats();
            goto post_battle_decision; // 打印完状态后重新选择
          } else {
            cout << "See you next time!" << endl;
            return 0;
          }
        } else {
          // === 输了 ===
          if (hero.getcoin() == 0) {
            if (floor <= 10 && hero.getfreerevives() != 0) {
              cout << "Although you don't have any coins, you can still revive "
                      "at Floor 1!"
                   << endl;
              floor = 1;
              hero.revive();
              hero.decreaserevives();
              cout << "Revived! You have " << hero.getfreerevives()
                   << " free revives left." << endl;
              break; // 跳出内层循环，重新加载楼层;
            }
            cout
                << "You have no coins to lose,or you have no free revives left!"
                << endl;
            cout << "GAME OVER" << endl;
            return 0;
          }
          int coin_loss = max(1, hero.getcoin() / 2);
          cout << "\nDo you want to revive? Cost: " << coin_loss << " coins."
               << endl;
          cout << "[1] Revive & Back 1 Floor  [2] Die & Quit" << endl;

          int revive_choice;
          cin >> revive_choice;

          if (revive_choice == 1) {
            // 记得在 player 类里实现 revive() 哦！
            hero.revive();
            // 如果还没实现，可以用 hero.gainexp(0); 这种临时方法或者 setHp

            hero.costcoin(coin_loss);
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
      } else {
        cout << "Logged out." << endl;
        return 0;
      }
    }
  }

  return 0;
}