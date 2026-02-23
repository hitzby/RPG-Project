#include<iostream>
#include"player/player.h"
#include"enemy/enemy.h"
using namespace std;
int main(){
    player hero("你", 100,10,20,1,0,0);
    enemy slime("史莱姆",20,5,0,100,5,1);
    cout<<"战斗开始！"<<endl;
    while(hero.isalive()&&slime.isalive()){
        hero.attacktarget(slime);
        if(!slime.isalive()){
            cout<<"史莱姆被击败了！"<<endl;
            hero.gainexp(slime.getexp_reward());
            hero.gaincoin(slime.getcoin_reward());
            break;
        }
        slime.attacktarget(hero);
        if(!hero.isalive()){
            cout<<"你被击败了！游戏结束。"<<endl;
            break;
        }
    }
    cout<<"战斗结束！"<<endl;
    cout<<"你的等级为："<<hero.getlevel()<<endl;
    cout<<"你的经验值为："<<hero.getexp()<<endl;
    cout<<"你的金币数为："<<hero.getcoin()<<endl;
    return 0;
}