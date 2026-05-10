#pragma once
#include "player.h"

//此class處理玩家輸入
class PlayerController{
    public:
        void update(Player& player); //讀取鍵盤輸入 根據WASD移動玩家
};
