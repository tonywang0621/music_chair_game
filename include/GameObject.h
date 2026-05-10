#pragma once //確保此標頭檔在編譯過程中只被包含一次，避免重複定義導致的錯誤。
#include "raylib.h"
//遊戲物件都有共同的「位置、大小、顏色、更新、繪製」功能。
class GameObject
{
protected:
    Vector2 position; // 物件的位置
    Color color;      // 物件的顏色
public: 
    GameObject(Vector2 position,Color color);
    virtual void update(); //更新物件狀態
    virtual void draw() =0; //將物件畫出來 每個子類別的圖形不一樣
    Vector2 getPosition() const;
    void setPosition(Vector2 newPosition); //更新物件的座標
    virtual ~GameObject();
};

