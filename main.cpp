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

// ══════════════════════════════════════════════════════════════
//  装备名称池（按职业 + 品质分级）
//  tier: 0=普通  1=精良  2=稀有  3=史诗
// ══════════════════════════════════════════════════════════════

// ── 战士专用武器（重型近战：剑/锤/斧/枪/弯刀）──
static const char *W_WPN[5][4] = {
    {"破旧铁剑", "精锻铁剑", "精钢巨剑", "神圣圣剑"},
    {"铁制战锤", "精钢战锤", "符文战锤", "暗金战锤"},
    {"缺口战斧", "精锻战斧", "暗铁战斧", "符文巨斧"},
    {"朽木长枪", "精钢战枪", "龙魂长枪", "神圣战枪"},
    {"生锈弯刀", "精钢弯刀", "血月弯刀", "神器弯刀"},
};
// ── 战士专用防具（重甲/盾/护腿/头盔）──
static const char *W_ARM[4][4] = {
    {"破旧链甲", "铁链甲", "精钢板甲", "秘银重甲"},
    {"木制圆盾", "铁皮盾牌", "精钢塔盾", "神盾"},
    {"皮革护腿", "铁制护腿", "精钢护腿", "龙鳞护腿"},
    {"旧铁头盔", "铁制头盔", "精钢战盔", "龙骨战盔"},
};
// ── 法师专用武器（法杖/魔杖/法典/念珠）──
static const char *M_WPN[4][4] = {
    {"朽木法杖", "水晶法杖", "龙纹法杖", "混沌魔杖"},
    {"学徒魔杖", "元素魔杖", "奥术魔杖", "虚空魔杖"},
    {"破旧法典", "元素法典", "古代法典", "禁忌法典"},
    {"玻璃念珠", "水晶念珠", "符文念珠", "星辰念珠"},
};
// ── 法师专用防具（法袍/法帽/腰带）──
static const char *M_ARM[3][4] = {
    {"粗布法袍", "学者长袍", "魔纹法衣", "星辰法衣"},
    {"布制帽子", "学者帽", "魔法高帽", "星辰法冠"},
    {"皮质腰带", "魔力腰带", "奥术腰带", "虚空腰带"},
};
// ── 刺客专用武器（短刀/弓/毒刃/飞镖）──
static const char *A_WPN[4][4] = {
    {"锈铁短刀", "寒铁匕首", "幽影刺刀", "影刃"},
    {"木弓", "角弓", "精灵长弓", "龙骨弓"},
    {"毒涂匕首", "蛇毒短刃", "幽毒利刃", "死亡之刃"},
    {"铁制飞镖", "毒针飞镖", "幻影飞镖", "神器飞镖"},
};
// ── 刺客专用防具（皮甲/披风/手套）──
static const char *A_ARM[3][4] = {
    {"破旧皮甲", "精皮甲", "影袭皮甲", "暗影战甲"},
    {"布制披风", "皮质披风", "暗影披风", "虚空披风"},
    {"皮手套", "利爪手套", "暗影手套", "死神手套"},
};
// ── 通用防具（护符/腰带）──
static const char *C_ARM[2][4] = {
    {"破旧护符", "精制护符", "圣光护符", "神器护符"},
    {"兽皮腰带", "皮腰带", "守护腰带", "圣战腰带"},
};
// ── 药水（通用）──
static const char *POTIONS[4] = {"小型治疗药水", "治疗药水", "优质治疗药水",
                                 "神圣圣水"};

