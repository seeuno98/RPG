#include "floor.h"
#include <cstdlib>
#include <string>
#include "invalid.h"
#include "subject.h"
#include "observer.h"
#include "player.h"
#include <ctime>
#include <cmath>


using namespace std;

Floor::Floor(vector<vector<Cell*>> *floorPlan, Game* game, Cell* player,
			 Cell* prevPlayer, string race, int gCount, int eCount, int pCount, int nextLevel, bool merchantAggro):
	game{game},
	curPlayer{player},
	prevPlayer{prevPlayer},
	goldCount{gCount},
	enemyCount{eCount},
	potionCount{pCount},
	nextLevel{nextLevel},
	merchantAggro{merchantAggro} {

	curRace = race;


	//set seed for srand()
	// int seed = 1; //just for testing => later we can change rand() into rand() or srand(time)
	srand(time(NULL));

	//create temp chambers
	vector<Cell*> chamber0;
	vector<Cell*> chamber1;
	vector<Cell*> chamber2;
	vector<Cell*> chamber3;
	vector<Cell*> chamber4;
	for (int row = 0; row < 25; ++row) {
		vector<Cell*> newCellRow;
		for (int col = 0; col < 79; ++col) {

			newCellRow.emplace_back((*floorPlan)[row][col]);


			string cellType = newCellRow[col]->getType();
			string cellValue = newCellRow[col]->getValue();

			//create chambers

			if (cellValue == ".") {
				//chamber0
				if ((row > 2)&&(row < 7)&&(col > 2)&&(col < 29)) {
					chamber0.emplace_back(newCellRow[col]);
					identifyLocation(row, col, cellValue, 0);
				  //chamber1	
				} else if ((row > 14)&&(row < 22)&&(col > 3)&&(col < 29)) {
					chamber1.emplace_back(newCellRow[col]);
					identifyLocation(row, col, cellValue, 1);

				  //chamber2
				} else if ((row > 9)&&(row < 13)&&(col > 37)&&(col < 51)) {
					chamber2.emplace_back(newCellRow[col]);
					identifyLocation(row, col, cellValue, 2);

				  //chamber4
				} else if (((row > 15)&&(row < 19)&&(col > 64)&&(col < 76))||
						   ((row > 18)&&(row < 22)&&(col > 36)&&(col < 76))) {
					chamber4.emplace_back(newCellRow[col]);
					identifyLocation(row, col, cellValue, 4);

				  //chamber3
				} else {
					chamber3.emplace_back(newCellRow[col]);
					identifyLocation(row, col, cellValue, 3);
				}
			} else {
				identifyLocation(row, col, cellValue, 1);
			}		
		}
		floorGrid.emplace_back(newCellRow);
	}
	chambers.emplace_back(chamber0);
	chambers.emplace_back(chamber1);
	chambers.emplace_back(chamber2);
	chambers.emplace_back(chamber3);
	chambers.emplace_back(chamber4);


	if (td) {
		delete td;
	} 
	td = new TextDisplay{&floorGrid};
	
	//assign observers
	for (int row = 0; row < 25; ++row) {
		for (int col = 0; col < 79; ++col) {
			assignOb(row,col);
		}
	}

	if (nextLevel > 1 && prevPlayer) {
		if (prevPlayer->getState().order == "random") {
			curPlayerLoc = -1;
		}
		setPlayer(race);
		Player * prevPC = dynamic_cast<Player *>(prevPlayer->getCharacter());
		Player * curPC = dynamic_cast<Player *>(curPlayer->getCharacter());

		int prevPlayerHP = prevPC->getHP();
		int prevPlayerGold = prevPC->getGold();
		curPC->getHP() = prevPlayerHP;
		curPC->getGold() = prevPlayerGold;

		prevPlayer->clearCharacter();
		prevPlayer = nullptr;
		updateTextDisplay();
	}
	
}


bool Floor::validPos(int row, int col, int randDirect) {

	if ((row >= 0)&&(row < 25)&&(col >= 0)&&(col < 79)) {

		//check if randDirect is valid
		if (randDirect != -1) {
			if (floorGrid[row][col]->getType() == "Path") {
				return true;
			} 

		  //check only if the row, col is within Grid
		} else {
			return true;
		}
		
	}
	return false;
}

