#include "Game.h"
#include "GameObjectFactory.h"
#include "raylib.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
Game:: Game():player(nullptr),musicPlaying(true),musicTimer(0.0f),musicDuration(5.0f),round(1),gameOver(false),sitTimer(0.0f),sitDuration(3.0f),playerLose(false),
npcSitTimer(0.0f),npcSitDelay(1.5f),npc_has_tried_to_sit(false),roundResolved(false),circleAngle(0.0f),circleRadius(180.0f),circleCenter(Vector2{400, 300}),circleSpeed(1.5f),playerAngle(-PI / 2),gameStarted(false){
}

void Game :: init(){
    musicDuration = getRandomMusicDuration();
    InitWindow(800,600,"Musical Chairs Game"); 
    srand(time(nullptr));
    InitAudioDevice();
    music = LoadMusicStream("chair_game_music/game.mp3");
    sitSound = LoadSound("chair_game_music/chair.wav");



    SetTargetFPS(60);


    player = GameObjectFactory :: createPlayer();

    chairs.push_back(
        GameObjectFactory::createChair(Vector2{320, 330})
    );

    chairs.push_back(
        GameObjectFactory::createChair(Vector2{460, 330})
    );

    npcs.push_back(
        GameObjectFactory::createNPC(Vector2{250, 500}, "NPC1", ORANGE)
    );

    npcs.push_back(
        GameObjectFactory::createNPC(Vector2{550, 500}, "NPC2", PURPLE)
    );

}


