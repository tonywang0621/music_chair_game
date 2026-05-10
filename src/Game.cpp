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
        float timeleft = sitDuration - sitDuration;

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

    check_player_too_close_to_chair();

    if (!musicPlaying && !roundResolved) {
        sitTimer += GetFrameTime();

        if (sitTimer >= sitDuration) {
            resolveRound();
        }
    }
    if (!musicPlaying && !npc_has_tried_to_sit) {
        npcSitTimer += GetFrameTime();

        if (npcSitTimer >= npcSitDelay) {
            let_npc_sit();
            npc_has_tried_to_sit = true;
        }
    }

    // 音樂停止後，NPC 如果有目標椅子，就往椅子移動
    if (!musicPlaying) {
        for (NPC* npc : npcs) {
            if (!npc->isEliminated() && !npc->isSitting()) {
                npc->moveToTargetChair();
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
        }
    }
    player->setPosition(Vector2{400, 500});

    if(chairs.size()>1){
        delete chairs.back();
        chairs.pop_back();
    }
    for (Chair* chair : chairs) {
        chair->setOccupied(false);
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

        for (Chair* chair : chairs) {
        if (!chair->isOccupied() && !chair->isReserved()){
                npc->setTargetChair(chair);
                chair->setReserved(true);
                break;
            }
        }
    }
}

// ResolveRound：回合結算
void Game::resolveRound() {
    if (!player->isSitting()) {
        for (Chair* chair : chairs) {
            if (!chair->isOccupied()) {
                player->setSitting(true);
                chair->setOccupied(true);
                chair->setOccupantName(player->getName());
                break;
            }
        }
    }

    if (!player->isSitting()) {
        playerLose = true;
        gameOver = true;
    }

    roundResolved = true;
}

float Game::getRandomMusicDuration() {
    return 3.0f + static_cast<float>(rand() % 6);
}