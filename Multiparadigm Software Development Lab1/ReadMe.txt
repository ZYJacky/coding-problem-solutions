CSE 532 Fall 2021 Lab 1

Jackie Zhong (jackie.z@wustl.edu)

--------------------------------------------

Program Overview:
		
		The program "Play" takes a single argument that specifies a configuration file (see Appendix for expected format) of a play. The configuration file
	specifies the name of the play, the name of the players, and the names of the files that store each player's line (see Appendix for expected format). The ouput
	of the program is the complete play script (see Appendix for output format) if all the players' have their lines in ascending order; or if the lines are 
	misordered, the program will still provide the maximum number of lines that are in ascending order and report those that violates the ordering.

		"Play" takes the first command line argument as the config. file name and will ignore anything after it. If less than one argument is provided, or the
	program cannot open the file by the provided file name, it will print out corresponding message and return unique values, correspondingly for the two cases.

		If the program can open the file, it will try to take the first non-blank line of the file as the name of the play. If the file is completely empty
	or blank so the program cannot extract a play name, it will complain and return a unique value.

		If the program manages to get a play name, it will expect each of the following lines to contain at least two strings. The first one to be the character
	definition, and the second one to be the file for character lines. The main() function calls a helper function line_is_valid() to check for two things: the line
	contains at least two strings, and the second string which is expected to be the file name can be open (with std::ifstream). If the line is valid the player's name
	and its respective file are stored in two separate containers that the main function passed by reference to the helper one. Empty lines will be skipped and 
	invalid line will receive a complain and be skipped, but the program won't return. After the configuratioin file has been completely read, if the program does not
	find at least one valid player definition (i.e. line_is_valid() never returns true) it will complain and return a unique value;

		If the program finds at least one valid charater definition in the configuration file, it will construct a play object using the name extracted at 
	step 2, and constructs as many player objects as there are valid player definition (int valid_role_count) using the play object, player names, and the 
	input files stream. It will then call each players' enter() function, which lauches an internal thread that does read() and act() (wrapped together in a
	lambda function).

		The enter function of the player will first call read(), which iterates through the input file stream of the player (provided at construction and stored
	in the Player object), skipping empty lines/lines not starting with a line number/lines without at least one words after line number, extract the line number 
	and actual words, reform them as a structured line ([[Line No., Charater Name], Line]), and store them in a member container.

		After read(), the player will call act(), which takes a reference to the Play object (again, provided at construction). act() iterates through the
	structured lines that read() built at the previous step, and call the Play's object recite() method on each line (by passing an iterator to the line container).
	Each player races to let the Play recite their lines, and the synchronization is done in the following manner:

		1. A unique_lock<std::mutex> guards the entire scope of recite(). Player thread that holds the mutex checks the line number against a counter that the
	play object holds, which indicates the next line that it is expecting to recite (initialized to 1).

		2. If the couter is less than the line number, that means it is not the play's term yet. It first pushes its line number to a min-first priority queue
	that keeps track of the line numbers of currently pending thread. Then it increments a pending_thread variable of the Play object, and check to see if the 
	pending_thread equals the current acting players (a variable of Play object that is set at the construction initialized to valid_role_count).
	If so, that means the line numbers are misordered and it will set the counter to line number at top of the priority queue to make sure the program still proceeds.
	It itself is the thread that should proceed, it will pop its line number from the queue, decrement the pending thread, and proceeds to step 4. Else it will notify
	all the pending thread that counter has changed, and go to sleep. When it wakes up it will also do the same pop and decrement operation.

		3. If the counter is bigger than the line number, that means the lines are out of order. Since the program aims to always recite the line in ascending
	order, this line will be skipped by incrementing the player's iterator. Before it exits, it will check to see if this is the last line of this player (by checking
	if iterator equals containter.end() which is passed as a variable), if so and the rest of the active threads are all pending, it has to update the counter to the
	top of the priority_queue to avoid livelock and also update the number of acting role to signify it finishes. It then notifies all thread and simply return.

		4. If the counter is equal to the line number, the passed structured line will be printed out. If the player that speaks is different from the previous one
	that speak (which the Play object keeps track using a variable). It also makes a blank line and prints the new Player's name. Again, if the current player is finished,
	to avoid potential livelock in a misordered scenario, it will check to see if all the rest of the acting roles are waiting, if so it updates the counter to the 
	top of the priority queue. Otherwise it simply increments the counter. It then notifies all and return.

		For more reasoning behind the design, see the following sections. The main thread calls each player's exit() method that joins with their internal thread
	doing enter() if it is still joinable. Then the main thread return a unique value indicating SUCCESS that marks the program finishes normally. 
	
		
