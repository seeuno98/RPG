#include "decorator.h"
using namespace std;

Decorator::Decorator(string type): decoratorType{type} {}

void Decorator::setPlayer(Character* player) {
 this->player = player;
}
   
string &Decorator::getType() {
 return decoratorType;
}

Decorator::~Decorator() {}
