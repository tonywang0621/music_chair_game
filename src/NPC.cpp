#include "NPC.h"
#include "raylib.h"
#include "cmath"
using namespace std;

NPC::NPC(Vector2 position, float radius, Color color, std::string name)
    : GameObject(position, color),
      radius(radius),
      name(name),
      _isSitting(false),
      eliminated(false),speed(2.5f),targetChair(nullptr),startPosition(position) {
}

void NPC ::draw(){
    DrawCircleV(position,radius,color);
}

void NPC::setSitting(bool sitting) {
    _isSitting = sitting;
}

bool NPC::isSitting() const {
    return _isSitting;
}

void NPC::setEliminated(bool eliminated) {
    this->eliminated = eliminated;
}


bool NPC::isEliminated() const {
    return eliminated;
}
string NPC::getName() const {
    return name;
}

void NPC::setTargetChair(Chair* chair) {
    targetChair = chair;
}

Chair* NPC::getTargetChair() const {
    return targetChair;
}

void NPC::moveToTargetChair() {
    if (targetChair == nullptr) {
        return;
    }

    if (targetChair->isOccupied()) {
        targetChair = nullptr;
        return;
    }
    if (targetChair == nullptr) {
        return;
    }

    Vector2 chairPos = targetChair->getPosition();
    Vector2 chairSize = targetChair->getSize();

    Vector2 chairCenter = {
        chairPos.x + chairSize.x / 2,
        chairPos.y + chairSize.y / 2
    };

    Vector2 direction = {
        chairCenter.x - position.x,
        chairCenter.y - position.y
    };

    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance <= speed) {
        position = chairCenter;
        setSitting(true);
        targetChair->setOccupied(true);
        targetChair->setOccupantName(name);
        return;
    }

    direction.x /= distance;
    direction.y /= distance;

    position.x += direction.x * speed;
    position.y += direction.y * speed;
}

void NPC::resetForNextRound() {
    position = startPosition;   // 回到最初位置
    _isSitting = false;          // 下一局重新站起來
    targetChair = nullptr;      // 清除上一局的目標椅子
}

float NPC::getRadius()const{
    return radius;
}

void NPC::moveToward(Vector2 targetPosition) {
    Vector2 direction = {
        targetPosition.x - position.x,
        targetPosition.y - position.y
    };

    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance <= speed || distance == 0) {
        return;
    }

    direction.x /= distance;
    direction.y /= distance;

    position.x += direction.x * speed;
    position.y += direction.y * speed;
}