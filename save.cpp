#include "save.h"
#include "jobs.h"
#include "skills.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// ─────────────────────────────────────────────
//  内部辅助：分割字符串
// ─────────────────────────────────────────────
static vector<string> split(const string &s, char delim) {
  vector<string> tokens;
  stringstream ss(s);
  string tok;
  while (getline(ss, tok, delim))
    tokens.push_back(tok);
  return tokens;
}

// ─────────────────────────────────────────────
//  Item 序列化 / 反序列化
// ─────────────────────────────────────────────
string SaveSystem::serializeItem(const Item &item) {
  // 格式：名称|等级需求|类型|数值|耐久|价格|数量|职业限制
  ostringstream oss;
  oss << item.getName() << "|" << item.getLevelRequirement() << "|"
      << static_cast<int>(item.getType()) << "|" << item.getValue() << "|"
      << item.getDurability() << "|" << item.getPrice() << "|"
      << item.getQuantity() << "|"
      << static_cast<int>(item.getJobRequirement());
  return oss.str();
}

Item SaveSystem::deserializeItem(const string &line) {
  auto tok = split(line, '|');
  if (tok.size() < 7)
    throw runtime_error("存档物品格式错误: " + line);

  string name = tok[0];
  int levelReq = stoi(tok[1]);
  ItemType type = static_cast<ItemType>(stoi(tok[2]));
  int value = stoi(tok[3]);
  int durability = stoi(tok[4]);
  int price = stoi(tok[5]);
  int quantity = stoi(tok[6]);
  // tok[7] 可能不存在（旧存档兼容）
  JobReq jobReq = AnyJob;
  if (tok.size() >= 8)
    jobReq = static_cast<JobReq>(stoi(tok[7]));

  return Item(name, levelReq, type, value, durability, price, quantity, jobReq);
}

// ─────────────────────────────────────────────
//  保存游戏
// ─────────────────────────────────────────────
bool SaveSystem::saveGame(const player &hero, int floor,
                          const string &filename) {
  ofstream ofs(filename);
  if (!ofs.is_open()) {
    cerr << "[存档] 无法打开文件: " << filename << endl;
    return false;
  }

  ofs << "VERSION 1\n";
  ofs << "NAME " << hero.getname() << "\n";
  ofs << "JOB " << hero.getjob() << "\n";
  ofs << "LEVEL " << hero.getlevel() << "\n";
  ofs << "EXP " << hero.getexp() << "\n";
  ofs << "COIN " << hero.getcoin() << "\n";
  ofs << "HP " << hero.gethp() << "\n";
  ofs << "MAX_HP " << hero.getmaxhp() << "\n";
  ofs << "ATTACK " << hero.getattack() << "\n";
  ofs << "MP " << hero.getmp() << "\n";
  ofs << "MAX_MP " << hero.getmaxmp() << "\n";
  ofs << "CRIT_RATE " << hero.getcritRate() << "\n";
  ofs << "FREE_REVIVES " << hero.getfreerevives() << "\n";
  ofs << "FLOOR " << floor << "\n";

  // ── 背包 ──
  const auto &bp = hero.getSaveBackpack();
  ofs << "BACKPACK_COUNT " << bp.size() << "\n";
  for (const auto &item : bp)
    ofs << "ITEM " << serializeItem(item) << "\n";

  // ── 已装备 ──
  const auto &eq = hero.getSaveEquipments();
  ofs << "EQUIP_COUNT " << eq.size() << "\n";
  for (const auto &item : eq)
    ofs << "EQUIP " << serializeItem(item) << "\n";

  // ── 技能（保存技能等级与当前冷却） ──
  int skillCount = hero.getSkillCount();
  ofs << "SKILL_COUNT " << skillCount << "\n";
  for (int i = 1; i <= skillCount; ++i) {
    const skill *s = hero.getSkill(i);
    if (s)
      ofs << "SKILL " << s->getskillid() << "|" << s->getskilllevel() << "|"
          << s->getcurrentcooldown() << "\n";
  }

  ofs.flush();
  cout << "[存档] 游戏已保存到 " << filename << endl;
  return true;
}

