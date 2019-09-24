#ifndef _GAME_H
#define _GAME_H
#include <iostream>
#include <fstream>
#include <vector>

// #include "character.h"
#include "cell.h"
#include "floor.h"

// class Cell;

class Game{

	bool merchantAggro = false; //true if merchant is hostile
	bool stopNonPC = false; //true if all characters except player character stop moving
	Cell* curPlayer = nullptr; //a pointer to Cell where a player character is in
	std::string curRace = "";
	Cell* curStair = nullptr;
	std::vector<std::vector<std::vector<Cell*>>> floors; //5 floors plan of cells
	std::vector<int> enemyCount; //# of enemies in each 5 floor given in source file
	std::vector<int> potionCount; //# of potions in each 5 floor given in source file
	std::vector<int> goldCount; //# of golds in each 5 floor given in source file
	// TextDisplay *td; //a pointer to text display
	int curLevel = 0; //a current level among 5 floors
	Floor* curFloor; //a pointer to a current floor
	std::vector<Cell*> players;


 public:
 	~Game();
 	void init(std::string src);
 	void setPlayer(std::string race);
 	void movePlayer(std::string direction);
 	void usePotion(std::string direction);
 	void attack(std::string direction);
 	void stop();
 	void restart();

 	void setLevel(int nextLevel);
	void setMerchantAggro(bool status);

	bool getStopNonPC();
	int getEnemyCount();
	int getPotionCount();
	int getGoldCount();

	// void notify(Subject &whoFrom) override;
 	friend std::ostream &operator<<(std::ostream &out, const Game &gd);
};
#endif




