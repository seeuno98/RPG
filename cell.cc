#include "cell.h"
#include <iostream>
#include "invalid.h"
#include "shade.h"
#include "halfling.h"
#include "human.h"
#include "merchant.h"
#include "orcs.h"
#include "dwarf.h"
#include "elf.h"
#include "shade.h"
#include "troll.h"
#include "vampire.h"
#include "dragon.h"
#include "drow.h"
#include "goblin.h"
#include "potion.h"
#include "atkpotion.h"
#include "hppotion.h"
#include "defpotion.h"
#include "gold.h"
#include <iostream>

using namespace std;

Cell::Cell(int r, int c, string type, string value): row{r}, col{c},
type{type}, pathValue{value} {}

bool removeCharacter2(Subject &whoNotified) { //returns true if character2 exists and
    if(whoNotified.getCharacter2()) {         //delete it
        delete(whoNotified.getCharacter2());
        whoNotified.clearCharacter2();
        return true;
    }
    else return false;
}

bool Cell::tryMove(Subject &whoNotified) {
    //when whoNotified = Player Character
    if (whoNotified.getType() == "@") {
        //when the Cell contains Gold
        if (getType() == "G") {
            if (Gold *checkType =
                dynamic_cast<Gold*>(getCharacter())) {
                if (checkType->getValue() == 6) {
                    setState("checkDragon");
                    if(notifyObservers()) { //Dragon is around Dragon hoard
                        character2 = character; //pass DragonHoard to a temp character pointer;
                        character = nullptr;
                        setCharacter(whoNotified);
                        if(removeCharacter2(whoNotified)) whoNotified.setGold("9");
                        return true;
                    }
                }
            }
            Gold *G = dynamic_cast<Gold*>(getCharacter());
            G->setPlayer(whoNotified.getCharacter());
            G->ability(); //add Gold to whoNotified
            delete(getCharacter());
            clearCharacter();
            setCharacter(whoNotified); //pass character from whoNotified to current cell
            if(removeCharacter2(whoNotified)) whoNotified.setGold("9");
            return true;
        } else if (character != nullptr) {
            //when the Cell contains Enemy or Potions
            return false; //does not do anything for Player Character
        } else {
            //when the character == nullptr
            setCharacter(whoNotified);
            if(removeCharacter2(whoNotified)) whoNotified.setGold("9");
            return true;
        }
    }
    //when whoNotified = Enemy
    else {
        //when Enemy can move to the Cell
        if(!getCharacter() && (getValue() == ".")) {
            setCharacter(whoNotified);
            return true;
        }
        return false;
    }
}

bool Cell::tryAttack(Subject &whoNotified) {
    //if there is no character
    if(character == nullptr) {
        return false;
        //throw InvalidMove("not invalid attack direction");
    }
    
    //if the cell is attacked by the player character
    else if(whoNotified.getType() == "@") {
        
        if(getType() == "H" || getType() == "W" || getType() == "L" ||
           getType() == "E" || getType() == "O" || getType() == "M" ||
           getType() == "D") {
            whoNotified.getCharacter()->attack(*(this->getCharacter()));
            if(getCharacter()->getHP() < 1) {
                if(getType() == "M") { //current cell is merchant
                    delete(getCharacter());
                    clearCharacter();
                    //merchant aggro
                    setGold("8");
                }
                else if(getType() == "H") { //current cell is human
                    delete(getCharacter());
                    clearCharacter();
                    setGold("6");
                    setState("checkEmptyCell");
                    if(notifyObservers()) {
                        setState("setNormalPile");
                        notifyObservers();
                    }
                    else {
                        whoNotified.getCharacter()->affectGold(2);
                    }
                }
                else {
                    getCharacter()->goldDrop(*(whoNotified.getCharacter()));
                    delete(getCharacter());
                    clearCharacter();
                }
                //throw InvalidMove("Enemey Died!");
            }
            return true;
        }
        else return false;
    }
    
    //if the cell is attacked by the enemy
    else if(getType() == "@"){
        whoNotified.getCharacter()->attack(*(this->getCharacter()));
        if(getCharacter()->getHP() < 1) {
            throw InvalidMove("Player DIED!");
        }
        return true;
    }
    else return false;
}

bool Cell::tryUse(Subject &whoNotified) {
    //if there is no potion
    if(character == nullptr) {
        return false;
        //throw InvalidMove("not invalid attack direction");
    }
    if(getType() == "RH" || getType() == "BA" || getType() == "BD" ||
       getType() == "PH" || getType() == "WA" || getType() == "WD") {
        Potion *P = dynamic_cast<Potion*>(getCharacter());
        P->setPlayer(whoNotified.getCharacter());
        P->ability(); //potion affects player
        delete(getCharacter());
        clearCharacter();
        return true;
    }
    return false;
}

bool Cell::tryCheckDragon() {
    if(dynamic_cast<Dragon*>(getCharacter())) return true;
    else return false;
}

bool Cell::tryCheckEmptyCell() {
    if(getType() == "Path" && getValue() == ".") return true;
    else return false;
}

bool Cell::tryCheckSurrounding(string target) {
    if(getType() == target) return true;
    else return false;
}

bool Cell::tryCheckSurroundingPotion() {
	if(getType() == "RH" || getType() == "BA" || getType() == "BD" || getType() == "PH" ||
	getType() == "WA" || getType() == "WD") return true;
	else return false;
}

