/**@file life.cpp
 * Main for game_of_life
 * Contains: main, thread functions
 * Utilizes: "AbstractLife.h"
 * By: Evan Lutz
*/
#include <iostream>
#include <fstream>
#include <string.h>
#include <semaphore.h>
#include <cstring>
#include "AbstractLife.h"
using namespace std;


/**
 * @brief Threads recieve an orginal message of type RANGE gives frid reigon they have control over
 * Go message begins calculation of games. sending back to the main thread
 * when finished. Messages ALLDONE when finished, or recieves STOP, when game
 * has stopped
 * @param arg A pointer to the thread ID.
 * @return void*
 */
void* thread(void* arg);


/**
 * @brief The main function for the program.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line argument strings.
 * @return int Exit status of the program.
 */
int main(int argc, char* argv[]) {

    // Check if the correct number of command line arguments is provided
    if (argc < 4 || argc > 6) {

        printf("Usage:\n./game_of_life\n [# of Threads to use]\n [File name]\n [Max generations]\n Optional: [Print generations]\n Optional: [Wait for input]\n");
        exit(1);
    }

    // Extract arguments from command line
    int numbThread = atoi(argv[1]);  // Number of threads
    char* fileName = argv[2];       // Name of the file containing grid
    gens = atoi(argv[3]);           // Number of generations to do
    bool isPrint = false;           // Print any generations flag
    bool wait4Key = false;          // Wait for user input flag

 
    if (argc >= 5 ) {
        isPrint = (argv[4][0] == 'y');
    }


    if (argc == 6) {
        wait4Key = (argv[5][0] == 'y');
    }

    if(numbThread > MAXTHREAD){
        perror("To many threads, MAX: 10.\n");
        exit(1);
    }

    LifeFunctions lifeObject; /*Grab functions from "AbstractLife.h"*/

    // Parse the grid from the file
    lifeObject.ParseNMakeGrid(fileName);
   // ParseNMakeGrid(fileName);

    //Get dimensions of the grid
    rows = OLDGEN.size();
    columns = OLDGEN[0].size();


    if (rows > MAXGRID || columns > MAXGRID) {
        perror("The original grid exceeds the grid limit 40");
        exit(1);
    }

    //Ensure that the number of threads does not exceed the number of rows
    numbThread = min(rows, numbThread);

    //Initialize Semaphores for threads
    ClusterBox = new MBox[numbThread + 1];
    for (int i = 0; i < numbThread + 1; i++) {
        if (sem_init(&ClusterBox[i].sendSem, 0, 1) != 0) {
            perror("pthread_create");
            exit(1);
        }
        if (sem_init(&ClusterBox[i].recvSem, 0, 0) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    //Create threads 
    pthread_t id[numbThread + 1];
    for (int i = 1; i <= numbThread; i++) {
        int* indexArg = new int(i);
        if (pthread_create(&id[i], NULL, thread, (void*) indexArg)) {
            perror("pthread_create");
            exit(1);
        }
    }

    //Send grid range to threads
    for (int i = 1; i <= numbThread; i++) {
        msg whichRow = lifeObject.MakeMessage(
                        0, 
                        RANGE, 
                        ((i-1) * rows / numbThread), 
                        (i * rows / numbThread) - 1);
        lifeObject.SendMsg(i, whichRow);
    }

    //Print gen 0, if Print
    if(isPrint){
        lifeObject.PrintGens(0, OLDGEN);
    }

    // Iterate through generations
    for (int currGen = 1; currGen <= gens; currGen++) {

        // Send GO message to threads to start playing
        for (int i = 1; i <= numbThread; i++) {
            msg GOmsg;
            GOmsg.iSender = 0;
            GOmsg.type = GO;
            lifeObject.SendMsg(i, GOmsg);
        }

        int numbThreadDone = 0;     //Tracks GENDONE, per generation
        int numbThreadAlldone = 0;  //Tracks ALLDONE, if the thread has finished completely
        msg threadmsg;

        //Max generation reached
        if (currGen == gens) {

            while (numbThreadAlldone != numbThread) {
                lifeObject.RecvMsg(0, threadmsg);
                if (threadmsg.type == ALLDONE) {
                    numbThreadAlldone++;
                }
            }
            if (wait4Key) {
                printf("Please press any key to continue...\n");
                getchar();
                lifeObject.PrintGens(currGen, NEWGEN);
            } else {
                if(isPrint){    
                    lifeObject.PrintGens(currGen, NEWGEN);
                }
            }


        } 
        else /*Game is not finished play next generation*/
        {

            while (numbThreadDone != numbThread) {
                lifeObject.RecvMsg(0, threadmsg);
                if (threadmsg.type == GENDONE) {
                    numbThreadDone++;
                }
            }

            if(wait4Key) {
                printf("Please press any key to continue...\n");
                getchar();
                if(isPrint){
                    lifeObject.PrintGens(currGen, NEWGEN); 
                }
            } 
            else{
                if(isPrint){
                    lifeObject.PrintGens(currGen, NEWGEN); 
                }
            }
        }

        // Check if game is redundant, and send STOP if is
        if(lifeObject.isGenSame(NEWGEN, FINALGEN)) {
            printf("Game Stopped: Game has reached redundancy\n");
            for (int i = 1; i <= numbThread; i++) {
                msg STOPmsg = lifeObject.MakeMessage(0, STOP, 0, 0);    //Send STOP to kill all threads
                lifeObject.SendMsg(i, STOPmsg);
            }
            exit(1);
        }

        if (currGen != 1 && lifeObject.isGenSame(OLDGEN, NEWGEN)) {
            printf("Game Stopped: Repetitive Generations\n");
            for (int i = 1; i <= numbThread; i++) {
                msg STOPmsg = lifeObject.MakeMessage(0, STOP, 0, 0);
                lifeObject.SendMsg(i, STOPmsg);
            }
            exit(1);
        }
        //Update the old generation
        OLDGEN = NEWGEN;
    }
    printf("Number of set generations reached\n");
    return 0;
}



void* thread(void* arg) {
    LifeFunctions TlifeObject;
    int threadID = *(int*) arg; //Passed the index of the thread

    msg getRowMsg;
    TlifeObject.RecvMsg(threadID, getRowMsg);   //Look for rows assigned to this thread

    
   msg StopMsg; 

    //Wait till main tells thread to comput assigned roe
    for (int gen = 1; gen <= gens; gen++) {
        msg look4Go;
        TlifeObject.RecvMsg(threadID, look4Go);
        if(look4Go.type == STOP){      //Check for stop message
            sem_destroy(&ClusterBox[threadID].recvSem);
            sem_destroy(&ClusterBox[threadID].sendSem);
            pthread_exit(NULL);
    }
        //Compute new gen
        if (look4Go.type == GO) {
            TlifeObject.calculateGen(gen, getRowMsg.rowBegin, getRowMsg.rowEnd);
            msg signalDone = TlifeObject.MakeMessage(threadID,  GENDONE, 0, 0);
            TlifeObject.SendMsg(0, signalDone); //Signal thread has finished with gen
        }
    }

    //Send alldone, number of generations met
    msg signalAllDone = TlifeObject.MakeMessage(threadID, ALLDONE, 0, 0);
    TlifeObject.SendMsg(0, signalAllDone);
    sem_destroy(&ClusterBox[threadID].recvSem);
    sem_destroy(&ClusterBox[threadID].sendSem);
    pthread_exit(NULL);
}
