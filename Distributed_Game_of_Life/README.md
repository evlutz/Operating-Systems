# Distributed Game of Life

**Description:**

This distributed version of John Conway's Game of Life, utilizing message passing, semaphores, and synchronization. Depending the size of the board it is divided up amongst a user predefined number of threads, which communicate with the main thread through message passing, allowing synchronization between each generation. With semaphores protecting shared recorces and processes when communicating between threads.

**Features:**

- Distributed Computation allows multiple threads to parallelize the evaluation of cells for each generation.

- Implements John Conway’s Game of Life *[learn more](https://www.nytimes.com/2020/12/28/science/math-conway-game-of-life.html).*

- Message passing implements synchronization of threads between each generation

- Semaphores protect shared data while sending messages between threads.

- File Input for initial generation



**Configurable via Command Line Arguments**
- Accepts 3-5 command line arguments for customization:
    - Number of threads
    - File containing Generation 0
    - Number of generations to play
    - Option to print each generation (default is *False*)
    - Option for keyboard input before proceeding (default is *False*)

## Example Run:
##### $ ./game_of_life 10 glider_gun.txt 100 y y
![Alt Text](LifeRun.gif)

## Compilation:

To compile the program with the multi-threaded architecture, use the **makefile** or use the following command:<br>
**make clean**<br>
**make**<br>

***or***

**$ g++ -o proj4 proj4.cpp -lpthread**

## Usage:

### Serial Architecture:

In the serial architecture version, the program reads file names from standard input, processes them sequentially, and provides the required file statistics. To run the program in serial mode:


<h6>$ ./SRH</h6>

Enter file names one per line. Press ***Ctrl + D*** to signal the end of input.


**or**

Piper in the ***ls*** command into the SRH, by:


<h6>$ ls -1d* | ./SRH</h6>

### Multi-Threaded Architecture:

The multi-threaded architecture allows for concurrent handling of file requests. To use this mode, include the argument thread followed by the maximum number of threads to execute at a time.


###### $ ./SRH thread ***[max_threads]***
<p>
Example, to run with at most 10 threads:</p>

**$ ./SRH thread 10**
<p>
Enter file names as in serial mode. Press Ctrl + D to signal the end of input.
</p>

**or**
<p>
Piper in the <strong>ls -1d*</strong> command into the SRH, by:
</p>

<h6>$ ls -1d* | ./SRH thread 10</h6>
           
## Output:
<p>
The program will provide statistics for each file, including the number of bad files, directories, regular files, special files, total bytes used, and more.
</p>


Ensure that the pthread library is available.

