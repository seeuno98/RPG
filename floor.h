#ifndef _FLOOR_H
#define _FLOOR_H

#include "textdisplay.h"
#include "cell.h"
#include <memory>
#include <sstream>
#include "state.h"
class Game;

class Floor {
	std::vector<std::vector<Cell*>> floorGrid;
	std::vector<std::vector<Cell*>> chambers;
	Game* game = nullptr;
	Cell* curPlayer = nullptr;
	Cell* prevPlayer = nullptr;
	std::string curRace = "";
	TextDisplay* td = nullptr;

	int goldCount;
	int enemyCount;
	int potionCount;
	int nextLevel;
	bool stopNonPC = false;
	bool merchantAggro;
	int curPlayerLoc = -1;
	int curStairLoc = -1;
	void enemyBehaviour();
	std::ostringstream action;



 public:
 	Floor(std::vector<std::vector<Cell*>> *floorPlan, Game* game, 
 		  Cell* player, Cell* prevPlayer, std::string race, int gCount,
 		  int eCount, int pCount, int nextLevel, bool merchantAggro);
 	void assignOb(int row, int col);
 	bool validPos(int row, int col, int randDirect);
 	void setPlayer(std::string race);
 	bool movePlayer(std::string direction);
 	// void checkPosAfterMove(int checkRow, int checkCol, std::string direction);
 	bool validToMove(int row, int col);
 	void usePotion(std::string direction);
 	void attack(std::string direction);
 	void stop(bool status);
	void randGenerate(int chamberNum, std::string type);
	void identifyLocation(int row, int col, std::string value, int chamberNum);
	
	Cell* findCell(Cell* whoFrom, std::string direction);
	bool getMerchantAggro();
	int getLevel();
	friend std::ostream &operator<<(std::ostream &out, const Floor &f);
	std::string getDirection(Cell* whoFrom, Cell* whoToward);

	Cell* getCurPlayer() { return curPlayer; }
	void updateTextDisplay();
	int damageCalculation(double defender, double attacker);

};

#endif