bool Cell::notify(Subject &whoNotified) {
    State whoFrom = whoNotified.getState();
    if(whoFrom.order == "checkDragon") {
        if(tryCheckDragon()) return true;
        else return false;
    }
    if(whoFrom.order == "checkEmptyCell") {
        if(tryCheckEmptyCell()) return true;
        else return false;
    }
    if(whoFrom.order == "setNormalPile") {
        if(!getCharacter()) {
            setGold("6");
            return true;
        }
        else return false;
    }
    if(whoFrom.order == "checkPlayer") {
        if(tryCheckSurrounding("@")) return true;
        else return false;
    }
    if(whoFrom.order == "checkPotion") {
        if(tryCheckSurroundingPotion()) return true;
        else return false;
    }
    //determine the direction from where we are notified
    if(whoFrom.row < row) {
        if(whoFrom.col < col) setState(whoFrom.order, "se");
        else if(whoFrom.col > col) setState(whoFrom.order, "sw");
        else setState(whoFrom.order, "so");
    }
    else if(whoFrom.row > row) {
        if(whoFrom.col < col) setState(whoFrom.order, "ne");
        else if(whoFrom.col > col) setState(whoFrom.order, "nw");
        else setState(whoFrom.order, "no");
    }
    else {
        if(whoFrom.col < col) setState(whoFrom.order, "ea");
        else setState(whoFrom.order, "we");
    }
    
    //if notifying direction is not the same
    if(whoFrom.direction != getState().direction) return false;
    
    //move Players/Enemy
    if(whoFrom.order == "move") {
        if(tryMove(whoNotified)) return true;
        else return false;
    }
    //Attack Player/Enemy
    if(whoFrom.order == "attack") {
        if(tryAttack(whoNotified)) return true;
        else return false;
    }
    //Use potion
    if(whoFrom.order == "use") {
        if(tryUse(whoNotified)) return true;
        else return false;
    }
    //throw InvalidMove("invalid command");
    return false;
}



void Cell::setPlayer(string race) {
    if(race == "s") {
        character = new Shade{};
    }
    else if(race == "d") {
        character = new Drow{};
    }
    else if(race == "v") {
        character = new Vampire{};
    }
    else if(race == "t") {
        character = new Troll{};
    }
    else { //race == "g"
        character = new Goblin{};
    }
    notifyObservers();
}

void Cell::setEnemy(string race) {
    if(race == "H") {
        character = new Human{};
    }
    else if(race == "W") {
        character = new Dwarf{};
    }
    else if(race == "L") {
        character = new Halfling{};
    }
    else if(race == "E") {
        character = new Elf{};
    }
    else if(race == "O") {
        character = new Orcs{};
    }
    else if(race == "M") {
        character = new Merchant{};
    } else {//race == Dragon
        character = new Dragon{};
    }
}

void Cell::setPotion(string potionNum) {
    if(potionNum == "0") {
        character = new HPpotion{1, "RH"};
    }
    else if(potionNum == "1") {
        character = new ATKpotion{1, "BA"};
    }
    else if(potionNum == "2") {
        character = new DEFpotion{1, "BD"};
    }
    else if(potionNum == "3") {
        character = new HPpotion{-1, "PH"};
    }
    else if(potionNum == "4") {
        character = new ATKpotion{-1, "WA"};
    }
    else { //potionNum == "5"
        character = new DEFpotion{-1, "WD"};
    }
    
}

void Cell::setGold(string goldNum) {
    if(goldNum == "6") {
        character = new Gold{2, "G"};
    }
    else if(goldNum == "7") {
        character = new Gold{1, "G"};
    }
    else if(goldNum == "8") {
        character = new Gold{4, "G"};
    }
    else { //goldNum == "9"
        character = new Gold{6, "G"};
    }
}

void Cell::assignCharacter(Character *PC) {
    character = PC;
}


void Cell::setStair() {
    type = "Stair";
    pathValue = "\\";
    
}

State Cell::getState() {
    return state;
}

void Cell::setState(string order, string direction) {
    state = State{row, col, order, direction};
}

void Cell::setState(string order) {
    state = State{row, col, order, ""};
}

Character* Cell::getCharacter() {
    return character;
}

Character* Cell::getCharacter2() {
    return character2;
}

void Cell::setCharacter(Subject &whoNotified) {
    character = whoNotified.getCharacter();
    whoNotified.clearCharacter();
}

bool Cell::move(string direction) {
    setState("move", direction);
    if(Troll *PC = dynamic_cast<Troll*>(getCharacter())) PC->ability();
    if(notifyObservers()) return true;
    else return false;
}

bool Cell::usePotion(string direction) {
    setState("use", direction);
    if(notifyObservers()) {
     if(Troll *PC = dynamic_cast<Troll*>(getCharacter())) PC->ability();
     return true;
    }
    else return false;
}

bool Cell::attack(string direction) {
    setState("attack", direction);
    if(notifyObservers()) {
     if(Troll *PC = dynamic_cast<Troll*>(getCharacter())) PC->ability();
     return true;
    }
    else return false;
}

bool Cell::checkSurroundingPlayer() {
    setState("checkPlayer");
    if(notifyObservers()) return true;
    else return false;
}

bool Cell::checkSurroundingPotion() {
    setState("checkPotion");
    if(notifyObservers()) return true;
    else return false;
}

bool Cell::checkEmptyCell() {
    setState("checkEmptyCell");
    if(notifyObservers()) return true;
    else return false;
}
string Cell::getValue() {
    return pathValue;
}

string Cell::getType() {
    if(character) return character->getType();
    else return type;
}

int Cell::getLevel() { return 0; }

Cell::~Cell() {
 delete character;
 delete character2;
}