void Game::draw(){
    if (!gameStarted) {
        DrawText("MUSICAL CHAIRS", 210, 180, 50, BLACK);
        DrawText("Press ENTER to Start", 250, 280, 30, DARKGRAY);
        DrawText("During music: Use A / D to move", 210, 340, 24, DARKGREEN);
        DrawText("When music stops: Press SPACE near a chair", 140, 380, 24, RED);

        EndDrawing();
        return;
    }
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
        DrawText("Press R to Restart", 260, 330, 30, BLACK);
    }

    if (!musicPlaying && !player->isSitting() && !gameOver) {
        float timeleft = sitDuration - sitTimer;

        DrawText(TextFormat("Sit Time Left: %.1f", timeleft), 20, 100, 30, RED);
    }

    if (roundResolved && !gameOver) {
        DrawText("Press ENTER for next round", 220, 520, 30, BLACK);

    if (musicPlaying && !gameOver) {
        DrawText("Music Playing: Walk around the chairs", 20, 100, 24, DARKGREEN);
        DrawText("Use A / D to move on the circle", 20, 130, 24, DARKGREEN);
    }

    if (!musicPlaying && !roundResolved && !gameOver) {
        DrawText("Music Stopped! Press SPACE near a chair!", 20, 130, 24, RED);
    }

    }
    
    EndDrawing();
} 
void Game::update(){
    UpdateMusicStream(music);

    if (!gameStarted) {
    if (IsKeyPressed(KEY_ENTER)) {
        gameStarted = true;
        PlayMusicStream(music);
    }
    return;
    }

    if (gameOver) {
        if (IsKeyPressed(KEY_R)) {
            restartGame();
        }
        return;
    }

    if(musicPlaying){
        musicTimer = musicTimer +GetFrameTime();

        if(musicTimer>=musicDuration){
            musicPlaying = false;
            StopMusicStream(music);
        }
    }

    if (musicPlaying) {
        move_player_during_music();
    } else {
        if (!player->isSitting()) {
            controller.update(*player);
        }
    }

    move_npc_during_music();
    check_player_npc_collision();
    check_npc_npc_collision();


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

void Game::startNextRound() {
    player->setSitting(false);
    player->setPosition(Vector2{400, 500});

    // 先刪掉一張椅子
    if (chairs.size() > 1) {
        delete chairs.back();
        chairs.pop_back();
    }

    // 重設椅子狀態
    for (Chair* chair : chairs) {
        chair->setOccupied(false);
        chair->setReserved(false);
        chair->setOccupantName("");
    }

    // 重設 NPC
    for (NPC* npc : npcs) {
        if (!npc->isEliminated()) {
            npc->resetForNextRound();
        }
    }

    musicPlaying = true;
    PlayMusicStream(music);
    musicTimer = 0.0f;
    musicDuration = getRandomMusicDuration();
    round++;

    sitTimer = 0.0f;
    playerLose = false;
    npcSitTimer = 0.0f;
    npc_has_tried_to_sit = false;
    roundResolved = false;
    circleAngle = 0.0f;
    playerAngle = -PI / 2;
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

    float deltaTime = GetFrameTime();

    for (int i = 0; i < npcs.size(); i++) {
        NPC* npc = npcs[i];

        if (npc->isEliminated() || npc->isSitting()) {
            continue;
        }

        float angle = circleAngle + i * 2.0f * PI / (npcs.size() + 1);

        Vector2 newPosition = {
            circleCenter.x + cos(angle) * circleRadius,
            circleCenter.y + sin(angle) * circleRadius
        };

        npc->setPosition(newPosition);
    }

    circleAngle += circleSpeed * deltaTime;
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

void Game::move_player_during_music() {
    if (!musicPlaying || player->isSitting()) {
        return;
    }

    float deltaTime = GetFrameTime();

    // A D 控制順時針 / 逆時針
    if (IsKeyDown(KEY_D)) {
        playerAngle += circleSpeed * deltaTime;
    }

    if (IsKeyDown(KEY_A)) {
        playerAngle -= circleSpeed * deltaTime;
    }

    Vector2 newPosition = {
        circleCenter.x + cos(playerAngle) * circleRadius,
        circleCenter.y + sin(playerAngle) * circleRadius
    };

    player->setPosition(newPosition);
}

void Game::check_npc_npc_collision() {
    for (int i = 0; i < npcs.size(); i++) {
        NPC* npcA = npcs[i];

        if (npcA->isEliminated() || npcA->isSitting()) {
            continue;
        }

        for (int j = i + 1; j < npcs.size(); j++) {
            NPC* npcB = npcs[j];

            if (npcB->isEliminated() || npcB->isSitting()) {
                continue;
            }

            Vector2 posA = npcA->getPosition();
            Vector2 posB = npcB->getPosition();

            float dx = posA.x - posB.x;
            float dy = posA.y - posB.y;
            float distance = sqrt(dx * dx + dy * dy);

            float minDistance = npcA->getRadius() + npcB->getRadius();

            if (distance < minDistance && distance > 0) {
                float nx = dx / distance;
                float ny = dy / distance;

                float pushDistance = 10.0f;

                npcA->setPosition(Vector2{
                    posA.x + nx * pushDistance,
                    posA.y + ny * pushDistance
                });

                npcB->setPosition(Vector2{
                    posB.x - nx * pushDistance,
                    posB.y - ny * pushDistance
                });
            }
        }
    }
}

void Game::restartGame() {
    player->setSitting(false);
    player->setPosition(Vector2{400, 500});

    for (Chair* chair : chairs) {
        delete chair;
    }
    chairs.clear();

    chairs.push_back(GameObjectFactory::createChair(Vector2{320, 330}));
    chairs.push_back(GameObjectFactory::createChair(Vector2{460, 330}));

    for (NPC* npc : npcs) {
        delete npc;
    }
    npcs.clear();

    npcs.push_back(GameObjectFactory::createNPC(Vector2{250, 500}, "NPC1", ORANGE));
    npcs.push_back(GameObjectFactory::createNPC(Vector2{550, 500}, "NPC2", PURPLE));

    gameOver = false;
    playerLose = false;
    roundResolved = false;
    musicPlaying = true;
    gameStarted = true;

    round = 1;
    musicTimer = 0.0f;
    musicDuration = getRandomMusicDuration();

    sitTimer = 0.0f;
    npcSitTimer = 0.0f;
    npc_has_tried_to_sit = false;

    circleAngle = 0.0f;
    playerAngle = -PI / 2;

    PlayMusicStream(music);
}