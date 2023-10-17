/**@file life.h
 * Header for game_of_life, "life.cpp", utilized in "AbstractLife.h"
 * Contains: Macros, Structs, and Function declarations
 * By: Evan Lutz*/

#include <vector>
using namespace std;


/********************************* RESTRICTIONS ******************/
#define MAXGRID 40
#define MAXTHREAD 10


/********************************* MESSAGES ******************/
/*Message passing between threads threads to synchronize game*/
#define RANGE 1     //First message, tells threads which rows they are to compute - Sent to Threads
#define ALLDONE 2   //Thread has completed User set # of generations - Sent to controlling thread
#define GO 3        //Begin new gerneration - Sent to Threads
#define GENDONE 4   //Generation has been completed - Sent to controlling thread
#define STOP 5      //Sent if a repedative generation has been reached - clear memory and exit - Sent to Threads


/************************************** STRUCTS *****************************************/

/*Message struct, sent between threads*/
struct msg {
int iSender; /* sender of the message (0 .. number-of-threads) */
int type; /* its type */
int rowBegin; /* first value */
int rowEnd; /* second value */
}; typedef struct msg msg;

/*Mailbox, assigned to each thread, contains producing
* and consuming sem for each semaphore
*/
struct MBox {
    sem_t sendSem;
    sem_t recvSem;
    struct msg message;
};
typedef struct MBox MBox;

typedef vector<int> Row;    //Array of integers
typedef vector<Row> Grid;   //Array of Rows, 2D array

/******************************** GLOBAL VARIABLES **************************************/

MBox* ClusterBox = nullptr;     //Array of Mailboxes
Grid OLDGEN;
Grid NEWGEN;
Grid FINALGEN;
int rows = 0;       //Number of rows in grid
int columns = 0;    //Number of columns
int gens = 0;       //Number of generations


/******************************** FUNCTIONS **************************************/
//Function Declarations
class LifeAbstractFunctions{
public:


    /**
     * @brief Calculates the next generation of the grid 
     * based on the Game of Life rules.
     * @param gen The current generation.
     * @param starts The starting row for the thread.
     * @param end The ending row for the thread.
     */
    virtual void calculateGen(int gen, int startRow, int endRow) = 0;

    /**
     * @brief Semaphore, controlling acces to each thread and the
     * main thread in memory space.
     * @param iTo The index of the recipient's thread.
     * @param Msg The message struct to be sent.
     */
    virtual void SendMsg(int iTo, msg &Msg) = 0;    

    /**
     * @brief Receives a message from a specified thread.
     * @param iFrom The index of the sender's thread.
     * @param Msg Reference to the message structure where the
     * message will be stored.
     */
    virtual void RecvMsg(int iFrom, msg &Msg) = 0;

    /**
     * @brief Checks if two generations of the grid are identical.
     * @param one First grid.
     * @param two Second grid.
     * @return bool Returns true if the grids are equal, false otherwise.
     */
    virtual bool isGenSame(Grid one, Grid two) = 0;

    virtual void PrintGens(int gen, Grid grid) = 0;

    /**
     * @brief Parses a original generation from file, fills finalgen
     * with zeros 
     * stores in: NEWGEN, OLDGEN, FINALGEN
     * @param fileName The name of the file to be parsed.
     */
    virtual void ParseNMakeGrid(char *fileName) = 0;

    /**
     * @brief Creates a msg structure.
     * @param iSender The index of the Threads.
     * @param type The type of message.
     * @param value1 The first value passed.
     * @param value2 The second value passed.
     * @return msg Returns a msg struct.
     */
    msg MakeMessage(int iSender, int type, int value1, int value2);
};
//////////////////////////////////////////////////////
