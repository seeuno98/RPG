#include "textdisplay.h"
#include "player.h"
using namespace std;


TextDisplay::TextDisplay(vector<vector<Cell*>> *floorDisplay) {

 	for(int row = 0; row < 25; ++row) {
		vector<string> newRow;
		for (int col = 0; col < 79; ++col) {
			
			string type = (*floorDisplay)[row][col]->getType();
			string pathValue = (*floorDisplay)[row][col]->getValue();	

			if ((type == "NonPath")||(type == "Path")||(type == "Stair")) {
				newRow.emplace_back(pathValue);
			} else if ((type == "RH")||(type == "BA")||(type == "BD")||
			   			(type == "PH")||(type == "WA")||(type == "WD")) {
				newRow.emplace_back("P"); 
			} else if (type != "NonPath") {
				newRow.emplace_back(type);
			}
		}
		theDisplay.emplace_back(newRow);
	}

}

bool TextDisplay::notify(Subject &whoNotified) {
	int rowNum = whoNotified.getState().row;
	int colNum = whoNotified.getState().col;
	string type = whoNotified.getType();
	string pathValue = whoNotified.getValue();

	if ((type == "Path")||(type == "Stair")) {
		theDisplay[rowNum][colNum] = pathValue;

	  //if type is one of the potions
	} else if ((type == "rh")||(type == "ba")||(type == "bd")||
			   (type == "ph")||(type == "wa")||(type == "wd")) {
		theDisplay[rowNum][colNum] = "P";

	  //if type is either enemy or player character
	} else if (type != "NonPath") {
		theDisplay[rowNum][colNum] = type;
	}
	return true;
}


void TextDisplay::update(Cell* newPlayer, int newLevel){
	curPlayer = newPlayer;
	curLevel = newLevel;
}

void TextDisplay::updateAction(string newAction) {
	action = newAction;
}

ostream &operator<<(std::ostream &out, const TextDisplay &td) {
	for (int row = 0; row < 25; ++row) {
		for (int col = 0; col < 79; ++col) {
			out << td.theDisplay[row][col];
		}
		out << endl;
	}



	if (td.curPlayer) {
		Player *PC = dynamic_cast<Player *>(td.curPlayer->getCharacter());
		string race;
		int curPlayerMaxHP = PC->getMaxHP();
		if (curPlayerMaxHP == 125) {
			race = "Shade";
		} else if (curPlayerMaxHP == 150) {
			race = "Drow";
		} else if (curPlayerMaxHP == 50) {
			race = "Vampire";
		} else if (curPlayerMaxHP == 120) {
			race = "Troll";
		} else {
			race = "Goblin";
		}

		int curPlayerHP = td.curPlayer->getCharacter()->getHP();
		if (curPlayerHP <= 0) {
			curPlayerHP = 0;
		}
		out << "Race: " << race << " Gold: " << PC->getGold() << "                  ";
		out << "Floor: " << (td.curLevel) << endl;
		out << "HP: " << curPlayerHP << endl;
		out << "Atk: " << td.curPlayer->getCharacter()->getATK() << endl;
		out << "Def: " << td.curPlayer->getCharacter()->getDEF() << endl;
		out << "Action: ";
		if (td.action.size() == 0) {
			out << "It is invalid command. Please press (u)se potion, ";
			out << "(a)ttack, (m)ove or (r)estart the game";
		} else {
			out << td.action;
		}
	}
	out << "." << endl;

	return out;
}




