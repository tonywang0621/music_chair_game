#include "Player.h"

Player::Player(Vector2 position, float radius, Color color, float speed,string name)
:GameObject(position, color),radius(radius), speed(speed), _isSitting(false){
}

void Player::draw() {
    DrawCircleV(position, radius, color);
}

void Player::setSitting(bool _isSitting){
    this->_isSitting = _isSitting;
}

bool Player::isSitting() const{
    return  _isSitting;
}

float Player::getSpeed()const{
    return speed;
}

string Player::getName()const{
    return name;
}

float Player :: getRadius()const{
    return radius;
}











