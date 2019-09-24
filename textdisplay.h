#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H
#include <iostream>
#include <vector>
#include "observer.h"
#include <string>
#include "subject.h"
#include "cell.h"

class TextDisplay: public Observer {
  std::vector<std::vector<std::string>> theDisplay;
  Cell* curPlayer = nullptr;
  Cell* curEnemy = nullptr;
  std::string curPotion = "";
  std::string moveDirection = "";
  int curLevel;
  int prevHealth;
  std::string action;
  // const int gridSize;
 public:

  //create empty text display board including only Walls, Passageways, Stairs
  TextDisplay(std::vector<std::vector<Cell*>> *floorDisplay);

  bool notify(Subject &whoNotified) override;
  // void update(Cell* newPlayer, Cell* newEnemy, std::string newPotion, std::string newDirection, int newLevel);
  void update(Cell* newPlayer, int newLevel);

  void updateAction(std::string newAction);


  friend std::ostream &operator<<(std::ostream &out, const TextDisplay &td);

};
#endif





