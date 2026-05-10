#include<GameObjectFactory.h>

Player* GameObjectFactory::createPlayer(){
    return new Player(Vector2{400,500},20.0f,BLUE,4.0f,"player");
}

Chair* GameObjectFactory::createChair(Vector2 position){
    return new Chair(position,Vector2{60,30},BROWN);
}

NPC* GameObjectFactory::createNPC(Vector2 position, string name, Color color) {
    return new NPC(position,20.0f,color,name);        
}