void Floor::assignOb(int row, int col) {

	for (int i = -1; i < 2; i += 2) {

		if ((validPos(row+i,col, -1))&&
			(floorGrid[row+i][col]->getType() != "NonPath")) {  //no, so
			floorGrid[row][col]->attach(floorGrid[row+i][col]);
		}
		if ((validPos(row, col+i, -1))&&
			(floorGrid[row][col+i]->getType() != "NonPath")) { //we, ea
			floorGrid[row][col]->attach(floorGrid[row][col+i]);
		}
		if ((validPos(row+i,col+i, -1))&&
			(floorGrid[row+i][col+i]->getType() != "NonPath")) { //nw, se
			floorGrid[row][col]->attach(floorGrid[row+i][col+i]);
		}
		if ((validPos(row-i,col+i, -1))&&
			(floorGrid[row-i][col+i]->getType() != "NonPath")) { //ne, sw
			floorGrid[row][col]->attach(floorGrid[row-i][col+i]);
		}
	}
}

void Floor::identifyLocation(int row, int col, string value, int chamberNum) {

	if(curPlayer && curPlayer->getRow() == row && curPlayer->getCol() == col) {
		curPlayerLoc = chamberNum;
	}
	if(value == "\\") {
		curStairLoc = chamberNum;

	}
}


void Floor::randGenerate(int chamberLoc, string type) {

	int randLocation;
	int chamberSize = chambers[chamberLoc].size();

	//generate player
	if ((type == "s")||(type == "d")||(type == "v")||
		(type == "g")||(type =="t")) {
		randLocation = rand() % chamberSize;
		while (true) {
			if (chambers[chamberLoc][randLocation]->getType() == "Path") {
				curPlayer = chambers[chamberLoc][randLocation];
				curPlayer->setPlayer(type);
				if (curPlayer->getCharacter()) {
				}
				break;
			} else {
				randLocation = rand() % chamberSize;
			}
		}

	//generate stair	
	} else if (type == "stair") {
		randLocation = rand() % chamberSize;
		while (true) {
			if (chambers[chamberLoc][randLocation]->getType() == "Path") {
				chambers[chamberLoc][randLocation]->setStair();
				break;
			} else {
				randLocation = rand() % chamberSize;
			}
		}

	  //generate potion
	} else if (type == "potion") {
		randLocation = rand() % chamberSize;
		while (true) {
			if (chambers[chamberLoc][randLocation]->getType() == "Path") {
				int pRandType = rand() % 6;
				string potionType = to_string(pRandType);
				chambers[chamberLoc][randLocation]->setPotion(potionType);
				break;
			} else {
				randLocation = rand() % chamberSize;
			}
		}

	  //generate gold
	} else if (type == "gold") {
		randLocation = rand() % chamberSize;
		while (true) {

			if (chambers[chamberLoc][randLocation]->getType() == "Path") {
				//5/8 chance of normal, 1/8 dragon hoard, 2/8 small hoard.
				int gRandType;
				while ((gRandType = rand() % 8)) {
					if (gRandType < 5) { //normal gold
						chambers[chamberLoc][randLocation]->setGold("6"); 
						break;
					} else if (gRandType < 7) { //small hoard
						chambers[chamberLoc][randLocation]->setGold("7"); 
						break;
					} else { //dragon hoard

						chambers[chamberLoc][randLocation]->setGold("9"); 
						if (chambers[chamberLoc][randLocation]->checkEmptyCell()) {
							Cell *dragonCell = chambers[chamberLoc][randLocation];
							Cell *temp;
							while(true) {
								int randDragonLoc = rand() % 8;

								if (randDragonLoc == 0) {
									temp = findCell(dragonCell, "no");
								} else if (randDragonLoc == 1) {
									temp = findCell(dragonCell, "so");
								} else if (randDragonLoc == 2) {
									temp = findCell(dragonCell, "we");
								} else if (randDragonLoc == 3) {
									temp = findCell(dragonCell, "ea");
								} else if (randDragonLoc == 4) {
									temp = findCell(dragonCell, "se");
								} else if (randDragonLoc == 5) {
									temp = findCell(dragonCell, "ne");
								} else if (randDragonLoc == 6) {
									temp = findCell(dragonCell, "nw");
								} else if (randDragonLoc == 7) {
									temp = findCell(dragonCell, "sw");
								} 
								if (!temp->getCharacter() && temp->getValue() == ".") {
									temp->setEnemy("D");
									break;
								
 								}
 							}
						}
						break;
					}
				}
				break;
			} else {
				randLocation = rand() % chamberSize;
			}
		}

	  //generate enemy
	} else if (type == "enemy") {
		randLocation = rand() % chamberSize;
		while (true) {

			if (chambers[chamberLoc][randLocation]->getType() == "Path") {

				int eRandType;
				eRandType = rand() % 18;
				if (eRandType < 4) { //(H)uman
					chambers[chamberLoc][randLocation]->setEnemy("H"); 

				} else if (eRandType < 7) { //d(W)arf
					chambers[chamberLoc][randLocation]->setEnemy("W"); 

				} else if (eRandType < 12) { //Half(L)ing
					chambers[chamberLoc][randLocation]->setEnemy("L"); 

				} else if (eRandType < 14) { //(E)lf
					chambers[chamberLoc][randLocation]->setEnemy("E"); 

				} else if (eRandType < 16) { //(O)rc
					chambers[chamberLoc][randLocation]->setEnemy("O"); 

				} else if (eRandType < 18) { //(M)erchant
					chambers[chamberLoc][randLocation]->setEnemy("M");

				}
				
				break;
			} else {
				randLocation = rand() % chamberSize;
			}
		}
	}	

}


