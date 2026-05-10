#include "chair.h"
#include "raylib.h"
#include <string>

using namespace std;

Chair :: Chair(Vector2 position,Vector2 size,Color color):
GameObject(position,color),size(size),occupied(false),reserved(false){
}

void Chair :: draw(){
    DrawRectangleV(position, size, color);
}

void Chair :: setOccupied(bool occupied){
    this->occupied = occupied;
    if(occupied){
        color =RED;
    }
    else{
        color=BROWN;
        occupantName ="";
        reserved = false;
    }
}

bool Chair :: isOccupied()const{
    return occupied;
}

Vector2 Chair:: getSize() const{
    return size;
}


void Chair::setOccupantName(string name) {
    occupantName = name;
}

string Chair::getOccupantName() const {
    return occupantName;
}

void Chair::setReserved(bool reserved) {
    this->reserved = reserved;
}

bool Chair::isReserved() const {
    return reserved;
}