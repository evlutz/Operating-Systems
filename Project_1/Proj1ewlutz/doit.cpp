/*doit.cpp by Evan Lutz, ewlutz*/


/*
OBSORVATION:
Creating a mini shell told me a lot about built in handled commands
versus system calls. For instance the pipe line opperator "|" is not handled
by system calls but interpreted by the shell and then put into system calls. While other
commands like "man" handle perfectly in the shell
Also the implementation of background processes and how they are managed and handled,
I created a struct followed by a list of that struct to handle  and manage 
background commands. The waitpid function I found very intresting, especially how it
can wait for a process to finish or just check if the process has finished.
It is intruiging how the background tasks mingle in with the foreground task
and seeing how many voulentary stops there are for each command. I had an issue
dealing with my prompt I found telling of how the data and buffer are managed
after putting in a larger character array than orginially programmed for gave mysterious
and random results. I fixed the issue by using calloc do ensure the data has enough space
and does not get over written.
*/

#include <iostream>
using namespace std;
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>


/*
backgroundJob struct Holds processes: pid, and, call
*/
struct backgroundJob {
    pid_t pid;  //Process id
    char call[128];  //Command for background job
    struct timeval start;
    struct timeval end;
};

struct backgroundJob jobsList[32];   //List of background Jobs
int numberOfJobs = 0;   //Tracks number of currently operating jobs


/*
Takes in void, prints list of struct backgroundJob utilizing
jobList[], iterates through the list and prints the Process ID and
command call
*/
void printJobs(){
    for(int i = 0; i < numberOfJobs; i++){
        printf("{%d}, %d, %s\n", i+1, jobsList[i].pid, jobsList[i].call);
    }

}



/*
Parse line of characters into arguments, seperated by " " (SPACE)
Adds NULL terminator to the last argument
*/

void Parse(char *input, char *args[]){
    char *token = strtok(input, " ");   //parse the line by " "
    int argCount = 0;
    while (token != NULL && argCount < 31) {
        args[argCount++] = token;
        token = strtok(NULL, " ");
    }
    args[argCount] = NULL; // NULL terminated array to be handled by exec
}