void Floor::setPlayer(string race) {

	int randChamber;


	//STEP1: randomly assign PC and Stairs	

	//assign PC at a pre-defined location
	if (curPlayerLoc >= 0) {

		curPlayer->setPlayer(race);
		floorGrid[curPlayer->getRow()][curPlayer->getCol()]->setPlayer(race);
	}

	//randomly assign PC location
	if (curPlayerLoc < 0) {

		//assign random chamber number for PC
		randChamber = rand() % 5; 
		while (true) {
			if (randChamber != curStairLoc) {
				curPlayerLoc = randChamber;
				break;
			} else {
				randChamber = rand() % 5;
			}
		}

		//assign PC
		randGenerate(curPlayerLoc, race);
		
	}

	if (nextLevel == 1) {
		action << "Player character has spawned";
	}

	//randomly assign stairs only
	if (curStairLoc < 0) {

		//assign random chamber number for stairs
		randChamber = rand() % 5;
		while (true) {
			if (randChamber != curPlayerLoc) {
				curStairLoc = randChamber;
				break;
			} else {
				randChamber = rand() % 5;
			}
		}

		//assign stair
		randGenerate(curStairLoc, "stair");

	}

	//STEP2: randomly spawn potions
	int pCount = potionCount;
	while (pCount < 10) {
		randChamber = rand() % 5;
		randGenerate(randChamber, "potion");
		++pCount;
	}


	//STEP3: randomly spwan golds
	int gCount = goldCount;

	while(gCount < 10) {
		randChamber = rand() % 5;
		randGenerate(randChamber, "gold");
		++gCount;
	}
	//STEP4: randomly spwan enemies
	int eCount = enemyCount;

	while(eCount < 20) {
		randChamber = rand() % 5;
		randGenerate(randChamber, "enemy"); 
		++eCount;
		
	}
	updateTextDisplay();

}

