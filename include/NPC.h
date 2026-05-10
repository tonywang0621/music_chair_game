#pragma once

#include "GameObject.h"
#include "Chair.h"
#include <string>
using namespace std;
//對手

class NPC :public GameObject{
    private:
        float radius;
        string name;
        bool _isSitting; 
        bool eliminated; //是否已出局
        float speed;
        Chair* targetChair ;
        Vector2 startPosition; // NPC 第一局開始時的位置
    public:

        NPC(Vector2 position,float radius,Color color,string name);

        void draw() override;

        void setSitting(bool sitting);

        bool isSitting() const;

        void setEliminated(bool eliminated);

        bool isEliminated() const;

        string getName() const;

        //設定 NPC 要前往的椅子
        void setTargetChair(Chair* chair);

        //取得 NPC 目前目標椅子
        Chair* getTargetChair() const;

        // 讓 NPC 往目標椅子移動
        void update();
        void resetForNextRound();

        void moveToward(Vector2 targetPosition); //讓NPC 往某個目標方向移動
        float getRadius() const;

};