/*
 *  main.cpp
 *
 *  The entry point of the program "Play". It checks the validity of command line arguments
 *  and input files, extract all the necessary information (names of players, play name, etc.),
 *  and construct the Play and Player objects, and recite all the lines by calling corresponding
 *  member functions.
 *
 *  Created: 9/16/21
 *  Last edited: 10/10/21
 */


// local hearder files
#include "utility.h"
#include "Play.h"
#include "Player.h"


// standard library
#include <iostream> // for cout, endl
#include <fstream> // for ifstream
#include <sstream> // for istringstream

// helper functions. See conmments at function definition below for details
bool line_is_valid(std::string& line, std::vector<std::string>& RoleNames, std::vector<std::ifstream>& RoleFiles);


/*
 *  The entry point for the program. Process config file, constructs the player and play objects,
 *  and recite the lines by calling the object's member functions.
 *
 *  Input: config file name as command line argument
 *  Return value: program status specified in utility.h
 */
int main(int argc, char* argv[])
{   
    /******************************
     * CHECKING AND PREPROCESSING * 
     ******************************/

    // check input arguments; all arguments after the second (config file name) will be ignored
    if(argc < ARGS_NEEDED)
    {
        std::cout << "usage: " << argv[PROGRAM_NAME] << " <configuration file name>" << std::endl;
        return INVALID_ARG;
    }

    // open config file provided and check validity
    std::ifstream ifs (argv[FILE_NAME]);
    if(!ifs) 
    {
        std::cout << "Cannot open configuration file." << std::endl;
        return NO_FILE;
    }

    // Extract PlayName from Config file
    std::string PlayName;
    while(PlayName.length() == 0) 
    {   
        if(ifs.eof())      // completely empty file
        {
            std::cout << "File content is invalid. No play name specified." << std::endl;
            return NO_PLAY_NAME;
        }
        std::getline(ifs, PlayName);    // skip blank lines (if any) and get the first line supposed to be Play name

        std::string tmp;
        std::istringstream iss (PlayName);
        if(!(iss >> tmp)) PlayName.clear();     // DESIGN CHOICE: just in case a line contains entirely of white spaces; by design this will
    }                                           // not be tolerated. 
    
    // process each line in config file
    std::vector<std::string> RoleNames;
    std::vector<std::string> RoleFileNames;
    std::vector<std::ifstream> RoleFiles;
    unsigned int valid_role_count = 0;

    while(!ifs.eof())
    {   
        std::string config_line;
        std::getline(ifs, config_line);
        if(config_line.length() == 0) continue;       // ignore empty lines

        if(!line_is_valid(config_line, RoleNames, RoleFiles))   // ignore invalid lines
        {
            std::cout << "Invalid line detected: " << config_line << std::endl;
            continue;
        }
        ++valid_role_count;
    }

    if(valid_role_count == 0)    // file does not have at least one valid character definition 
    {
        std::cout << "File content is invalid. Should have at least one valid character." << std::endl;
        return NO_VALID_ROLE;
    } 

    /****************************
     * ENTERING EXECUTION PHASE * 
     ****************************/

    // construct Play object and Player object
    Play play(PlayName, valid_role_count);  
    
    std::vector<Player> players;
    for(unsigned int i = 0; i < valid_role_count; ++i)
    {    
        players.push_back(Player(play, RoleNames[i], RoleFiles[i])); 
    }
    
    std::cout << std::endl <<  PlayName << std::endl;

    // player enter, spawn threads that call recite
    for(unsigned int i = 0; i < valid_role_count; ++i)
    {   
        players[i].enter();
    }

    // player exit, join with their internal threads
    for(unsigned int i = 0; i < valid_role_count; ++i)
    {   
        players[i].exit();
    }

    return SUCCESS;
}

/****************** 
 * HELPER METHODS *
 ******************/

/*
 *   Helper function that test whether a line is valid, which should consist of two strings, one for role name and one for files
 *   Input: string to be tested, reference to two vectors that will store the result if it is valid
 *   Output: a bool value of whether the line is valid
 */
bool line_is_valid(std::string& line, std::vector<std::string>& RoleNames, std::vector<std::ifstream>& RoleFiles)
{
    std::string RoleName, FileName;

    std::istringstream iss (line);
    
    if(!(iss >> RoleName)) return false;   // in case of a non-empty but blank lines (only white spaces)
    
    if(!(iss >> FileName)) return false;    // no file name provided

    std::ifstream ifs = std::ifstream(FileName);
    if(!ifs) return false;                // cannot open file by the provided name

    RoleNames.push_back(RoleName);   
    RoleFiles.push_back(std::move(ifs));    
                                          
    return true;
}