void Floor::enemyBehaviour() {

	for (int row = 0; row < 25; ++row) {
		for (int col = 0; col < 79; ++col) {
			if(floorGrid[row][col]->getCharacter()) floorGrid[row][col]->getCharacter()->movedOnce = false;
		}
	}
	string direction;
    if (stopNonPC == false) {
	for (int row = 0; row < 25; ++row) {
		for (int col = 0; col < 79; ++col) {
            Cell *curPos = floorGrid[row][col];
			//When the Cell is "Enemy" except Dragon
			if ((curPos->getType() == "H")||
				 (curPos->getType() == "W")||
				 (curPos->getType() == "L")||
				 (curPos->getType() == "E")||
				 (curPos->getType() == "O")||
                 (curPos->getType() == "M")||
				 (curPos->getType() == "D")) {

				if(curPos->checkSurroundingPlayer() && (curPos->getType() != "M" ||
                   merchantAggro == true)) {
					floorGrid[row][col]->getCharacter()->movedOnce = true;
					string atkDirection = getDirection(floorGrid[row][col], curPlayer);
					string atkType = curPlayer->getCharacter()->getType();
					int prevPlayerHP = curPlayer->getCharacter()->getHP();
					try {
						floorGrid[row][col]->attack(atkDirection);
					} catch (InvalidMove &im) {
						action << " PC died";
						updateTextDisplay();
						throw InvalidMove("PC died!");

					}
					int curPlayerHP = curPlayer->getCharacter()->getHP();

					if (action.str().size() > 0) {
						action << " and ";
					}

					if (floorGrid[row][col]->getCharacter()->getType() == "G") {
						action << atkType << " died and dropped Gold";
					} else {
						action << floorGrid[row][col]->getCharacter()->getType();
						action << " deals " << prevPlayerHP - curPlayerHP << " damage to PC";
					}
				}

									
					while (!curPos->getCharacter()->movedOnce && curPos->getType() != "D") {
					int randDirect = rand() % 8;
					int randRow = row;
					int randCol = col;
						
						if (randDirect == 0) {
							direction = "no";
							randRow -= 1;

						} else if (randDirect == 1) {
							direction = "so";
							randRow += 1;

						} else if (randDirect == 2) {
							direction = "ea";
							randCol += 1;

						} else if (randDirect == 3) {
							direction = "we";
							randCol -= 1;

						} else if (randDirect == 4) {
							direction = "ne";
							randRow -= 1;
							randCol += 1;

						} else if (randDirect == 5) {
							direction = "nw";
							randRow -= 1;
							randCol -= 1;
						} else if (randDirect == 6) {
							direction = "se";
							randRow += 1;
							randCol += 1;
						} else if (randDirect == 7) {
							direction = "sw";
							randRow += 1;
							randCol -= 1;
						}

						if (curPos->getCharacter()->movedOnce == true ||
                            curPos->getType() == "D") {

							break;
						}
						
						if (curPos->move(direction)) {
							floorGrid[randRow][randCol]->getCharacter()->movedOnce = true;
							break;
						}
						//check if there is no surrounding cell that we can move
						if (!validToMove(row,col)) {
							break;
						}
					}
				}
			}
		}
	}
}

bool Floor::movePlayer(string direction) {
	
	int checkRow = curPlayer->getRow();
	int checkCol = curPlayer->getCol();

	if (direction == "no") {
		checkRow -= 1;
	} else if (direction == "so") {
		checkRow += 1;
	} else if (direction == "ea") {
		checkCol += 1;

	} else if (direction == "we") {
		checkCol -= 1;

	} else if (direction == "ne") {
		checkRow -= 1;
		checkCol += 1;

	} else if (direction == "nw") {
		checkRow -= 1;
		checkCol -= 1;
	} else if (direction == "se") {
		checkRow += 1;
		checkCol += 1;
	} else if (direction == "sw") {
		checkRow += 1;
		checkCol -= 1;
	}

	//if the Player Character steps over "Stair"
 	if (floorGrid[checkRow][checkCol]->getType() == "Stair") {
		return true;
 	}

 	//move Player Character toward "direction"
	if (curPlayer->move(direction)) {
		curPlayer = floorGrid[checkRow][checkCol];
		action << "PC moves ";

		if (direction == "no") {
			action << "North";
		} else if (direction == "so") {
			action << "South";
		} else if (direction == "ea") {
			action << "East";
		} else if (direction == "we") {
			action << "West";
		} else if (direction == "ne") {
			action << "North East";
		} else if (direction == "nw") {
			action << "North West";
		} else if (direction == "se") {
			action << "South East";
		} else if (direction == "sw") {
			action << "South West";
		}

		if (curPlayer && curPlayer->checkSurroundingPotion()) {
			action << "and sees an unknown potion";
		}
	}

	//move Enemies if stopNonPc is false
	enemyBehaviour();
	updateTextDisplay();
	return false;
}

string Floor::getDirection(Cell* whoFrom, Cell* whoToward) {

	int cmpRow = whoToward->getRow() - whoFrom->getRow();;
	int cmpCol = whoToward->getCol() - whoFrom->getCol();

	if (cmpRow == 1) {
		if (cmpCol == 1) {
			return "se";
		} else if (cmpCol == 0) {
			return "so";
		} else {
			return "sw";
		}
	} else if (cmpRow == -1) {
		if (cmpCol == 1) {
			return "ne";
		} else if (cmpCol == 0) {
			return "no";
		} else {
			return "nw";
		}
	} else if (cmpCol == -1) {
		return "we";
	} else {
		return "ea";
	}
} 

