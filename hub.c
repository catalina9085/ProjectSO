#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <ctype.h>

pid_t childPid; //pid ul monitorului
int existsMonitor=0;
int pfd[2];//intre monitor si programul principal
int sfd[2]; //intre scores si programul principal

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
    close(fd);
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
    pid_t pid=fork();
    if(pid==-1){
        perror("Couldn't open another process!");
        exit(-1);
    }
    if(pid==0){
        dup2(pfd[1],1);
        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(1);
        }
    }
}

void endHandler(){
    for(int i=0;i<10;i++)
        usleep(900000);
    exit(0);
}
void sendSignal(int type){
    if (childPid > 0) {
        int fd = open("pipe.txt", O_RDWR | O_CREAT|O_TRUNC, 0777);
        char fullMessage[300]={0};
        if(type==1){//list_hunts
            sprintf(fullMessage,"%s %s %s","./p","--list","--all");
            write(fd,fullMessage,strlen(fullMessage));
        }
        else if(type==2){//list_treasures
            char huntId[100]={0};
            printf("Enter hunt id: ");
            if(scanf("%99s",huntId)!=1) exit(-1);
            sprintf(fullMessage,"%s %s %s","./p","--list",huntId);
            write(fd,fullMessage,strlen(fullMessage));
        }
        else if(type==3){//view_treasure
            char huntId[100]={0};
            char treasureId[100]={0};
            printf("Enter hunt id: ");
            if(scanf("%99s",huntId)!=1) exit(-1);
            printf("Enter treasure id: ");
            if(scanf("%99s",treasureId)!=1) exit(-1);
            sprintf(fullMessage,"%s %s %s %s","./p","--view",huntId,treasureId);
            write(fd,fullMessage,strlen(fullMessage));
        }
        close(fd);
        int result=kill(childPid, SIGUSR1);
        if (result == -1) {
            perror("Failed to send signal to child");
        } else {
            printf("Signal sent to child successfully\n");
        }
        usleep(100000);
        char buffer[1024];
            ssize_t n;
            while((n=read(pfd[0],buffer,sizeof(buffer)-1))>0){
                buffer[n]=0;
                printf("Parintele a primit:\n%s\n",buffer);
            }
    } else {
        printf("No monitor started yet!\n");
    }
}




void stopMonitor(){
    kill(childPid, SIGTERM);
    int status;
    waitpid(childPid, &status, 0);

        if (WIFEXITED(status)) {
            existsMonitor=0;
            printf("Parent: Child exited normally with status %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Parent: Child was terminated by signal %d\n", WTERMSIG(status));
        } else {
            printf("Parent: Child ended abnormally\n");
        }
}
int isDirectory(char* dirName) {
    struct stat myStat;
    if (stat(dirName, &myStat) == -1)
        return 0;//nu exista
    if (S_ISDIR(myStat.st_mode)) return 1;
    return 0;
}
void calculateScores(){
    printf("\n");
    DIR* dir = opendir(".");
    struct dirent* entry;
    char **args=malloc(3*sizeof(char *));
    for(int i=0;i<2;i++)
        args[i]=malloc(100*sizeof(char));
    strcpy(args[0],"./score_p");
    args[2]=NULL;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0||strcmp(entry->d_name,".git")==0)
            continue;
        if (isDirectory(entry->d_name)) {
            strcpy(args[1],entry->d_name);
            pid_t pid=fork();
            if(pid==-1){
                perror("Couldn't open another process!");
                exit(-1);
            }
            if(pid==0){
                close(sfd[0]);
                dup2(sfd[1],1);
                if (execvp(args[0], args) == -1) {
                    perror("execvp failed");
                    exit(1);
                }
            }
            close(sfd[1]);
            usleep(100000);
        char buffer[1024];
            ssize_t n;
            while((n=read(sfd[0],buffer,sizeof(buffer)-1))>0){
                buffer[n]=0;
                printf("Parintele a primit:\n%s\n",buffer);
            }
        }
    }
    closedir(dir);
}
int main(){
    char command[100];
    if(pipe(pfd)<0 || pipe(sfd)<0){
        perror("Eroare la creare pipe!");
        exit(-1);
    }
    fcntl(pfd[0],F_SETFL,O_NONBLOCK);
    fcntl(sfd[0],F_SETFL,O_NONBLOCK);

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
                close(pfd[0]);//inchidem capatul de citire
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
            close(pfd[1]);//inchidem capatul de scriere
            childPid=pid;
        }
        else if(strcmp(command,"list_treasures")==0){
            sendSignal(2);
        }
        else if(strcmp(command,"view_treasure")==0){
            sendSignal(3);
        }
        else if(strcmp(command,"stop_monitor")==0){
            stopMonitor();
            printf("Monitorul a fost închis complet.\n");
        }
        else if(strcmp(command,"list_hunts")==0){
            sendSignal(1);
        }
        else if(strcmp(command,"calculate_scores")==0){
            calculateScores();
        }
        else if(strcmp(command,"exit")==0){
            if(existsMonitor) printf("The monitor is still running!You can stop it with *stop_monitor*\n");
            else{ exit(0);}
        }
    }
    return 0;
}