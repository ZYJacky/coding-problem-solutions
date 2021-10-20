/*
 *   Play.h
 *
 *   Declaration of class Play, which represents a play script, including the name, characters, and lines  
 *
 *   Created: 9/16/21
 *   Last edited: 10/9/21
 */

#ifndef PLAY_H
#define PLAY_H

// libraries for data structures and general utilities
#include <utility> // for pair
#include <vector>
#include <ostream> // ostream
#include <string>
#include <queue>

// libraries for concurrency design
#include <mutex>
#include <condition_variable>

#ifndef STRUCTURED_LINE 
#define STRUCTURED_LINE
typedef std::pair<std::pair<unsigned int, std::string>, std::string> structured_line; // [[Line No., Charater Name], Line]
#endif

class Play
{   
    public:
        Play(std::string PlayName, unsigned int);                     // consturctor that sets the PlayName

        void recite(std::vector<structured_line>::iterator&, std::vector<structured_line>::iterator&);

    private:
        std::string PlayName;
        unsigned int counter;
        std::string current_speaking;

        // synchronization features
        std::condition_variable cv_counter;
        std::mutex counter_lk; 

        // features used to deal with misordered lines
        unsigned int pending_thread;       
        unsigned int acting_role;
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
};                                
#endif  // define PLAY_H