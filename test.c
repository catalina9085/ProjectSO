#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>



int main(){
    pid_t pid=fork();
            if(pid==-1){
                perror("Couldn't open another process!");
                 exit(-1);
            }
            if(pid==0){
    char *args[] = {
        "./p",  // Programul executabil
        "--list",            // Primul argument
        "hunt1",             // Al doilea argument
        NULL                 // Terminați array-ul cu NULL
    };
    chdir("./treasureHunt");
    execvp("./p",args);
    exit(0);
}
    return 0;

}