----------------------------------------------

Wrapper Facades Pattern:

	thread():

		This wrapper provided by the <thread> library combines low-level function (pthread_create(), pthread_exit(), etc.) into a thread object. This simplifies
	the management of threads from calling different pthread functions to playing with a concrete object which is more maintainable and hides many low-level details
	from the program.
	

	Play class and its member functions:

		The play class wraps all the data that represents the play (play name, speaking character, etc.) together and therefore increases the modularity so varaibles 
	are not all over the main function as well as privacy since private data of a class are secured and has less chance of getting modified from outside. In addition, 
	the member function included in the play object also increases the modularity as they are directly related to the object so can be grouped together. They will 
	also have direct access to the private member. The play object is also managing the synchronization within it, while not worrying about thread spawning and joining,
	which is a good example of separation for responsibility.


	Player class and its member functions: 

		The player class further separates responsibility from the play class by grouping methods and variables that are specific to the player, e.g. reading from
	the input files, spawning its internal thread, etc. Such wrapper of player function and threads again avoids having the main thread to do all the work that can 
	result in lengthy code, a lot of variables, etc. By managing its own thread (spawning and joining at the end) it separates that responsibility from the main function
	and Play object so they don't need to worry about it.
	  

		
	  
		
-----------------------------------------------

Design Choices and observations:

	Storage of play line:

		As required, a play line consist of the character name, the line number, and the actual line. To store this in a container in the Play object, a pair of
	a pair and a string is used and declared as follow:

		typedef std::pair<std::pair<unsigned int, std::string>, std::string> structured_line; // [[Line No., Charater Name], Line]

	By doing so, the program uses standard data structure and avoids designing a custom one, which means it can simply uses the standard library
	funtion when necessary to simplify the operation.  
	
	
	Concurrency design:

		In this lab, multiple threads will be racing for the recite() methods. More specifically, they are trying to compare their line number with the counter
	variable and increment it, and wait if it is not their term yet. Based on this scheme, I design to lock the entire recite() scope with a unique_lock<std::mutex>
	(since condition variable is used) as the first operation of the method is to compare line number with counter and the last operation in the method is to increment 
	the counter and notify all waiting threads. Since the invariant is that from the thread that is currently doing recite(), the counter variable cannot change during its operation. 
	Otherwise, consider this race condition: 

		1. thread B comes and compare its counter and sees that it is not its term yet
		2. thread A comes and is the one that should recite, so it prints out its recitation and increment the counter
		3. thread A notify_all(), but no one is currently waiting on the condition variable
		4. thread B starts to wait on the condition variable, but thread A has finished so it does not get notified

		Though it is possible to lock where the counter variable is accessed, the only operation inbetween comparing the counter variable and incrementing it
	is the print method (only one thread will print at a time so there is no race to the standard output stream), so it may no be worth to acquire the lock when
	the function starts to compare counter variable, releases it after it exits block from wait(), and acquire it immediately again after printint out. Besides,
	the other threads can't proceed anyway before the current thread increments the current counter variable. Considering the overhead with repeatedlly acquiring
	and releasing lock, the very small amount work that can be parallelized, and further issues with misordered lines, I think it is not worth to do more precise
	locking in this case. So instead the whole recite() scope is lock, and relies on the condition variable to release the lock and block with wait(), and at 
	some point proceed to finish the job and exit the scope.

	

	Misordered cases:

		In an ordered case, each thread will either wait for its term, or one thread is guaranteed to have the counter equal its line number and can therefore 
	increment the counter and notify all other threads to proceed. In an misordered case, however, the situation is much more complicated. Consider the following case:

		1. The counter defaults to start at 1.
		2. All players' script is misordered, and none of their first line start with 1.
		3. They all block by the condition variable wait(), and the program livelock.

		This scenario can not only happen at the beginning, but can also happen in the middle. To deal with this potential livelock, one solution is to have a
	monitor thread checks at a time interval how many threads are blocked (by letting thread increment a variable before they wait()) and perform action is all
	threads are blocked. The issue with this is it is hard to tune the time interval and synchronize the monitor thread with the rest of the threads. Instead,
	this design let the player thread themselves to get out of the livelock situation. All threads when entering the wait phase (counter < line number) will push its
	line number onto a min-first priority queue, so that in the misordered scenario described just now, we now which line we should jump to. If a thread sees that
	all threads including itself is in the wait phase (by checking a variable that each thread increments and decrements entering and leaving the phase), it will
	change the counter to directly jump to the smallest current line (top of min heap), and then notify_all to make sure the program proceeds.

	
	Another case is the counter is larger than the line number, per the requirement of the lab to at least print out in ascending order, this case is handled by simply 
	print out a warning and skip it by incrementing the counter. However, this is an edge case such that if this is the last line of the player, if the thread simply
	prints out the warning and exit, if all the rest of threads are blocked, the program livelock again. Again, to avoid this scenario we have to make sure the program
	increases the counter accordingly in every possible path. Therefore additional condition checking is done before the thread exit the recite method in the bigger case (counter > line number)
	and equal case (counter == line number) to see if this is the last line of the player. If so and all the rest of the threads that haven't finished acting are all blocked,
	need to again let counter be the top element on the priority queue to make sure the program proceeds. Notice that in an ordered scenario the top of the priority queue
	is always going to be the next line number as it is min-first. 

	This methodology will generate a consistent output (for part of the lines that are in order) for misorded lines and is guarenteed to maximize
	the ascending lines, which means it will start comparing the first line of each character, take the smallest, move the line iterator for that
	character, and repeatedlly perform this opeartion (with min-first heap) to assure that maximum ascending sequnce across the files are achieved.

		

