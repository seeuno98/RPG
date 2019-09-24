#include <stdexcept>
#include "game.h"
#include "textdisplay.h"
#include "invalid.h"
#include "floor.h"
using namespace std;


Game::~Game() {
	floors.clear();
	enemyCount.clear();
	potionCount.clear();
	goldCount.clear();
}

void Game::init(string src) {
	ifstream file {src};
	char c;
	// int count = 0;
	int eCount = 0;//enemyCount for each floor
	int pCount = 0;//potionCount for each floor
	int gCount = 0;//goldCount for each floor
	int readInt = -1;
	string type = "Path";
	for (int floorNum = 0; floorNum < 5; ++ floorNum) {
		vector<vector<Cell*>> newFloor;
		for (int row = 0; row < 25; ++row) {
			vector<Cell*> newCellRow;
			for (int col = 0; col < 80; ++col) {
				if ((file >> noskipws >> c) && (c != '\n')) {
					string s;
					s.push_back(c);


					try {
						readInt = stoi(s);
					} catch (invalid_argument &ia) {
						readInt = -1;
					}

					if (readInt != -1) { //if "s" is a number;
						type = "Path";
						Cell* newCell = new Cell {row,col,type,"."};
						// Cell newCell{row, col, type, "."};
						if (readInt < 6) {
							++pCount;
							newCell->setPotion(s);
						} else {
							++gCount;
							newCell->setGold(s);
						}


						newCellRow.emplace_back(newCell);
						
					} else if ((s == "|")||(s == "-")||(s ==" ")) {//wall or empty spaces
						type = "NonPath";
						// Cell newCell{row, col, type, s};
						Cell* newCell = new Cell {row,col,type,s};
						newCellRow.emplace_back(newCell);

					} else if ((s == "H")||(s == "W")||(s == "E")||(s == "O")||(s == "M")||(s == "L")) {
						
						type = "Path";
						Cell* newCell = new Cell {row,col,type,"."};
						newCell->setEnemy(s);
	 					++eCount;

	 					newCellRow.emplace_back(newCell);

					} else if (s == "D") { //dragon
						type = "Path";
						Cell* newCell = new Cell {row,col,type,"."};
						newCell->setEnemy(s);
						newCellRow.emplace_back(newCell);

					} else if (s == "\\") {//stair
						type = "Stair";
						Cell* newCell = new Cell {row,col,type,s};
						newCell->setStair();
						newCellRow.emplace_back(newCell);

					} else if (s == "@") {//player character
						type = "Path";
						Cell* newCell = new Cell {row,col,type,"."};
						newCellRow.emplace_back(newCell);
						if (!curPlayer) {
							curPlayer = newCell;
						}
						players.emplace_back(newCell);

					} else { //+,#,.
						type = "Path";
						// Cell newCell{row, col, type, s};
						Cell* newCell = new Cell {row,col,type,s};
						newCellRow.emplace_back(newCell);
					}

				}

				
			}
			newFloor.emplace_back(newCellRow);



		}

		//when there is no pre-defined location in the src file
		if (players.size() < (floorNum + 1)) {
			players.emplace_back(nullptr);
		} 

		//add each floor's information
		floors.emplace_back(newFloor);
		potionCount.emplace_back(pCount);
		goldCount.emplace_back(gCount);
		enemyCount.emplace_back(eCount);

		//make them default value to store next level's info
		gCount = 0;
		eCount = 0;
		pCount = 0;
	}

	curFloor = new Floor{&floors[0],this,curPlayer, nullptr, curRace, goldCount[0],enemyCount[0],potionCount[0],curLevel+1, merchantAggro};

}

void Game::setPlayer(std::string type) {
	curRace = type;
	curFloor->setPlayer(type); //setPlayer in Floor

}


void Game::movePlayer(std::string direction) {
	if (!curFloor) {
		cout << "curFloor is not generated correctly!" << endl;
	}
	if (curFloor->movePlayer(direction)) {
		setLevel(curLevel+1);
	}
}

void Game::usePotion(std::string direction) {
	curFloor->usePotion(direction);
}

void Game::attack(std::string direction) {
	curFloor->attack(direction);
}

void Game::stop() {

	if(!stopNonPC) {
		stopNonPC = true;
	} else {
		stopNonPC = false;
	}
	curFloor->stop(stopNonPC);
}

void Game::restart() {
	merchantAggro = false;
	stopNonPC = false;
	delete curPlayer;
	curPlayer = nullptr;
	delete curStair;
	curStair = nullptr;
	floors.clear();
	enemyCount.clear();
	potionCount.clear();
	goldCount.clear();
	curLevel = 0;
	delete curFloor;
	curFloor = nullptr;
	players.clear();
	// if (players[curLevel]) {
	// 	curPlayer = players[curLevel];
	// }
	return;
		// curFloor = new Floor{&floors[0],this,curPlayer, goldCount[0],enemyCount[0],
		// 						potionCount[0], curLevel+1};
}


void Game::setLevel(int nextLevel) {

	if (nextLevel == 5) {
		cout << "if you want to play again, please press (r)" << endl;
		cout << "if you want to quit please press (q)" << endl;
		return;
	}
    merchantAggro = curFloor->getMerchantAggro();
	curLevel = nextLevel;

	if (curFloor) {
		curPlayer = curFloor->getCurPlayer();
		delete curFloor;

		if (players[curLevel]) {

			Cell* prevPlayer = curPlayer;
			curPlayer = players[curLevel];

			curPlayer->assignCharacter(prevPlayer->getCharacter());
			prevPlayer->clearCharacter();
		} else {
			curPlayer->setState("random", "no");
		}
		
		
		curFloor = new Floor{&floors[curLevel],this, nullptr, curPlayer,
									  curRace,
									  goldCount[curLevel],
									  enemyCount[curLevel],
									  potionCount[curLevel],
									  curLevel+1, merchantAggro};
	} else {
		curFloor = new Floor{&floors[curLevel],this, nullptr, curPlayer,
									  curRace,
									  goldCount[curLevel],
									  enemyCount[curLevel],
									  potionCount[curLevel],
									  curLevel+1, merchantAggro};
	}

	if(stopNonPC == true) {
		curFloor->stop(stopNonPC);
	}
}

void Game::setMerchantAggro(bool status) {
	merchantAggro = status;
}

int Game::getEnemyCount() {
	return enemyCount[curLevel];
}
int Game::getPotionCount() {
	return potionCount[curLevel];
}
int Game::getGoldCount() {
	return goldCount[curLevel];
}


ostream &operator<<(std::ostream &out, const Game &g) {
	out << *(g.curFloor);
	return out;
}






