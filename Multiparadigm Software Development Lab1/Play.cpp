/*
 *   Play.cpp
 *   
 *   Implement member functions declared in Play.h
 *
 *   Created: 9/16/21
 *   Last edited: 10/10/21
 */

#include "Play.h"

#include <iostream> // for cout, endl

Play::Play(std::string PlayName, unsigned int valid_role_count)
            : PlayName(PlayName), counter(1), pending_thread(0), acting_role(valid_role_count)
{} 

/* Takes a reference to the player's line, check the counter, and block if it is not its turn yet
 * using wait(). Otherwise print out the line Additional features that deal with unordered player lines are implemented. See
 * lines are implemented. See ReadMe.txt for details.
 */
void Play::recite(std::vector<structured_line>::iterator& it, std::vector<structured_line>::iterator& end)
{   
    std::unique_lock<std::mutex> lk(counter_lk);
    unsigned int lineNo = (*it).first.first;

    if(counter < lineNo)
    {   
        pq.push(lineNo);
        
        ++pending_thread;

        // all other threads are waiting, need to update counter to avoid livelock
        if(pending_thread == acting_role) counter = pq.top();

        // this is not the thread that should proceed 
        if(counter != lineNo)
        {   
            cv_counter.notify_all();
            cv_counter.wait(lk, [this, lineNo]{return counter == lineNo;});
        }
        
        pq.pop();
        --pending_thread;
    }
    else if(counter > lineNo)
    {   
        std::cerr << lineNo << " Error: lines are out of order" << std::endl;
        ++it;

        // in an unordered scenario, to avoid livelock if the current role finished all his
        // lines, it still may have to update the counter if other threads are in wait()
        // to avoid livelock
        if(it == end && --acting_role > 0 && pending_thread == acting_role)  counter = pq.top();

        cv_counter.notify_all();
        return;
    }

    // when counter == current line number
    if((*it).first.second != current_speaking)  // switch speaking character
    {
        current_speaking = (*it).first.second;
        std::cout << std::endl << current_speaking << "." << std::endl;
    }
    std::cout << (*it).second << std::endl;
    ++it;

    // in an unordered scenario, to avoid livelock if the current role finished all his
    // lines, it still may have to update the counter if other threads are in wait()
    // to avoid livelock
    if(it == end && --acting_role > 0 && pending_thread == acting_role)  counter = pq.top();
    else ++counter;

    cv_counter.notify_all();
}