------------------------------------------------

Operation Instructions:

	To unpack: Run "tar -xvf Jackie_532_Lab1.tar.gz" 

	To make: run "make" in the command line. Flags of "-pthread", "std=c++11", "-Wall" are already included. If more flags are needed, modify Makefile or copy:

			      g++ -o Play main.cpp Play.cpp Player.cpp -Wall -std=c++11 -pthread

		 and add more flags. Run "make clean" to remove the executable.

	To run: run "./Play <config file name>" in the terminal; if the config file is not under the same directory of Play needs to specify the path
		
	            e.g. "./Play CONFIG_hamlet1.1.txt"


	
	Test Cases: Two test cases are included under the directory. To avoid confusion, no subdirectories are made. Instead the CONFIGURATION FILE that will
                    be used to run the Play program will start with "CONFIG" to make them more recognizable. The output file will starts with "OUTPUT", and the 
                    original file will start with "ORIGINAL".

                    1. test case named "CONFIG_ill_hamlet_2.2.txt" and related files. Multiple ill-formed lines are inserted manually, including but not limited to: 
			invalid file name in config file, invalid character in config file, blank lines in config file and script, line with only line number in script, 
			line with spacing in front, line does not have a line number in script...... However, the valid lines are correctly ordered.

		       The output is shown in "OUTPUT_ill_hamlet_2.2.txt".

		    2. Misordered lines 

			Player script files that are filled with misordred lines randomly generated with the extra_credit program is included in the directory 
		       specified in "CONFIG_hamlet_1.1.txt". Actually multiple runs with different sets of misordered files are done, each being executed
			for about 30 times to make sure the output is consistent and there is no race condition or livelock. One of the result is shown in  	
			"OUTPUT_hamlet_1.1.txt". For the purpose of illustrating the lines are printed out in ascending order and the misordered lines are
			reported, in this output file the line number appears before each line.		

------------------------------------------------

