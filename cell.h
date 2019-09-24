#ifndef _CELL_H
#define _CELL_H

#include "character.h"
#include <string>
#include <memory>
#include "observer.h"
#include "subject.h"
#include "state.h"
//class Observer;
//class Subject;
class Cell: public Observer, public Subject {
    int row, col;
    State state;
    Character* character = nullptr;
    Character* character2 = nullptr;
    std::string type; //NonPath, Path, Stair
    std::string pathValue; //store the string that read from file
    bool merchantAggro = false;
    //value can be ". + # \"
    // 0 is RH restore
    // 1 is BA boost
    // 2 is BD boost
    // 3 is PH poison
    // 4 is WA wound
    // 5 is WD wound
    // 6 is normal gold pile
    // 7 is small hoard
    // 8 is merchant hoard
    // 9 is dragon hoard
    
    //invalid moce is a turn
    //invalid attack/use is not a turn
    bool tryMove(Subject &whoNotified);
    bool tryAttack(Subject &whoNotified);
    bool tryUse(Subject &whoNotified);
    bool tryCheckDragon();
    bool tryCheckEmptyCell();
    bool tryCheckSurrounding(std::string target);
	bool tryCheckSurroundingPotion();
    
public:
    Cell(int r, int c, std::string type, std::string value);
    bool notify(Subject &whoNotified);
    void setPlayer(std::string race); //generate "race" player in the cell
    void setEnemy(std::string race); //generate "race" enemy in the cell
    void setPotion(std::string potionNum); //generate random potion in the cell
    void setGold(std::string goldNum)override; //generate random gold in the cell
    void assignCharacter(Character *PC);
    void setStair(); //generate stair in the cell (change the value = "\\")
    
    //bool validPos(std::string direction);
    //return true if the cell moved toward direction is valid to move
    
    // State getState();
    Character* getCharacter() override; //return what character points
    Character* getCharacter2();
    void clearCharacter() {
        character = nullptr;
    }
    void clearCharacter2() {
        character2 = nullptr;
    }
    void setCharacter(Subject &whoNotified);
    
    bool move(std::string direction);
    bool usePotion(std::string direction);
    bool attack(std::string direction);
    bool checkSurroundingPlayer();
    bool checkSurroundingPotion();
    bool checkEmptyCell();

    
    
    void setState(std::string order, std::string direction);
    void setState(std::string order);
    State getState() override;
    std::string getValue() override; //{return value;}
    std::string getType() override;
    int getLevel() override;
    
    int getRow() {return row;}
    int getCol() {return col;}
    ~Cell(); 
};

#endif




