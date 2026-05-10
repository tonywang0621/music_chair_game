#pragma once
                    
#include "Player.h"
#include "Chair.h"
#include "PlayerController.h"
#include "NPC.h"
#include <vector>

//開次窗 => 建立player =>建立chair =>讓鍵盤移動玩家=>畫出玩家跟椅子 =>關閉視窗

class Game{
    private:
        // 遊戲物件
        Player* player;                 // 玩家物件
        vector<Chair*> chairs;           // 存放所有椅子
        vector<NPC*> npcs;               // 存放所有 NPC
        PlayerController controller;     // 控制玩家移動

        // 遊戲狀態 
        bool gameStarted;                // 遊戲是否已開始
        bool musicPlaying;               // 音樂是否正在播放
        bool gameOver;                   // 遊戲是否結束
        bool playerLose;                 // 玩家是否輸了
        bool roundResolved;              // 本回合是否已結算

        int round;                       // 目前第幾回合

        // 音樂與音效 
        Music music;                     // 背景音樂
        Sound sitSound;                  // 成功坐下音效
        float musicTimer;                // 音樂播放計時器
        float musicDuration;             // 本回合音樂播放時間

        // 玩家搶椅子計時 
        float sitTimer;                  // 音樂停止後的搶椅子倒數計時
        float sitDuration;               // 玩家最多有幾秒可以搶椅子

        //  NPC 搶椅子計時 
        float npcSitTimer;               // NPC 開始搶椅子的計時器
        float npcSitDelay;               // NPC 等多久後開始搶椅子

        // 圓形移動設定
        float circleAngle;               // NPC 在圓形軌道上的基準角度
        float circleRadius;              // 圓形軌道半徑
        Vector2 circleCenter;            // 圓形軌道中心點
        float circleSpeed;               // 圓形移動速度
        float playerAngle;               // 玩家在圓形軌道上的角度

        // 初始化與重設 
        void createInitialChairs();      // 建立初始椅子
        void createInitialNPCs();        // 建立初始 NPC
        void resetRoundState();          // 重設每回合共用狀態
        void restartGame();              // 重新開始整個遊戲

        //  回合流程 
        void startNextRound();           // 進入下一回合
        void resolveRound();             // 結算目前回合
        float getRandomMusicDuration();  // 取得隨機音樂播放時間

        // 玩家與 NPC 行為
        void check_player_sit();                 // 判斷玩家是否成功坐下
        void check_player_too_close_to_chair();  // 音樂播放時，避免玩家太靠近椅子
        void let_npc_sit();                      // 讓 NPC 選擇椅子並開始移動過去
        void move_npc_during_music();            // 音樂播放時讓 NPC 沿圓形軌道移動
        void move_player_during_music();         // 音樂播放時讓玩家沿圓形軌道移動

        //  碰撞判斷 
        void check_player_npc_collision();       // 判斷玩家與 NPC 是否碰撞
        void check_npc_npc_collision();          // 判斷 NPC 彼此是否碰撞

        // 工具函式 
        float getDistance(Vector2 a, Vector2 b); // 計算兩點距離
        Vector2 getChairCenter(Chair* chair);    // 取得椅子中心點

    public:
        Game();          // 建構子，初始化基本變數

        void init();     // 初始化視窗、音樂、玩家、椅子與 NPC
        void run();      // 執行遊戲主迴圈
        void update();   // 每一幀更新遊戲狀態
        void draw();     // 每一幀繪製畫面
        void shutDown(); // 關閉遊戲並釋放記憶體與音效資源

        ~Game();        

                 
};
