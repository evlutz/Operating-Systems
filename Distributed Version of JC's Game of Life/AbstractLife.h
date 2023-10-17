/**@file AbstractLife.h
 * Abstract for life.cpp "game_of_life"
 * Utilizes: "life.h"
 * Contains: Function implementations
 * By: Evan Lutz */

#include <pthread.h>
#include "life.h"

class LifeFunctions : public LifeAbstractFunctions {
public:

    /**
     * @brief Calculates the next generation of the grid 
     *        based on the Game of Life rules.
     * @param gen The current generation.
     * @param starts The starting row for the thread.
     * @param end The ending row for the thread.
     */
    void calculateGen(int gen, int starts, int end) {
        // Define an array of neighboring cells
        int neighbors[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

        // Loop through rows assigned to this thread
        for(int i = starts; i <= end; i++) {
            for (int j = 0; j < columns; j++) {
                // Get the current state of neighboring cell
                int self = OLDGEN[i][j];
                int occupied = 0;

                // Count the number of occupied neighbors
                for (int k = 0; k < 8; k++) {
                    int ni = i + neighbors[k][0];
                    int nj = j + neighbors[k][1];
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
                        occupied += OLDGEN[ni][nj]; // Counting alive neighboring cells
                    }
                }

                // Apply Game of Life rules
                switch (self) {
                    case 1:
                        if (occupied != 2 && occupied != 3) {
                            self = 0; // Death underpopulation or overpopulation
                        }
                        break;
                    case 0:
                        if (occupied == 3) {
                            self = 1; // Birth exactly three neighbors
                        }
                        break;
                }

                // Update the next generation grid
                NEWGEN[i][j] = self;
            }
        }
    }

    /**
     * @brief Controls and processes message recieving with semaphores
     *        Ensures a message is not being recieved wil it is being read.
     * @param iFrom The index of the sender's thread.
     * @param Msg Reference to the message structure where the
     *            message will be stored.
     */
    void RecvMsg(int iFrom, msg &Msg) {

        sem_wait(&ClusterBox[iFrom].recvSem);
        Msg = ClusterBox[iFrom].message;
        sem_post(&ClusterBox[iFrom].sendSem);
    }

    /**
     * @brief Controls and processes message sending with semaphores
     *        Ensures a message cannot be sent to a already full mailbox
     * @param iTo The index of the recipient's thread.
     * @param Msg The message struct to be sent.
     */
    void SendMsg(int iTo, msg &Msg) {
        sem_wait(&ClusterBox[iTo].sendSem);
        ClusterBox[iTo].message = Msg;
        sem_post(&ClusterBox[iTo].recvSem);
    }


    /**
     * @brief Checks if two generations of the grid are identical.
     * @param one First grid.
     * @param two Second grid.
     * @return bool Returns true if the grids are equal, false otherwise.
     */
    bool isGenSame(Grid one, Grid two) {
        for (size_t i = 0; i < one.size(); ++i) {
            for (size_t j = 0; j < one[i].size(); ++j) {    
                if (one[i][j] != two[i][j]) {
                    return 0; 
                }
            }
        }
        return 1; 
    }

    /**
     * @brief Prints the grid after each game.
     * @param genNumb The generation number.
     * @param grid The grid to be printed.
     */
    void PrintGens(int genNumb, Grid grid) {
        printf("Generation: %d\n", genNumb);
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                if (grid[i][j] == 1) {
                    printf("■ ");
                } else {
                    printf("□ ");
                }
            }
            printf("\n");
        }
    }

    /**
     * @brief Parses an original generation from file, fills finalgen
     *        with zeros and stores in: NEWGEN, FINALGEN, OLDGEN
     * @param fileName The name of the file to be parsed.
     */
    void ParseNMakeGrid(char *fileName) {
        FILE* filePtr = fopen(fileName, "r");

        // Check if the file was successfully opened
        if(filePtr == NULL){
            perror("Could not open file");
            exit(1);
        }

        // Create a character array to store a line from the file
        char grid[80];

        // Read lines from the file until the end is reached
        while(fgets(grid, sizeof(grid), filePtr)) {
            Row newRow;      // new row for the grid
            Row rowOfNULL;   // row of zeros for final

            char * cell = strtok(grid, " ");

            // Process each token (cell)
            while(cell != NULL) {
                // Create a row, 
                newRow.push_back(atoi(cell));
                rowOfNULL.push_back(0);

                cell = strtok(NULL, " ");
            }

            // Add the new row to various arrays (NEWGEN, FINALGEN, OLDGEN)
            NEWGEN.push_back(newRow);
            FINALGEN.push_back(rowOfNULL);
            OLDGEN.push_back(newRow);
        }

        // Close the file after reading
        fclose(filePtr);
    }

    /**
     * @brief Creates a msg structure.
     * @param iSender The index of the Threads.
     * @param type The type of message.
     * @param value1 The first value passed.
     * @param value2 The second value passed.
     * @return msg Returns a msg struct.
     */
    msg MakeMessage(int iSender, int type, int value1, int value2) {
        struct msg newMsg = {iSender, type, value1, value2};
        return newMsg;
    }

};

typedef LifeFunctions LifeFunctions;

