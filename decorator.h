#ifndef DECORATOR_H
#define DECORATOR_H

#include <memory>
#include "character.h"
// class Character;
class Decorator: public Character {
 std::string decoratorType;
protected:
 Character* player = nullptr;
public:
 Decorator(std::string type);
 void setPlayer(Character* player);
 std::string &getType() override;
 virtual ~Decorator();

};

#endif
