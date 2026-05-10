#pragma once
                    
#include "Player.h"
#include "Chair.h"
#include "PlayerController.h"
#include "NPC.h"
#include <vector>

//開次窗 => 建立player =>建立chair =>讓鍵盤移動玩家=>畫出玩家跟椅子 =>關閉視窗

class Game{
    private:
        Player* player; //玩家物件
        vector<Chair*> chairs; //存放椅子物件
        vector<NPC*> npcs; //存放NPC 物件
        PlayerController controller; //玩家控制器

        void check_player_sit(); //判斷玩家有沒有成功坐到椅子
        void check_player_too_close_to_chair();

        bool musicPlaying; //音樂是否正再撥放

        float musicTimer; //音樂播放計時器
        float musicDuration; //音樂撥放多久
        int round;

        bool gameOver; //判斷遊戲是否結束

        float sitTimer; //音樂停止後 玩家搶椅子的計時器
        float sitDuration; //玩家最多可以花多久搶椅子
        bool playerLose;

        Music music;
        Sound sitSound; //成功坐下音效


        float npcSitTimer; //npc等多久開始搶椅子
        float npcSitDelay; //搶椅子的移動瞬間
        bool npc_has_tried_to_sit; //npc是否已搶過椅子
        void let_npc_sit(); //讓npc自動搶椅子

        void resolveRound(); //判斷玩家是否能進到下一局
        bool roundResolved; //確認回合結束 能進入下一局
        
        float getRandomMusicDuration(); // 取得隨機音樂播放時間

//要讓玩家 NPC 圍著椅子走的設置
        float circleAngle;
        float circleRadius;
        Vector2 circleCenter;
        float circleSpeed;
        float playerAngle;


        bool gameStarted; //遊戲開始了嗎
        void restartGame();

    
//



    public:
        Game();

        void init(); //初始化遊戲物件
        void run(); //執行遊戲主迴圈
        void update(); //更新遊戲狀態
        void draw(); //繪製遊戲畫面
        void shutDown(); //關閉遊戲視窗 並釋放記憶體
        void startNextRound();

        void move_npc_during_music();
        void move_player_during_music();

        void check_player_npc_collision();
        void check_npc_npc_collision();




        ~Game();

                 
};
