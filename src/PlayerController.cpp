#include "PlayerController.h"
#include "raylib.h"



void PlayerController:: update(Player& player){
    Vector2 position = player.getPosition(); 
    float speed = player.getSpeed();

    if (IsKeyDown(KEY_W)) {
        position.y = position.y - speed;
    }

    if (IsKeyDown(KEY_S)) {
        position.y = position.y + speed;
    }

    if (IsKeyDown(KEY_A)) {
        position.x = position.x - speed;
    }

    if (IsKeyDown(KEY_D)) {
        position.x = position.x + speed;
    }

    player.setPosition(position);
}