int main(int argc, char **argv)
{

    char input[128];    // Line input
    char *argvNew[32];  // New arguments
    pid_t pid;  //Process ID
    int status; //Status or process
  //
    char *prompt = (char *)calloc(128, sizeof(char));
    prompt = "==>";


    struct rusage use;      //Collects data about the program in the system
    struct timeval startTime;   //Used for non background processes for wall clock
    struct timeval endTime;



    while(1){
        int backgroundFlag = 0; // Flag to be set off when background task is detected
        //Begin session
        printf("%s", prompt);   //Print current prompt



        if (fgets(input, sizeof(input), stdin) == NULL) {   //Read input from the terminal
         // Handle EOF
             printf("EOF ERROR\n");
            exit(1);
        }

        //remove end of line character
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        //Check for process to be ran in the background
        if(input[strlen(input) - 1] == '&') {
            input[strlen(input) - 1] = '\0';
            backgroundFlag = 1; //Background task Detected
         }

    
    Parse(input, argvNew); //Parse lne input into argument list


/*
Check for BUILT-IN commands, "exit", "jobs", "cd", 
"exit", "set prompt ="
*/

    gettimeofday(&startTime, NULL); //For wall-clock time, used in parent


/*
Wait for background jobs to finish once exiting,
Close all background
Clear numberofJobs
Exit
    //Check for Exit
*/
    if(strcmp(argvNew[0], "exit") == 0){
        if(numberOfJobs>0){
            printf("Waiting for background processees to finish...\n");
            for(int i = 0; i < numberOfJobs; i++){
            waitpid(jobsList[i].pid, &status, 0);
                gettimeofday(&jobsList[i].end, NULL);
                getrusage(jobsList[i].pid, &use);

                printf("[%d], %d, Completed\nBACKGROUND TASK STATS: \n", i+1, jobsList[i].pid);
                /*Print system statistics about call*/
                float timeTaken = (((jobsList[i].end.tv_sec*1000)+(jobsList[i].end.tv_usec/1000)) - ((jobsList[i].start.tv_sec*1000)+(jobsList[i].start.tv_usec/1000)));
                printf("Wall Clock Time (Milliseconds): %.4f \n", timeTaken);
                
                printf("User CPU time: %.4ld (ms)\n", (use.ru_utime.tv_sec*1000)+(use.ru_utime.tv_usec/1000));
                printf("System CPU time: %.4ld (ms)\n", (use.ru_utime.tv_sec*1000)+(use.ru_utime.tv_usec/1000));
                printf("Times preempted Voulentarily: %ld \n", use.ru_nvcsw);
                printf("Times preempted Unvoulentarily: %ld \n", use.ru_nivcsw);
                printf("Major page faults: %ld \n", use.ru_majflt);
                printf("Minor page faults: %ld \n", use.ru_minflt);
                printf("Max residents (kb): %ld \n\n\n", use.ru_maxrss);
                fflush(stdout);
                for(int z = i; z < numberOfJobs - 1; z++){  
                    jobsList[z] = jobsList[z + 1];          //Clear jobList
                }
            numberOfJobs--;
            i--;
        }
        }

        exit(1);
    }

    //Check for cd
    else if(strcmp(argvNew[0], "cd") == 0){
        if(chdir(argvNew[1]) == 0){
            printf("Current working directory: %s\n", getcwd(*argvNew, sizeof(argvNew)));
        }
        else{
            printf("Directory not in location");
    }

    }
   
    //Check for new prompt
    else if(strcmp(argvNew[0], "set") == 0 &&
       strcmp(argvNew[1], "prompt") == 0 &&
       strcmp(argvNew[2], "=") == 0){
            prompt = argvNew[3];
            printf("prompt changed\n");
            
       }
    //check for jobs
    else if(strcmp(argvNew[0], "jobs") == 0){
        printJobs();
    }
    //Calls Fork
    else if((pid = fork()) < 0) {
        cerr << "Fork error\n";
        exit(1);
    }

    else if (pid == 0) {
    /* child process */
        if (execvpe(argvNew[0], argvNew, environ) < 0) {
            cerr << "Improper Command: " << input << endl;
            exit(1);
        }

    }
    else {
    /* parent */

    //Check for background tasks to be completed
    /*Cycle through jobsList check if result is back at the parents (> 0)*/
        for(int i = 0; i < numberOfJobs; i++){
            pid_t isDone = waitpid(jobsList[i].pid, &status, WNOHANG);
            if(isDone > 0){

                printf("{%d}, %d, Completed\nBACKGROUND TASK STATS: \n", i+1, jobsList[i].pid);
                        /*Print system statistics about call*/
                
                gettimeofday(&jobsList[i].end, NULL);
                float timeTaken = (((jobsList[i].end.tv_sec*1000)+(jobsList[i].end.tv_usec/1000)) - ((jobsList[i].start.tv_sec*1000)+(jobsList[i].start.tv_usec/1000)));
                printf("Wall Clock Time (Milliseconds): %.4f \n", timeTaken);
                getrusage(jobsList[i].pid, &use);
                printf("User CPU time: %.4ld (ms)\n", (use.ru_utime.tv_sec*1000)+(use.ru_utime.tv_usec/1000));
                printf("System CPU time: %.4ld (ms)\n", (use.ru_utime.tv_sec*1000)+(use.ru_utime.tv_usec/1000));
                printf("Times preempted Voulentarily: %ld \n", use.ru_nvcsw);
                printf("Times preempted Unvoulentarily: %ld \n", use.ru_nivcsw);
                printf("Major page faults: %ld \n", use.ru_majflt);
                printf("Minor page faults: %ld \n", use.ru_minflt);
                printf("Max residents (kb): %ld \n\n\n", use.ru_maxrss);
                fflush(stdout);
                for(int z = i; z < numberOfJobs - 1; z++){
                    jobsList[z] = jobsList[z + 1];
                }
            numberOfJobs--;
            i--;
            }


        }

    //Create new background job struct and add to jobsList
        if(backgroundFlag){
           struct backgroundJob newJob;
           newJob.pid = pid;   //save pid for background job
           gettimeofday(&newJob.start, NULL); //Get start time for job
           strncpy(newJob.call, input, sizeof(newJob.call)); //Copy command over to new job
           jobsList[numberOfJobs++] = newJob; //Iterate Job list by 1 and add new job to list
           printf("[%d], %d, %s\n", numberOfJobs, pid, input); //Print job
        }
        else{
        /* If not a background task wait for the child to finish */
            waitpid(pid, &status, 0);
                        
                        /*Print system statistics about call*/
            gettimeofday(&endTime, NULL);
                float timeTaken = (((endTime.tv_sec*1000)+(endTime.tv_usec/1000)) - ((startTime.tv_sec*1000)+(startTime.tv_usec/1000)));
                printf("Wall Clock Time (ms): %.4f \n", timeTaken);
                getrusage(RUSAGE_CHILDREN, &use);
                printf("User CPU time: %.4ld (ms)\n", (use.ru_utime.tv_sec*1000)+(use.ru_utime.tv_usec/1000));
                printf("System CPU time: %.4ld (ms)\n", (use.ru_utime.tv_sec*1000)+(use.ru_utime.tv_usec/1000));
                printf("Times preempted Voulentarily: %ld \n", use.ru_nvcsw);
                printf("Times preempted Unvoulentarily: %ld \n", use.ru_nivcsw);
                printf("Major page faults: %ld \n", use.ru_majflt);
                printf("Minor page faults: %ld \n", use.ru_minflt);
                printf("Max residents (kb): %ld \n", use.ru_maxrss);
                fflush(stdout);
            
        }
    
}
    }

}