Extra Credit Section:

	To unpack: Run "tar -xvf Jackie_532_Lab1_ExtraCredit.tar.gz" 

 	To build: issue "make" in the command line. Flags of "-Wall" and "-std=c++11" are already included. If more flags are needed, modify Makefile or copy:

			      g++ -o break_script break_script.cpp -Wall -std=c++11 

		  and add more flags. Run "make clean" to remove the executable.

	To run: run "./break_script <input file> <config file> <Play Name can have white spaces>" in the terminal; if the config file is not under the same 
			directory of Play needs to specify the path. Adding the "SCRAMBLE" option will create scripts whose lines are randomly ordered.
		
	            e.g. "./break_script SCRAMBLE ORIGINAL_hamlet_1.1.txt CONFIG_hamlet_1.1_config.txt Hamlet Act i Scene 1"





	This program takes a original script file with the following format requirement:
	
		1. Except for the first case, if and only if there is a blank line we see a character definition in the next line.
                   This case is very IMPORTANT! Since otherwise there is no way of distinguish a ONE-WORD-LINE (which presents in the test case)
                   from a character definition. In addition, this also agrees with the output produced by Lab 0 program

	        2. Empty lines and blank lines will be skipped.





		As shown in the run instruction, the program will first check if there is less than 4 arguments (argc < 4), if not it would complain and return a unique value.
	The program then checks for a SCRAMBLE option and will set a flag accordingly. It then shifts the index and extracts the script file name, output file name, and 
	play name from argv and stores them in respective strings.

		The program then checks if the script file is openable (with ifstream), and will complain and return if not. It then check to see if a config output
	file is available, and will try to create one if not. It will complain and return a unique value if cannot create one.

		Then the program starts to iterate the script file. When it sees a blank line (default to be true for the initial case), it will take the next
	line as a character name, create an file for it if not available (complain and return an unique value if creation failed). It will use a map and vector to 
	keep track of all the seen characters and add them into the containers along the way. It also outputs to config file the information. (See Appendix for format)
		
		When the previous line is not blank, the program will expect that next line to be a script line. It keep tracks of the current speaking characer,
	which is used with map to find the correspoinding container for that character's line and the line will then be inserted into the container.

		After all lines are put in containers separated by character, if the SCRAMBLE flag is set, the program will iterate through each character's container
	and randomize the lines (see design choice for details). Regardless of the SCRAMBLE option, as the last step the program inserts lines from all containers
	into their corresponding script files that is available/created at step 3.

		At the end, the program retursn 0 to indicate SUCCESS.




	Design Choice:
 		
		The randomization is done by calling std::swap() between each element and another random element whose indices is determined using "rand() % size".
	The random number generator is seeded with time (srand(time(NULL));) for each container. This method is inspired by possible implementation of std::shuffle()
	(which is available for c++11 and onward https://en.cppreference.com/w/cpp/algorithm/random_shuffle), and should have very similar effect.
			


	Test Case:
		
			Another hamlet act is taken at http://www.literaturepage.com/read/shakespeare_hamlet-2.html and be broken up with the program.
	The resulting config file (config.txt) and character files with SCRAMBLE option enalbed are included in the directory. In fact, about 10 different
	run of the SCRAMBLE option are done to get 10 different misordered sets of character files. And testing them through with the Play program helped me
	tested out edge cases in misordered scenarios (see design choice for Play program above) that resulted in livelock. In the end the main program is
	able to print out lines in ascending orders and report those that are misordered.

	           

------------------------------------------------------

Appendix: 

	Configuration file format:

		Name of the Play
		<Character Name 1> <Character FileName> (everything followed ignored)
		<Character Name 2> <Character FileName> (everything followed ignored)
		(empty line ignored)
		<Character Name 3> (no file provided, ignored)
		<Character Name 4> <invalid file name>  (invalid file, ignored)




	Character line file format:

		<Line No>  Actual Line
		(blank line ignored)
		Actual Line (no line number, ignored)
		<Line No>   (no actual line, ignored)




	Program return value (see utility.h):

   		0: SUCCESS; successful termination of the program
    		1: INVALID_ARG; does not provide valid argument (i.e. config file name)
    		2: NO_FILE; cannot open by the provided config file name 
    		3: NO_PLAY_NAME; configuration file does not provide a play name
		4: NO_VALID_ROLE; configuration file does not provide at least one valid character




	Output format:

	Play Name

	<Character 1>.
	Line 1
	Line 2
	Line 3

	<Character 2>.
	Line 1
	Line 2

	.
	.
	.
	.





	