bool Floor::validToMove(int row, int col) {
	for (int i = -1; i < 2; i += 2) {

		if ((validPos(row+i, col, -1))&&
			(floorGrid[row+i][col]->getType() == "Path")) {  //no, so
			return true;
		}
		if ((validPos(row, col+i, -1))&&
			(floorGrid[row][col+i]->getType() == "Path")) { //we, ea
			return true;
			
		}
		if ((validPos(row+i, col+i, -1))&&
			(floorGrid[row+i][col+i]->getType() == "Path")) { //nw, se
			return true;
		}
		if ((validPos(row-i, col+i, -1))&&
			(floorGrid[row-i][col+i]->getType() == "Path")) { //ne, sw
			return true;
		}
	}
	return false;
}



void Floor::usePotion(string direction) {

	Cell* potionCell = findCell(curPlayer, direction);
	string potionType = potionCell->getType();
	if (curPlayer->usePotion(direction)) {
		action << "PC uses " << potionType;
		enemyBehaviour();
		updateTextDisplay();
	} 
}

void Floor::updateTextDisplay() {
	delete td;
	td = new TextDisplay{&floorGrid};
	td->updateAction(action.str());
	action.str("");
	td->update(curPlayer, nextLevel - 1);
	
}

int Floor::damageCalculation(double defender, double attacker) {
	double damage;
	damage = ceil((100 / (100+defender))*attacker);
	return static_cast<int>(damage);
}


void Floor::attack(string direction) {
	//merchantAggro
	Cell* enemyCell = findCell(curPlayer, direction);
	string enemyType = enemyCell->getType();
	double enemyDEF;
	double curATK = curPlayer->getCharacter()->getATK();
	if (enemyCell->getCharacter()) {
		enemyDEF = enemyCell->getCharacter()->getDEF();
	}

	try {
		string atkType = findCell(curPlayer, direction)->getType();
        if(findCell(curPlayer, direction)->getType() == "M") merchantAggro = true;
		if (curPlayer->attack(direction)) {
			if (action.str().size() > 0) {
				action << " and ";
			}
			if (findCell(curPlayer, direction)->getType() != "G") {
				action << "PC deals " << damageCalculation(enemyDEF, curATK) << " damage to ";
				if (!(enemyCell->getCharacter())) {
	                action << enemyType << "(" << 0 << " HP). ";
	                action << enemyType << " died";
	            } else {
	                action << enemyCell->getType() << "(" << enemyCell->getCharacter()->getHP() << " HP)";
    	        }

			} else {
				action << "PC killed " << atkType << " and ";
				action << atkType << " dropped Gold";
			}
			enemyBehaviour();
			updateTextDisplay();
		}
	} catch(InvalidMove& im) { //if throw exception only when the player dies
		action << " and PC died";
		updateTextDisplay();
		throw InvalidMove("PC died!");
    }
}

Cell* Floor::findCell(Cell* whoFrom, string direction) {
	int checkRow = whoFrom->getRow();
	int checkCol = whoFrom->getCol();
	if (direction == "no") {
		checkRow -= 1;
	} else if (direction == "so") {
		checkRow += 1;
	} else if (direction == "ea") {
		checkCol += 1;

	} else if (direction == "we") {
		checkCol -= 1;

	} else if (direction == "ne") {
		checkRow -= 1;
		checkCol += 1;

	} else if (direction == "nw") {
		checkRow -= 1;
		checkCol -= 1;
	} else if (direction == "se") {
		checkRow += 1;
		checkCol += 1;
	} else if (direction == "sw") {
		checkRow += 1;
		checkCol -= 1;
	}
	return floorGrid[checkRow][checkCol];
}
 	
void Floor::stop(bool status) {
	stopNonPC = status;
}


int Floor::getLevel() {
	return nextLevel;
}


bool Floor::getMerchantAggro() {
	return merchantAggro;
}

ostream &operator<<(std::ostream &out, const Floor &f) {
	out << *(f.td);
	return out;
}