// ──────────────────────────────────────────────────────────────
//  generateRandomItem：带职业限制 + 品质分级
// ──────────────────────────────────────────────────────────────
Item generateRandomItem(int floor) {
  // ── 品质判定（高层品质下限提升）──
  int r = rand() % 100;
  int tier = (r < 50) ? 0 : (r < 80) ? 1 : (r < 95) ? 2 : 3;
  tier = min(3, tier + floor / 20);
  int t = tier;

  // ── 类型：30%武器 / 35%防具 / 35%药水 ──
  int roll = rand() % 100;
  bool isWeapon = (roll < 30);
  bool isArmor = (roll < 65 && !isWeapon);
  // else: 药水

  if (!isWeapon && !isArmor) {
    int potVal = 30 + (t + 1) * 15 + floor * 3;
    int price = (t + 1) * 20 + floor * 5;
    return Item(POTIONS[t], 1, Potion, potVal, 0, price, 1, AnyJob);
  }

  // ── 数值 ──
  // 武器攻击加成：参考同层怪物攻击的30~50%，让装备有明显提升
  int baseAtk = (t + 1) * 5 + floor * 2;
  int atkVal = baseAtk + rand() % max(1, baseAtk / 3);
  // 防具HP加成：比武器高，能扛1~2刀
  int baseHp = (t + 1) * 10 + floor * 4;
  int hpVal = baseHp + rand() % max(1, baseHp / 3);
  // 耐久/价格/等级需求
  int dur = (t + 1) * 10 + floor / 2 + rand() % 6;
  int price = (t + 1) * 35 + floor * 10;
  int levelReq = max(1, floor - 3 + t);

  // ── 职业归属 ──
  // 武器：战士35% / 法师30% / 刺客35%
  // 防具：战士35% / 法师25% / 刺客25% / 通用15%
  int jr = rand() % 100;
  JobReq job;
  if (isWeapon) {
    job = (jr < 35) ? WarriorOnly : (jr < 65) ? MageOnly : AssassinOnly;
  } else {
    job = (jr < 35)   ? WarriorOnly
          : (jr < 60) ? MageOnly
          : (jr < 85) ? AssassinOnly
                      : AnyJob;
  }

  if (isWeapon) {
    const char *name;
    if (job == WarriorOnly)
      name = W_WPN[rand() % 5][t];
    else if (job == MageOnly)
      name = M_WPN[rand() % 4][t];
    else
      name = A_WPN[rand() % 4][t];
    return Item(name, levelReq, Weapon, atkVal, dur, price, 1, job);
  } else {
    const char *name;
    if (job == WarriorOnly)
      name = W_ARM[rand() % 4][t];
    else if (job == MageOnly)
      name = M_ARM[rand() % 3][t];
    else if (job == AssassinOnly)
      name = A_ARM[rand() % 3][t];
    else
      name = C_ARM[rand() % 2][t];
    return Item(name, levelReq, Armor, hpVal, dur, price, 1, job);
  }
}

