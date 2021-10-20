/*
 *   Player.cpp
 *
 *   Implement member functions declared in Player.h
 *
 *   Created: 10/6/21
 *   Last edited: 10/8/21
 */

#include "Player.h"

#include <sstream> // for istringstream

Player::Player(Play& play, const std::string& str, std::ifstream& ifs)
                : play(play), PlayerName(str), in_file(ifs)
{}

// read the input file given at construction and fill the structured lines container
void Player::read(void)
{   
    while(!in_file.eof())
    {   
        std::string line, words;
        unsigned int LineNo;

        std::getline(in_file, line);
        if(line.length() == 0) continue;  // skip empty line

        std::istringstream iss(line);
        if(!(iss >> LineNo)) continue;   // can't extract a line number

        if(!(iss >> words)) continue;  // nothing after a line number

        // get and concatenate rest of the string
        std::string unread = iss.eof() ? "" : iss.str().substr( iss.tellg() );
        words += unread;
        
        lines.push_back(std::make_pair(std::make_pair(LineNo, PlayerName), words));
    }                                                                           
}

// ask the play object to recite the player's line
void Player::act(void)
{
    std::vector<structured_line>::iterator it = lines.begin();    // iterator at the start of the lines
    std::vector<structured_line>::iterator end = lines.end();    // iterator at the start of the lines
    while(it != end)
    {
        play.recite(it, end);
    }
}

// lauch the player's thread that does its read and act
void Player::enter(void)
{
    t = std::move(std::thread([this]
                    {
                        this->read();
                        this->act();
                    }));
}

// try to join witht the thread, marking the exit of the player
void Player::exit(void)
{
    if(t.joinable()) t.join();
}