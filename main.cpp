#include <iostream>
#include "character/character.h" // 根据你的文件夹结构调整
#include "player/player.h"
using namespace std;
int main() {
    // 1. 创建玩家：桐人, 100血, 10攻, 10蓝, 1级, 0经验, 0金币
    player p1("桐人", 100, 10, 10, 1, 0, 0);
    
    // 2. 创建怪物：史莱姆, 30血, 5攻, 0蓝
    // 注意：这里我们可以直接用父类 Character 创建怪物，或者以后写个 Enemy 子类
    Character slime("史莱姆", 30, 5, 0);

    cout << "--- 战斗开始 ---" << endl;

    // 3. 回合制互殴
    while (p1.isalive() && slime.isalive()) {
        // 玩家打怪
        p1.attacktarget(slime);
        if (!slime.isalive()) {
            cout << "史莱姆挂了！" << endl;
            p1.gainexp(150); // 测试升级逻辑：1级升2级需要100经验，应该能升级并剩50经验
            p1.gaincoin(10);
            break;
        }

        // 怪打玩家
        slime.attacktarget(p1);
    }

    // 4. 查看玩家状态
    cout << "--- 战斗结束 ---" << endl;
    cout << p1.getname() << " 等级: " << p1.getlevel() << endl;
    cout << "当前HP: " << p1.gethp() << endl;

    return 0;
}