// ─────────────────────────────────────────────
//  读档
// ─────────────────────────────────────────────
unique_ptr<player> SaveSystem::loadGame(int &floor, const string &filename) {
  ifstream ifs(filename);
  if (!ifs.is_open()) {
    cerr << "[读档] 无法打开文件: " << filename << endl;
    return nullptr;
  }

  // ── 逐行读取所有 key-value ──
  string name, job;
  int level = 1, exp = 0, coin = 0;
  int hp = 0, max_hp = 0, attack_val = 0, mp = 0, max_mp = 0;
  int critRate = 10, freeRevives = 3;
  floor = 1;

  vector<Item> backpackItems;
  vector<Item> equippedItems;

  // 技能恢复数据：id -> (skilllevel, currentcooldown)
  // 索引对应技能槽位（按读取顺序）
  vector<tuple<int, int, int>> skillData; // (id, level, cooldown)

  string line;
  while (getline(ifs, line)) {
    // 去除末尾 \r（Windows 换行）
    if (!line.empty() && line.back() == '\r')
      line.pop_back();
    if (line.empty() || line[0] == '#')
      continue;

    istringstream iss(line);
    string key;
    iss >> key;

    if (key == "VERSION") { /* 兼容性预留 */
    } else if (key == "NAME") {
      getline(iss >> ws, name);
    } else if (key == "JOB") {
      iss >> job;
    } else if (key == "LEVEL") {
      iss >> level;
    } else if (key == "EXP") {
      iss >> exp;
    } else if (key == "COIN") {
      iss >> coin;
    } else if (key == "HP") {
      iss >> hp;
    } else if (key == "MAX_HP") {
      iss >> max_hp;
    } else if (key == "ATTACK") {
      iss >> attack_val;
    } else if (key == "MP") {
      iss >> mp;
    } else if (key == "MAX_MP") {
      iss >> max_mp;
    } else if (key == "CRIT_RATE") {
      iss >> critRate;
    } else if (key == "FREE_REVIVES") {
      iss >> freeRevives;
    } else if (key == "FLOOR") {
      iss >> floor;
    } else if (key == "BACKPACK_COUNT") { /* 数量由 ITEM 行决定 */
    } else if (key == "ITEM") {
      string rest;
      getline(iss >> ws, rest);
      try {
        backpackItems.push_back(deserializeItem(rest));
      } catch (const exception &e) {
        cerr << "[读档] 背包物品解析失败: " << e.what() << endl;
      }
    } else if (key == "EQUIP_COUNT") { /* 跳过 */
    } else if (key == "EQUIP") {
      string rest;
      getline(iss >> ws, rest);
      try {
        equippedItems.push_back(deserializeItem(rest));
      } catch (const exception &e) {
        cerr << "[读档] 装备解析失败: " << e.what() << endl;
      }
    } else if (key == "SKILL_COUNT") { /* 跳过 */
    } else if (key == "SKILL") {
      string rest;
      getline(iss >> ws, rest);
      auto tok = split(rest, '|');
      if (tok.size() >= 3) {
        int sid = stoi(tok[0]);
        int slv = stoi(tok[1]);
        int scd = stoi(tok[2]);
        skillData.emplace_back(sid, slv, scd);
      }
    }
  }
  ifs.close();

  if (name.empty() || job.empty()) {
    cerr << "[读档] 存档文件损坏或格式错误！" << endl;
    return nullptr;
  }

  // ── 创建对应职业的角色（构造函数会设置基础属性并学习技能） ──
  unique_ptr<player> hero;
  if (job == "Warrior")
    hero = make_unique<Warrior>(name);
  else if (job == "Mage")
    hero = make_unique<Mage>(name);
  else if (job == "Assassin")
    hero = make_unique<Assassin>(name);
  else {
    cerr << "[读档] 未知职业: " << job << endl;
    return nullptr;
  }

  // ── 计算装备加成，还原基础属性 ──
  // saveGame 保存的 ATTACK/MAX_HP 是含装备加成的最终值。
  // equipItem_T 会再次叠加，所以先扣除加成，让 equipItem_T
  // 重新加回，避免双重叠加。
  int equip_attack_bonus = 0;
  int equip_hp_bonus = 0;
  for (const auto &item : equippedItems) {
    if (item.getType() == Weapon)
      equip_attack_bonus += item.getValue();
    else if (item.getType() == Armor)
      equip_hp_bonus += item.getValue();
  }

  int base_attack = attack_val - equip_attack_bonus;
  int base_max_hp = max_hp - equip_hp_bonus;
  // hp 不超过 base_max_hp，防止 loadStats 截断
  int base_hp = (hp <= base_max_hp) ? hp : base_max_hp;

  // ── 恢复基础数值（装备加成尚未包含） ──
  hero->loadStats(level, exp, coin, base_hp, base_max_hp, base_attack, mp,
                  max_mp, critRate, freeRevives);

  // ── 恢复背包 ──
  for (const auto &item : backpackItems)
    hero->addItem(item);

  // ── 恢复装备（equipItem_T 会将加成正确地重新叠加） ──
  for (const auto &item : equippedItems)
    hero->equipItem_T(item);

  // ── 恢复技能等级与冷却 ──
  for (size_t i = 0; i < skillData.size(); ++i) {
    auto [sid, slv, scd] = skillData[i];
    skill *s = hero->getSkill(static_cast<int>(i) + 1);
    if (s) {
      s->setskilllevelup(slv);
      // 每升一级补偿伤害和MP消耗（对应 levelUpSkill 的逻辑）
      // slv-1 次升级（初始为1）
      int extraLevels = slv - 1;
      s->setdamage(s->getdamage() + extraLevels * 10);
      s->setcostmp(s->getcostmp() + extraLevels * 2);
      // 恢复当前冷却
      for (int cd = 0; cd < scd; ++cd)
        s->resetcooldown(); // 先设最大值
      // 直接手动 patch 当前冷却
      // 因为 skill 没有 setCurrentCooldown，在此多减一步
      // 我们调用内部重置再逐步减
      s->resetcurrentcooltime(); // 置0
      for (int cd = 0; cd < scd; ++cd)
        s->resetcooldown(); // 置满，然后…
      // 重新置0，再恢复正确的剩余冷却
      s->resetcurrentcooltime();
      if (scd > 0) {
        // 将冷却置为 scd：先满后减
        s->resetcooldown(); // currentcooldown = cooldown
        int fullCd = s->getcooldown();
        for (int cd = 0; cd < (fullCd - scd); ++cd)
          s->reducecooldown(); // 减到 scd
      }
    }
  }

  cout << "[读档] 欢迎回来，" << name << "！当前楼层：" << floor << endl;
  return hero;
}

// ─────────────────────────────────────────────
//  工具函数
// ─────────────────────────────────────────────
bool SaveSystem::hasSaveFile(const string &filename) {
  ifstream f(filename);
  return f.good();
}

void SaveSystem::deleteSave(const string &filename) {
  if (remove(filename.c_str()) == 0)
    cout << "[存档] 存档文件已删除。" << endl;
  else
    cerr << "[存档] 删除存档失败：文件不存在或无权限。" << endl;
}