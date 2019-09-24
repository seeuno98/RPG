#include <iostream>
#include <string>

#include "game.h"
#include "invalid.h"

using namespace std;

int main(int argc, char *argv[]) {

  Game g;
  bool pcCreated;
  string fname;

  //if the source file is not given
  if(argc < 2) { 
    g.init("map.txt");
    fname = "map.txt";
    cout << "Choose your character: (s)hade, (d)row, (v)ampire, (g)oblin, (t)roll" << endl;

  } else {
    g.init(argv[0]);
    fname = argv[0];
  }       

  string cmd;

 
  while (true) {
    cin >> cmd;
    try {
      if (pcCreated && cmd == "u") {
        string direction;
        cin >> direction;

        try {
          g.usePotion(direction);

        } catch (InvalidMove &im) {} //invalid direction

      } else if (pcCreated && cmd == "a") {
        string direction;
        cin >> direction; //r: row, c: column

        try {
          g.attack(direction);

        } catch (InvalidMove &im) {
          cout << im.getError() << endl;
        } //invalid direction

      } else if (pcCreated && cmd == "f") {
          g.stop();

      } else if (cmd == "q") {
          break;

      } else if ((cmd == "s")||(cmd == "d")||(cmd == "v")||(cmd == "g")||(cmd =="t")) {

        if (!pcCreated) {
          pcCreated = true;
          g.setPlayer(cmd);
        } else {
          cout << "Please press (r)estart if you want to choose another race: (s)hade, (d)row, (v)ampire, (g)oblin, (t)roll" << endl;
        }
          

      } else if (pcCreated && ((cmd == "no")||(cmd == "so")||(cmd == "ea")||(cmd == "we")||
                 (cmd == "ne")||(cmd == "nw")||(cmd == "se")||(cmd == "sw"))) {
          g.movePlayer(cmd);
    
      } else if (!pcCreated) {
        cout << "Choose your character first: (s)hade, (d)row, (v)ampire, (g)oblin, (t)roll" << endl;
      } else {
        cout << "It is invalid command. Please press (u)se potion, (a)ttack, (m)ove or (r)estart the game" << endl;
      }


      if (cmd == "r") {
          g.restart();
          pcCreated = false;
          g.init(fname);
          cout << "Choose your character: (s)hade, (d)row, (v)ampire, (g)oblin, (t)roll" << endl;

      } else {
         cout << g; //display the game
      }
    } catch (InvalidMove &im) { //when pc died
      cout << g;
      cout << "Please press (r)estart if you want to replay!" << endl;
      pcCreated = false;
      
    }
  }
  
}





