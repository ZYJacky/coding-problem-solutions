/*
 *   Player.h
 *
 *   Declaration of class Player, which represents a player in a play script, 
 *   including its name, the lines, and related action/methods  
 *
 *   Created: 10/6/21
 *   Last edited: 10/8/21
 */

 
#ifndef PLAYER_H
#define PLAYER_H

#include "Play.h"

#include <utility> // for pair
#include <vector>
#include <fstream> // for ifstream
#include <thread>

#ifndef STRUCTURED_LINE 
#define STRUCTURED_LINE
typedef std::pair<std::pair<unsigned int, std::string>, std::string> structured_line; // [[Line No., Charater Name], Line]
#endif // STRUCTURED_LINE

class Player
{
    private:
        std::vector<structured_line> lines;    // storage of structured lines

        Play& play;
        const std::string& PlayerName;
        std::ifstream& in_file;

        std::thread t;  // TODO: default construct by constructer? per lab instructions

    public:
        Player(Play&, const std::string&, std::ifstream&); 

        void read(void); // fill structureded lines container
        void act(void); // make call to Play's recite
        void enter(void); // lauch worker thread
        void exit(void); // try to join with thread
};

#endif  // define PLAYER_H