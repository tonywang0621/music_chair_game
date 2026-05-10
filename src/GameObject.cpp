#include<GameObject.h>

GameObject::GameObject(Vector2 position,Color color):
position(position),color(color){   
}

void GameObject::update(){
}

Vector2 GameObject::getPosition()const{
    return position;
}

void GameObject::setPosition(Vector2 newPosition){
    position = newPosition;
}

GameObject::~GameObject() {
}