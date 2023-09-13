#include <unistd.h>
#include <iostream>
using namespace std;
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

int main(int argc, char *argv[]){

struct rusage use;
pid_t pid;
int status;

if ((pid = fork()) < 0) {
    cerr << "Fork error\n";
    exit(1);
}

else if (pid == 0) {
    /* child process */
    cerr << "Process ID: " + to_string(pid) + "\n" ;

    char *file_name = argv[1];
    char *arg1 = argv[2];
    char *arg2 = argv[3];
    char *argvNew[5] = {argv[2], argv[3], argv[4], NULL};

    //char *cpuUse[4] = {"ps", "-p", pid, "-o", "%cpu"};

    if(execvp(file_name, argvNew) < 0){
        cerr << "Error\n";
        exit(1);
    }
}
else {
    
   // waitpid(pid, &status, 0);
   wait(0);
    /*Parrent Process*/
    getrusage(RUSAGE_CHILDREN, &use);
    printf("User CPU time: %ld.%06ld seconds\n", use.ru_utime.tv_sec, use.ru_utime.tv_usec);
    printf("System CPU time: %ld.%06ld seconds\n", use.ru_stime.tv_sec, use.ru_stime.tv_usec);

    

}

}