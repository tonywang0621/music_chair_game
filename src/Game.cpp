#include "Game.h"
#include "GameObjectFactory.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
Game:: Game():player(nullptr),musicPlaying(true),musicTimer(0.0f),musicDuration(5.0f),round(1),gameOver(false),sitTimer(0.0f),sitDuration(3.0f),playerLose(false),
npcSitTimer(0.0f),npcSitDelay(1.5f),npc_has_tried_to_sit(false),roundResolved(false){
}

void Game :: init(){
    musicDuration = getRandomMusicDuration();
    InitWindow(800,600,"Musical Chairs Game"); 
    srand(time(nullptr));
    InitAudioDevice();
    music = LoadMusicStream("chair_game_music/game.mp3");
    sitSound = LoadSound("chair_game_music/chair.wav");
    PlayMusicStream(music);


    SetTargetFPS(60);


    player = GameObjectFactory :: createPlayer();

    chairs.push_back(
        GameObjectFactory::createChair(Vector2{370, 220})
    );

    chairs.push_back(
        GameObjectFactory::createChair(Vector2{280, 350})
    );

    chairs.push_back(
        GameObjectFactory::createChair(Vector2{460, 350})
    );

    npcs.push_back(
        GameObjectFactory::createNPC(Vector2{250, 500}, "NPC1", ORANGE)
    );

    npcs.push_back(
        GameObjectFactory::createNPC(Vector2{550, 500}, "NPC2", PURPLE)
    );

}


void Game::draw(){

    BeginDrawing();
    ClearBackground(RAYWHITE);

    for (Chair* chair : chairs) {
        chair->draw();
    }
    if (!player->isSitting()) {
        player->draw();
    }

    // 畫出所有還沒出局、還沒坐下的 NPC
    for (NPC* npc : npcs) {
        if (!npc->isEliminated() && !npc->isSitting()) {
            npc->draw();
        }
    }

    for (Chair* chair : chairs) {
        if (chair->isOccupied()) {
            Vector2 chairPos = chair->getPosition();
            Vector2 chairSize = chair->getSize();

            const char* nameText = chair->getOccupantName().c_str();

            int fontSize = 20;
            int textWidth = MeasureText(nameText, fontSize);

            float textX = chairPos.x + chairSize.x / 2 - textWidth / 2;
            float textY = chairPos.y + chairSize.y / 2 - fontSize / 2;

            DrawText(nameText, textX, textY, fontSize, WHITE);
        }
    }

    DrawText(TextFormat("Round: %d",round),20,60,30,BLACK); //TextFormat 將字串轉為char 指標


    if (gameOver) {
        if (playerLose) {
            DrawText("YOU LOSE!", 280, 250, 60, RED);
        } else {
            DrawText("YOU WIN!", 300, 250, 60, RED);
        }
    }

    if(!musicPlaying && !player->isSitting() && !gameOver){
        float timeleft = sitDuration - sitTimer;

        DrawText(TextFormat("Sit Time Left: %.1f",timeleft),20,100,30,RED);
    }

    if (roundResolved && !gameOver) {
        DrawText("Press ENTER for next round", 220, 520, 30, BLACK);
    }
    
    EndDrawing();
} 
void Game::update(){
    UpdateMusicStream(music);

    if(gameOver) return;

    if(musicPlaying){
        musicTimer = musicTimer +GetFrameTime();

        if(musicTimer>=musicDuration){
            musicPlaying = false;
            StopMusicStream(music);
        }
    }

    if (!player->isSitting()) {
        controller.update(*player);
    }

    move_npc_during_music();
    check_player_npc_collision();

    check_player_too_close_to_chair();

    if (!musicPlaying && !roundResolved) {
        sitTimer += GetFrameTime();

        if (sitTimer >= sitDuration) {
            resolveRound();
        }
    }
    if (!musicPlaying) {
        npcSitTimer += GetFrameTime();

        if (npcSitTimer >= npcSitDelay) {
            let_npc_sit();
        }
    }

    // 音樂停止後，NPC 如果有目標椅子，就往椅子移動
    if (!musicPlaying) {
        for (NPC* npc : npcs) {
            if (!npc->isEliminated() && !npc->isSitting()) {
                npc->update();
            }
        }
    }
    check_player_sit();

    if (roundResolved && !gameOver) {
        if (IsKeyPressed(KEY_ENTER)) {
            startNextRound();
        }
    }
}

void Game::run(){
    while(!WindowShouldClose()){
        update();
        draw();
    }
}

void Game:: shutDown(){
    delete player;

    for (Chair* chair : chairs) {
        delete chair;
    }

    player = nullptr;
    chairs.clear();

    for (NPC* npc : npcs) {
        delete npc;
    }

    npcs.clear();

    UnloadMusicStream(music);
    UnloadSound(sitSound);
    CloseAudioDevice();
    CloseWindow();

}

Game::~Game(){
};

