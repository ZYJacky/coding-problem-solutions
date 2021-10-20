/*
 *  break_script.cpp
 *
 *  Break a scipt file into multiple ones each containing
 *  a character's lines. Will also generate a config file.
 *  
 *  V1.1: a SCRAMBLE option is enabled to randomly order
 *  each character's lines.
 *
 *  Expected input format: a script in which the name of the speaking
 *  character is after an empty line (except for the first occurence)
 *  followed by its lines (with no empty line until the speaking character switches).
 *
 *  Date Created: 9/19/21
 *  Last Modified: 10/9/21
 */

#define MIN_ARGS 4

enum CMD_ARGS
{
    PROGRAM_NAME,
    SCRAMBLE_OPTION = 1,
    SCRIPT_NAME = 1,
    OUTPUT_FILE,
    PLAY_NAME
};

enum RET_VALUES
{
    SUCCESS,
    INVALID_ARGS,
    INVALID_SCRIPT,
    NO_OUTPUT_FILE,
    ROLEFILE_CREATE_FAIL
};

#include <iostream> // for cout, endl
#include <fstream>  // for ifstream
#include <stdlib.h> // for system, rand    
#include <map>      
#include <vector>
#include <sstream>  // for istringstream
#include <cstring>
#include <time.h> // for time

// helper method
int check_or_create_output_file(std::ofstream& ofs, std::string filename, int FAIL_RET_VAL);

int main(int argc, char* argv[])
{   
    bool scramble_mode = false;
    int num_options = 0;


    /* * * * * * * * * * * * * * * * * 
     *  Verify arguments and options *
     * * * * * * * * * * * * * * * * */      
    // verify command line arguments
    if(argc < MIN_ARGS)
    {
        std::cout << "usage: " << argv[PROGRAM_NAME] << " (SCRAMBLE) <script name> <output config file name> <all the rest as play name>" << std::endl;
        return INVALID_ARGS;
    }

    // check SCRAMBLE option
    if(strcmp(argv[SCRAMBLE_OPTION], "SCRAMBLE") == 0)
    {   
        scramble_mode = true;
        ++num_options;
        if(argc < MIN_ARGS + num_options)  // SRAMBLE specified but not enough arguments
        {
            std::cout << "usage: " << argv[PROGRAM_NAME] << " (SCRAMBLE) <script name> <output config file name> <all the rest as play name>" << std::endl;
            return INVALID_ARGS;
        }
    }


    /* * * * * * * * * * * * * * * * * *
     * Build up information and stream *
     * * * * * * * * * * * * * * * * * */  

    // extract and store arguments into strings
    std::string ScriptName = argv[SCRIPT_NAME + num_options], OutputFile = argv[OUTPUT_FILE + num_options], PlayName = argv[PLAY_NAME + num_options]; 
    for(int i = PLAY_NAME + num_options + 1; i < argc; ++i) PlayName = PlayName + " " + argv[i];
    
    // try opening the script
    std::ifstream ifs (ScriptName);
    if(!ifs)
    {
        std::cout << "Script file cannot be open" << std::endl;
        return INVALID_SCRIPT;
    }   

    // try finding a config file; create one if not found
    std::ofstream ofs (OutputFile);
    int ret_value_1 = check_or_create_output_file(ofs, OutputFile, NO_OUTPUT_FILE);
    if(ret_value_1 != SUCCESS) return ret_value_1;

    ofs << PlayName << std::endl;   // insert playname to config file


    /* * * * * * * * * * * * * * * * * *
     * Iterate through script and fill *
     * * * * * * * * * * * * * * * * * */  

    std::string speaking_role;   // keep track of who is speaking
    int count_of_role = 0, count_of_line = 1;
    std::map<std::string, int> roleMap; // map player name to a location in lines container
    std::vector<std::ofstream> roleFiles;  
    std::vector<std::vector<std::string>> lines;// store the lines that may be reordered
    bool prev_line_is_blank = true;  // used to keep track of role switch

    while(!ifs.eof())
    {
        std::string tmp, firstword, secondword;
        std::getline(ifs, tmp);
        if(tmp.length() == 0)
        {   
            prev_line_is_blank = true;
            continue; // Skip empty line
        }; 

        std::istringstream iss (tmp);
        if(!(iss >> firstword)) // Skip blank line
        {   
            prev_line_is_blank = true;
            continue; 
        }

        if(prev_line_is_blank)   // we get a character definition if previous line is blank
        {                          
            firstword.pop_back();  
            speaking_role = firstword;
            if(roleMap.find(speaking_role) == roleMap.end())   // first appearance of the character, need to create output file (if not provided)
            {   
                std::ofstream ofs_role = std::ofstream(speaking_role + ".txt");

                std::string file_name = speaking_role + ".txt";
                int ret_value_2 = check_or_create_output_file(ofs_role, file_name, ROLEFILE_CREATE_FAIL);
                if(ret_value_2 != SUCCESS) return ret_value_2;
                
                roleMap[speaking_role] = count_of_role++;    // use map so later we know where to insert the lines if a character reappear
                roleFiles.push_back(std::move(ofs_role));    // store the filestream
                lines.push_back({});                        // allocate a space in lines container
                ofs << speaking_role << " " << speaking_role << ".txt" << std::endl;  // insert to config file
            }
            prev_line_is_blank = false;
        }
        else        // a script line, insert it into the corresponding file stream
        {
            tmp = std::to_string(count_of_line) + " " + tmp;  // build character line
            ++count_of_line;
            lines[roleMap[speaking_role]].push_back(tmp);
            prev_line_is_blank = false;
        }
    }

    // randomly reorder if SCRAMBLE option is specified
    if(scramble_mode)
    {
        for(int i = 0; i < count_of_role; ++i)
        {   
            srand(time(NULL));
            int cur_size = lines[i].size();
            for(int j = 0; j < cur_size; ++j)
            {
                std::swap(lines[i][j], lines[i][rand() % cur_size]);
            }
        }
    }

    // insert into corresponding file after finished
    for(int i = 0; i < count_of_role; ++i)
    {   
        int cur_size = lines[i].size();
        for(int j = 0; j < cur_size; ++j)
        {
                roleFiles[i] << lines[i][j] << std::endl;
        }
    }
    
    return SUCCESS;
}


// helper methods

// check to see if a file exist by its name, and create one if not
int check_or_create_output_file(std::ofstream& ofs, std::string filename, int FAIL_RET_VAL)
{
    if(!(ofs))                               
    {
        system(("touch " + filename).c_str());
        ofs = std::ofstream (filename);      
        if(!(ofs))
        {
            std::cout << "Fail to create output file." << std::endl;
            return FAIL_RET_VAL;
        }
    }
    return SUCCESS;
}