// ─────────────────────────────────────────────
//  怪物生成
// ─────────────────────────────────────────────
enemy generateEnemy(int floor) {
  // ── 成长曲线（参考 Slay the Spire）──
  // HP：前期慢，中后期加速，但不至于无解
  int hp = 35 + floor * 8 + (floor * floor) / 8;
  // 攻击：约等于对应职业HP的6~10%，有压力但不秒杀
  int attack = 4 + floor * 2 + floor / 4;
  // 经验：每层刷2~3只怪能稳定升级
  int exp_reward = 30 + floor * 25;
  // 金币：前期宽裕，中期可买装备，后期刷Boss发财
  int coin_reward = 8 + floor * 12;
  int level = floor;
  string name = "Monster of Floor " + to_string(floor);

  // ── Boss关卡（每10层）──
  if (floor % 10 == 0) {
    hp = hp * 3;                   // Boss 3倍血量
    attack = attack * 2;           // Boss 2倍攻击
    exp_reward = exp_reward * 4;   // Boss 4倍经验
    coin_reward = coin_reward * 5; // Boss 5倍金币
    level += 3;
    name = "★ BOSS of Floor " + to_string(floor) + " ★";
  }

  enemy monster(name, hp, attack, 0, exp_reward, coin_reward, level);
  monster.setAffix(floor);

  // 词缀加成更明显
  if (monster.hasAffix(Affix::STRONG))
    monster.resetattack(monster.getattack() + monster.getlevel() * 3);
  if (monster.hasAffix(Affix::TOUGH))
    monster.resethp(monster.gethp() + monster.getlevel() * 25);
  if (monster.hasAffix(Affix::QUICK))
    monster.setdogeRate(
        min(monster.getdogeRate() + monster.getlevel() * 2, 45));

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
         << "\n   职业限制: " << shopItems[i].getJobRequirementName()
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
    int coins = 40 + floor * 15 + rand() % (floor * 5 + 20); // 宝箱金币有随机性
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
    // 显示职业限制提示
    if (randomItem.getJobRequirement() != AnyJob)
      cout << "  ? 此物品为【" << randomItem.getJobRequirementName() << "】"
           << endl;
    cout << "[1] 加入背包  [2] 直接装备  [3] 丢弃（获少量金币）" << endl;
    int c;
    c = safeReadInt();
    if (c == 1) {
      hero.addItem(randomItem);
      cout << "物品已加入背包！" << endl;
    } else if (c == 2) {
      hero.equipItem_T(randomItem); // 职业检查在内部处理
    } else {
      int junkGold = randomItem.getPrice() / 3;
      hero.gaincoin(junkGold);
      cout << "物品被丢弃，获得 " << junkGold << " 金币。" << endl;
    }
  } break;

  case eventType::TRAP: {
    cout << "你触发了一个陷阱！受到了一些伤害！" << endl;
    // 陷阱伤害：约等于玩家当前最大HP的8%~15%，有痛感但不致命
    int trapDamage = 8 + floor * 2 + rand() % (floor + 5);
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
    int hpR = 40 + floor * 8, mpR = 20 + floor * 4; // 温泉：大回复，鼓励冒险
    hero.sethp(hero.gethp() + hpR);
    hero.setmp(hero.getmp() + mpR);
    cout << "你恢复了 " << hpR << " 点 HP 和 " << mpR << " 点 MP！" << endl;
  } break;

  case eventType::SHRINE: {
    cout << "你发现了一个神社！许个愿吧！" << endl;
    int outcome = rand() % 3;
    if (outcome == 0) {
      cout << "神明赐予了你金币和经验！" << endl;
      int gc = 80 + floor * 30, ec = 40 + floor * 20; // 神赐：大量经验和金币
      hero.gaincoin(gc);
      hero.gainexp(ec);
      cout << "获得 " << gc << " 金币，" << ec << " 经验！" << endl;
    } else if (outcome == 1) {
      cout << "神明拒绝了你！失去了一些 HP 和 MP！" << endl;
      int hl = 10 + floor * 3, ml = 8 + floor * 3; // 神罚：较轻，避免无聊死亡
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
    hero.setmp(hero.getmp() + 8); // 每回合回复8MP，鼓励使用技能
    hero.reduceallCooldowns();

    cout << ">>> 你的回合！What do you want to do!" << endl;

    // 剧毒伤害
    if (monster.hasAffix(Affix::POSIONOUS)) {
      cout << ">>> " << monster.getname() << "【剧毒】<<<" << endl;
      int pDmg = 8 + monster.getlevel() * 3; // 剧毒伤害稍强，增加威胁感
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
        int shield = 8 + monster.getlevel() * 6; // 护盾随等级更强
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
        int expDmg = 15 + monster.getlevel() * 8; // 爆炸：高层更危险
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
      hero.heal(hero.getmaxhp() / 10); // 击杀回复10%HP，有意义但不过强
      cout << ">>> YOU WIN!" << endl;
      // 掉落
      if (rand() % 100 < 60) { // 60%掉落率
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
      int healAmt = 12 + monster.getlevel() * 6; // 治疗词缀：敌方回血更可观
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
    // needNewMonster 控制是否重新生成怪物
    // 初次进入或战斗结束后才生成新怪，查看背包/状态等不刷新
    bool needNewMonster = true;
    enemy monster = generateEnemy(floor); // 占位初始化，马上被覆盖

    while (true) {
      if (needNewMonster) {
        monster = generateEnemy(floor);
        monster.displayAffixes();
        needNewMonster = false;
      }
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
        cout << "[1] 使用物品  [2] 装备物品  [3] 丢弃物品  [4] 返回" << endl;
        int bmc = safeReadInt();
        if (bmc == 1) {
          cout << "输入物品序号：";
          int idx = safeReadInt();
          ptr->useItem(idx);
        } else if (bmc == 2) {
          cout << "输入物品序号：";
          int idx = safeReadInt();
          ptr->equipItem(idx);
        } else if (bmc == 3) {
          cout << "输入要丢弃的物品序号（丢弃可获得少量金币）：";
          int idx = safeReadInt();
          ptr->dropItem(idx);
        }
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
            needNewMonster = true; // 生成新怪物
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
            cout << "[1] 使用物品  [2] 装备物品  [3] 丢弃物品  [4] 返回"
                 << endl;
            int bc = safeReadInt();
            if (bc == 1) {
              cout << "输入物品序号：";
              int idx = safeReadInt();
              ptr->useItem(idx);
            } else if (bc == 2) {
              cout << "输入物品序号：";
              int idx = safeReadInt();
              ptr->equipItem(idx);
            } else if (bc == 3) {
              cout << "输入要丢弃的物品序号（丢弃可获得少量金币）：";
              int idx = safeReadInt();
              ptr->dropItem(idx);
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
            cout << "[1] 购买  [2] 返回  [3] 刷新商店（50金币）" << endl;
            int sc;
            sc = safeReadInt();
            if (sc == 1) {
              bool buying = true;
              while (buying) {
                if (shopItems.empty()) {
                  cout << "商店已售罄！是否刷新（50金币）？[1] 是  [2] 否"
                       << endl;
                  int rc;
                  rc = safeReadInt();
                  if (rc == 1 && ptr->getcoin() >= 50) {
                    ptr->costcoin(50);
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
              if (ptr->getcoin() >= 50) {
                ptr->costcoin(50);
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