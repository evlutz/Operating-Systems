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

- **$ g++ -o -pthread life.cpp life.h AbstractLife.h -o game_of_life**

## Usage:
###### $ ./game_of_life [*# Threads*] [*File name*] [*# of generations*]   *Optional:*{Print generations?} *Optional:*{Wait for input}

**Configurable Command Line Arguments**
- Accepts 3-5 command line arguments for customization:
    - Number of threads
    - File containing Generation 0
    - Number of generations to play
    - Option to print each generation (default is *False*)
    - Option for keyboard input before proceeding (default is *False*)

#### Game Boards

Game boards can be created by ant user, it consists of a grid of 1's and 0's which cannot be more than 40 rows or columns.
1's represent alive cells and 0's represent deadcells.

To create a gameboard simply create a text file and create or edit the pre installed grids under the `Starting_Boards` directory
           
## Output:
<p>
The program will provide statistics for each file, including the number of bad files, directories, regular files, special files, total bytes used, and more.
</p>


Ensure that the pthread library is available.


