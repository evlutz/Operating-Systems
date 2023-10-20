#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <semaphore.h>
using namespace std;

#define MAXLINE 50
#define MAXFILES 50

#define BUFSIZE 10096

// Global counts
int numbFiles;
int badFilesGlobal;
int numbDirGlobal;
int numbRegGlobal;
int numbSpecialGlobal;
int regularBytesGlobal;
int allTextGlobal;
int textFileBytesGlobal;
bool useThreads = true;

/* 
   @brief Texr files need to be opened and processed differently
*/
void HandleTextFile();

/**
 * @brief Handle the output of -l and return the file name. Read until EOF.
 */
void ProcessInput();

/**
 * @brief Check if file contains all text.
 * @param fileIn The input file name.
 * @return True if all characters are text, otherwise false.
 */
bool isAllCharacters(string fileIn);

/**
 * @brief Ran by threads to check a file.
 * @param arg A pointer to the file name.
 * @implements *checkFile
 */
void *checkFile(void* arg);

/**
 * @brief Mutexed function, contains access to global variables.
 * @param badFiles Number of bad files.
 * @param numbDir Number of directories.
 * @param numbReg Number of regular files.
 * @param numbSpecial Number of special files.
 * @param regularBytes Number of bytes in regular files.
 * @param allText Number of text files.
 * @param textFileBytes Number of bytes in text files.
 */
void addTotalsToGlobal(int badFiles, int numbDir, int numbReg, int numbSpecial, int regularBytes, int allText, int textFileBytes);

/**
 * @brief Counts number of regular, special, dirs, regular file bytes, text file bytes.
 * @param thisFile The input file name.
 */
void countFile(string thisFile);

pthread_mutex_t CountMutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char* argv[]) {
    int numbThreads;

    if (argc > 1 && !strcmp(argv[1], "thread")) {
        numbThreads=1;
        if (argc == 3 && isdigit(argv[2][0])) {
            if(atoi(argv[2])>15){
                cerr << "Too many threads requested. Max: 15 " << strerror(errno) << endl;
                exit(1);
            }
            numbThreads = atoi(argv[2]);
        }
    }
    else if (argv[1] != "thread" && argc == 2){
        cout << "Invalid argument. Usage: ./proj4 [thread] [# of threads]" << endl;
        return 1; // Return an error code
    }

    /* Serialization */
    if (argc == 1) {
        useThreads = false;
        string fileName;
        while (getline(cin, fileName)) {
            countFile(fileName);
        }
        printf("Bad Files: %d\n", badFilesGlobal);
        printf("Directories: %d\n", numbDirGlobal);
        printf("Regular Files: %d\n", numbRegGlobal);
        printf("Special Files: %d\n", numbSpecialGlobal);
        printf("Regular File Bytes: %d\n", regularBytesGlobal);
        printf("Text Files: %d\n", allTextGlobal);
        printf("Text File Bytes: %d\n", textFileBytesGlobal);
        return 1;
    }

    string thisFile;
    vector<pthread_t> threadList;

    while (getline(cin, thisFile)) {

        while (threadList.size() >= (unsigned int) numbThreads) {
            pthread_join(threadList[0], NULL);
            threadList.erase(threadList.begin());
        }

        string* allocFile = new string(thisFile);
        pthread_t threadID;

        pthread_create(&threadID, NULL, checkFile, allocFile);
        threadList.push_back(threadID);
    }

    for (pthread_t threadID : threadList) {
        pthread_join(threadID, NULL);
    }

    printf("Bad Files: %d\n", badFilesGlobal);
    printf("Directories: %d\n", numbDirGlobal);
    printf("Regular Files: %d\n", numbRegGlobal);
    printf("Special Files: %d\n", numbSpecialGlobal);
    printf("Regular File Bytes: %d\n", regularBytesGlobal);
    printf("Text Files: %d\n", allTextGlobal);
    printf("Text File Bytes: %d\n", textFileBytesGlobal);
}

void countFile(string thisFile){
    int badFiles = 0;
    int numbDir = 0;
    int numbReg = 0;
    int numbSpecial = 0;
    int regularBytes = 0;
    int allText = 0;
    int textFileBytes = 0;
    
    struct stat fileInfo;
    int dirFlag = 0;
    int regFlag=0;
    /* Cycle through file list determine file characteristics */
    if(stat(thisFile.c_str(), &fileInfo) == 0){
        if(S_ISDIR(fileInfo.st_mode)){  // Check for directories
            numbDir++;
            dirFlag=1;
        }
        if(S_ISREG(fileInfo.st_mode)){  // Check for regular files
            numbReg++;
            regFlag++;
            regularBytes += fileInfo.st_size;   // Grab number of bytes for a file
            if(isAllCharacters(thisFile)){
                allText++;
                textFileBytes += fileInfo.st_size;
            }
        }
        if(!dirFlag&&!regFlag){     // If a file is not a directory or a regular file it is special
            numbSpecial++;
        }
    }
    else{
        badFiles++;
    }

    if(useThreads){
        pthread_mutex_lock(&CountMutex);
        addTotalsToGlobal(badFiles, numbDir, numbReg, numbSpecial, regularBytes, allText, textFileBytes);
        pthread_mutex_unlock(&CountMutex);
    }
    else{
        addTotalsToGlobal(badFiles, numbDir, numbReg, numbSpecial, regularBytes, allText, textFileBytes);
    }
}

void *checkFile(void* arg){
    string* strPtr = (string*) arg;
    string thisFile = *strPtr;
    delete strPtr;
    countFile(thisFile);
}

void addTotalsToGlobal(int badFiles, int numbDir, int numbReg, int numbSpecial, int regularBytes, int allText, int textFileBytes){
    
    // Access global variables
    badFilesGlobal += badFiles;
    numbDirGlobal += numbDir;
    numbRegGlobal += numbReg;
    numbSpecialGlobal += numbSpecial;
    regularBytesGlobal += regularBytes;
    allTextGlobal += allText;
    textFileBytesGlobal += textFileBytes;
}

bool isAllCharacters(string fileIn) {
    char buf[BUFSIZE];
    ssize_t bytesRead = 0;

    int fd;
    if ((fd = open(fileIn.c_str(), O_RDONLY)) == -1) {
        cerr << "Could not open a file: " << strerror(errno) << endl;
        return false;  // Return false if file cannot be opened
    }
    // Copy input to stdout
    while ((bytesRead = read(fd, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            if (!isprint(buf[i]) && !isspace(buf[i])) {
                close(fd);
                return false;  // Found a non-text character
            }
        }
    }
    close(fd);
    return true;  // All characters are text
}