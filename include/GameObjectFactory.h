#pragma once
#include "Player.h"
#include "Chair.h"
#include "NPC.h"

//負責建立遊戲物件

class GameObjectFactory{
    public:
        static Player* createPlayer();
        static Chair*  createChair(Vector2 position);

        static NPC* createNPC(Vector2 position,string name,Color color);
};