void Game:: check_player_sit(){
    if(musicPlaying) return;
    if(player->isSitting()) return;



    if(IsKeyPressed(KEY_SPACE)){
        for (Chair* chair : chairs) {

            // 如果這張椅子已被占用
            if (chair->isOccupied()) {
                continue;
            }

            Vector2 playerPos = player->getPosition();
            Vector2 chairPos = chair->getPosition();
            Vector2 chairSize = chair->getSize();

            Vector2 chairCenter = {
                chairPos.x + chairSize.x / 2,
                chairPos.y + chairSize.y / 2
            };

            float dx = playerPos.x - chairCenter.x;
            float dy = playerPos.y - chairCenter.y;

            float distance = sqrt(dx * dx + dy * dy);

            // 距離夠近
            if (distance < 50.0f) {
                player->setSitting(true);
                chair->setOccupied(true);
                chair->setReserved(true);
                chair->setOccupantName(player->getName());
                PlaySound(sitSound);
                
                if(chairs.size()==1) gameOver = true;


                break;
            }
        }

    }

}

void Game::startNextRound(){
    player->setSitting(false);

    for (NPC* npc : npcs) {
        if (!npc->isEliminated()) {
            npc->resetForNextRound();

            for (int i = 0; i < npcs.size(); i++) {
                if (i < chairs.size()) {
                    npcs[i]->setTargetChair(chairs[i]);
                }
            }
        }
    }
    player->setPosition(Vector2{400, 500});

    if(chairs.size()>1){
        delete chairs.back();
        chairs.pop_back();
    }
    
    for (Chair* chair : chairs) {
        chair->setOccupied(false);
        chair->setReserved(false);
        chair->setOccupantName("");
    }


    musicPlaying = true;
    PlayMusicStream(music);
    musicTimer =0.0f;
    musicDuration = getRandomMusicDuration();
    round = round +1;

    sitTimer = 0.0f;
    playerLose = false;
    npcSitTimer = 0.0f;
    npc_has_tried_to_sit = false;
    roundResolved = false;
}
void Game:: check_player_too_close_to_chair(){
    if(!musicPlaying) return;


    for (Chair* chair : chairs) {
        Vector2 playerPos = player->getPosition();
        Vector2 chairPos = chair->getPosition();
        Vector2 chairSize = chair->getSize();

        Vector2 chairCenter = {
            chairPos.x + chairSize.x / 2,
            chairPos.y + chairSize.y / 2
        };

        float dx = playerPos.x - chairCenter.x;
        float dy = playerPos.y - chairCenter.y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < 90.0f) {
            player->setPosition(Vector2{400, 500});
            break;
        }
    }
}

void Game::let_npc_sit() {
    for (NPC* npc : npcs) {
        if (npc->isEliminated() || npc->isSitting()) {
            continue;
        }

        if (npc->getTargetChair() != nullptr) {
            continue;
        }

        for (Chair* chair : chairs) {
            if (!chair->isOccupied() && !chair->isReserved()) {
                npc->setTargetChair(chair);
                chair->setReserved(true);
                break;
            }
        }
    }
}

void Game::resolveRound() {
    // 玩家沒有坐到椅子，就直接輸
    if (!player->isSitting()) {
        playerLose = true;
        gameOver = true;
        roundResolved = true;
        return;
    }

    // 玩家有坐到，而且只剩最後一張椅子，玩家勝利
    if (chairs.size() == 1) {
        gameOver = true;
        roundResolved = true;
        return;
    }

    // 玩家有坐到，這回合結束，等待按 Enter 下一局
    roundResolved = true;
}

float Game::getRandomMusicDuration() {
    return 3.0f + static_cast<float>(rand() % 6);
}



void Game::move_npc_during_music() {
    if (!musicPlaying) {
        return;
    }

    Vector2 targets[4] = {
        Vector2{250, 250},
        Vector2{550, 250},
        Vector2{550, 450},
        Vector2{250, 450}
    };

    for (int i = 0; i < npcs.size(); i++) {
        NPC* npc = npcs[i];

        if (npc->isEliminated() || npc->isSitting()) {
            continue;
        }

        int targetIndex = (round + i) % 4;
        npc->moveToward(targets[targetIndex]);
    }
}



// CheckPlayerNPCCollision：玩家和 NPC 碰到時，雙方往反方向彈開
void Game::check_player_npc_collision() {
    if (player->isSitting()) {
        return;
    }

    Vector2 playerPos = player->getPosition();
    float playerRadius = player->getRadius();

    for (NPC* npc : npcs) {
        if (npc->isEliminated() || npc->isSitting()) {
            continue;
        }

        Vector2 npcPos = npc->getPosition();
        float npcRadius = npc->getRadius();

        float dx = playerPos.x - npcPos.x;
        float dy = playerPos.y - npcPos.y;
        float distance = sqrt(dx * dx + dy * dy);

        float minDistance = playerRadius + npcRadius;

        if (distance < minDistance && distance > 0) {
            float nx = dx / distance;
            float ny = dy / distance;

            float pushDistance = 20.0f;

            player->setPosition(Vector2{
                playerPos.x + nx * pushDistance,
                playerPos.y + ny * pushDistance
            });

            npc->setPosition(Vector2{
                npcPos.x - nx * pushDistance,
                npcPos.y - ny * pushDistance
            });
        }
    }
}