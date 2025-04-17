#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
void handle_list_treasures(int sig) {
    printf("\nChild: Received signal to list treasures!\n");

}

pid_t childPid;
int main(){
    char command[100];
    int existsMonitor=0;
    while(1){
        printf("Enter comand: ");
        if(scanf("%99s",command)!=1) exit(-1);
        if(strcmp(command,"start_monitor")==0){
            if(existsMonitor){
                printf("There's a monitor opened already!");
                continue;
            }
            existsMonitor=1;
            pid_t pid=fork();
            if(pid==-1){
                perror("Couldn't open another process!");
                 exit(-1);
            }
            printf("Monitor successfully started!\n");
            childPid=pid;
            if(pid==0){
                struct sigaction sa;
                sa.sa_handler = handle_list_treasures;
                sigemptyset(&sa.sa_mask);
                sa.sa_flags = 0;

                if (sigaction(SIGUSR1, &sa, NULL) == -1) {
                    perror("sigaction");
                    exit(1);
                }
                while (1) {
                    pause(); 
                }

                exit(0);
            }
        }
        else if(strcmp(command,"list_treasures")==0){
            if (childPid > 0) {
                kill(childPid, SIGUSR1);
            } else {
                printf("No monitor started yet!\n");
            }
        }
    }
    return 0;
}