# Distributed Game of Life

**Description:**

This distributed version of John Conway's Game of Life, utilizing message passing, semaphores, and synchronization. Depending the size of the board it is divided up amongst a user predefined number of threads, which communicate with the main thread through message passing, allowing synchronization between each generation. With semaphores protecting shared recorces and processes when communicating between threads.

**Features:**

- Distributed Computation allows multiple threads to parallelize the evaluation of cells for each generation.

- Implements John Conway’s Game of Life *[learn more](https://www.nytimes.com/2020/12/28/science/math-conway-game-of-life.html).*

- Message passing implements synchronization of threads between each generation

- Semaphores protect shared data while sending messages between threads.

- File Input for initial generation


## Example Run:
##### $ ./game_of_life 10 glider_gun.txt 120 y y
![Alt Text](LifeRun.gif)

## Compilation:

To compile the `game_of_life` with header files and abstracts, use the **makefile** or use the following command:<br>
- **make clean**<br>
- **make**<br>

***or***

- **`$ g++ -o -pthread life.cpp life.h AbstractLife.h -o game_of_life`**

## Usage:
###### `$ ./game_of_life [*# Threads*] [*File name*] [*# of generations*]   *Optional:*{Print generations?} *Optional:*{Wait for input}`

**Configurable Command Line Arguments**
- Accepts 3-5 command line arguments for customization:
    - Number of threads
    - File containing Generation 0
    - Number of generations to play
    - Option to print each generation (default is *False*)
    - Option for keyboard input before proceeding (default is *False*)

**Game Boards**

Game boards can be created by ant user, it consists of a grid of 1's and 0's which cannot be more than 40 rows or columns.
1's represent alive cells and 0's represent deadcells.

To create a gameboard simply create a text file and create or edit the pre installed grids under the `Starting_Boards` directory
           
## Output:

Depending on the User-set output congigurations `Print boards` & `Wait for key` determines what the output of the program will look like

`Wait for key` - Wait for user input before continuing to the next gerneration *Default=False* <br>
`Print boards` - Print generations *Default=False*

Regardless of these configurations if the program ran correctly `Number of set generations reached`, <br> or <br> `Game Stopped: Repetitive Generations` depending if the generations become redundant before they reach the pre-set number of generations is reached.

## Message Types

Message types used to synchronize threads and decide game state:

`RANGE`: Sent from main thread to worker threads to assign a range of rows to compute the new generation for. <br>
`GO`: Sent from main thread to worker threads to start computing the new generation.<br>
`GENDONE`: Sent from worker threads to main thread to indicate that the new generation has been computed.<br>
`ALLDONE`: Sent from worker threads to main, indicates pre-set number of generations have been reached<br>
`STOP`: Sent from main thread to worker threads to terminate the game, used if a redundant generation is reached.<br>


### Notes
- Ensure that the pthread library is available.
- As grid nears 40x40 malloc errors become more likely

