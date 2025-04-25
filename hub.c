#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

pid_t childPid; //pid ul monitorului

void handler(int sig) {
    //citire comanda din fisierul pipe.txt
    int fd = open("pipe.txt", O_RDWR, 0777);
    if (fd == -1) {
        perror("Couldn't create/open file!");
        exit(-1);
    }
    char command[300];
    int n = read(fd, command, sizeof(command));
    printf("Raw command read from pipe.txt: %s\n", command);
    command[n] = '\0';
    char *args[5];
    char *token = strtok(command, " ");
    int i = 0;
    while (token != NULL) {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
    
    if (execvp(args[0], args) == -1) {
        perror("execvp failed");
        exit(1);
    }
    close(fd);
}

void endHandler(){
    exit(0);
}

void listTreasures(){
    if (childPid > 0) {
        int fd = open("pipe.txt", O_RDWR | O_CREAT|O_TRUNC, 0777);
        char huntId[100]={0};
        printf("Enter hunt id: ");
        if(scanf("%99s",huntId)!=1) exit(-1);
        char fullMessage[300]={0};
        sprintf(fullMessage,"%s %s %s","./p","--list",huntId);
        write(fd,fullMessage,strlen(fullMessage));
        close(fd);
        int result=kill(childPid, SIGUSR1);
        if (result == -1) {
            perror("Failed to send signal to child");
        } else {
            printf("Signal sent to child successfully\n");
        }
        usleep(100000);
    } else {
        printf("No monitor started yet!\n");
    }
}

void viewTreasure(){
    if (childPid > 0) {
        int fd = open("pipe.txt", O_RDWR | O_CREAT|O_TRUNC, 0777);
        char huntId[100]={0};
        char treasureId[100]={0};
        printf("Enter hunt id: ");
        if(scanf("%99s",huntId)!=1) exit(-1);
        printf("Enter treasure id: ");
        if(scanf("%99s",treasureId)!=1) exit(-1);
        char fullMessage[300]={0};
        sprintf(fullMessage,"%s %s %s %s","./p","--view",huntId,treasureId);
        write(fd,fullMessage,strlen(fullMessage));
        close(fd);
        kill(childPid, SIGUSR1);
        usleep(100000);
    } else {
        printf("No monitor started yet!\n");
    }
}

void stopMonitor(){
    kill(childPid, SIGTERM);
    int status;
    waitpid(childPid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Parent: Child exited normally with status %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Parent: Child was terminated by signal %d\n", WTERMSIG(status));
        } else {
            printf("Parent: Child ended abnormally\n");
        }
}

int main(){
    char command[100];
    int existsMonitor=0;
    //int fd = open("pipe.txt", O_RDWR | O_CREAT|O_TRUNC, 0777);

    /*if (fd == -1) {
        perror("Couldn't create/open file!");
        exit(-1);
    }*/
    //pipefd=fd;
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
            if(pid==0){
                struct sigaction sa,sa_end;
                sa_end.sa_handler = endHandler;
                sigemptyset(&sa_end.sa_mask);
                sa_end.sa_flags = 0;

                if (sigaction(SIGTERM, &sa_end, NULL) == -1) {
                    perror("sigaction(SIGTERM) failed");
                    exit(1);
                }

                sa.sa_handler = handler;
                sigemptyset(&sa.sa_mask);
                sa.sa_flags = 0;

                if (sigaction(SIGUSR1, &sa, NULL) == -1) {
                    perror("sigaction(SIGUSR1) failed");
                    exit(1);
                }
                while (1) {
                    pause(); 
                }

                exit(0);
            }
            printf("Monitor successfully started!\n");
            childPid=pid;
        }
        else if(strcmp(command,"list_treasures")==0){
            listTreasures();
        }
        else if(strcmp(command,"view_treasure")==0){
            viewTreasure();
        }
        else if(strcmp(command,"stop_monitor")==0){
            stopMonitor();
            existsMonitor=0;
        }
        else if(strcmp(command,"exit")==0){
            if(existsMonitor) printf("The monitor is still running!You can stop it with *stop_monitor*\n");
            else{ exit(0);}
        }
    }
    return 0;
}