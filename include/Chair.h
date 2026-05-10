#pragma once
#include "GameObject.h"
#include <string>
using namespace std;
//代表遊戲的椅子
//繼承 GameObject

class Chair : public GameObject{

private:

    Vector2 size;
    bool occupied; //椅子是否已被坐走
    string occupantName;//紀錄坐在這椅子上的人
    bool reserved; //椅子是否被NPC 預約

public:

    Chair(Vector2 position,Vector2 size ,Color color);
    void draw() override;
    void setOccupied(bool occupied);
    bool isOccupied() const;

    Vector2 getSize()const;

    void setOccupantName(string name);
    string getOccupantName() const;

    void setReserved(bool reserved);
    bool isReserved() const;
};