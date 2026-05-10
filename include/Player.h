#pragma once
#include "GameObject.h"
#include<string>
using namespace std;
//代表玩家角色
class  Player : public GameObject{
    private:
        float radius;
        float speed; //玩家移動速度
        bool _isSitting; //紀錄玩家是否坐下
        string name;

    public:
        Player(Vector2 position, float radius, Color color, float speed,string name);
        void draw() override;
        void setSitting(bool sitting); //設定玩家是否坐下
        bool isSitting() const; 
        float getSpeed() const;
        string getName() const;
        float getRadius()const;
};

