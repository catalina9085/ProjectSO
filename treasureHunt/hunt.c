#include "hunt.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <ctype.h>
void closeDir(DIR* dir) {
    if (closedir(dir) == -1) {
        perror("Couldn't close the directory!");
        exit(-1);
    }
}
void closeFile(int fd) {
    if (close(fd) == -1) {
        perror("Couldn't close the file!");
        exit(-1);
    }
}

//scriere in fisierul de log intern,specific fiecarui hunt
void writeInInternalLogFile(char* dirName, char* internalLogName, char* message) {
    char internLogPath[100];
    sprintf(internLogPath, "%s/%s", dirName, internalLogName);
    int logFD = open(internLogPath, O_RDWR | O_APPEND, 0777);
    if (logFD == -1) {
        perror("couldn't open internal log file!");
        exit(-1);
    }
    time_t now=time(NULL);
    struct tm *t=localtime(&now);
    char time[32];
    strftime(time,sizeof(time),"[%Y-%m-%d %H:%M:%S]",t);
    if(write(logFD,time,strlen(time))!=strlen(time)){
    	perror(NULL);
    	exit(-1);
    }
    if(write(logFD, message, strlen(message))!=strlen(message)){
    	perror(NULL);
    	exit(-1);
    }
    closeFile(logFD);

}

void toLowerCaseString(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
    }
}

//la comanda --add,userul intra intr-o bucla unde decide daca vrea sa mai adauge un treasure
//dupa fiecare treasure adaugat,se adauga mesajul in fisierul de log
Treasure** readTreasures(int* size,char *dirName) {
    char id[max]={0},name[max]={0},clue[max]={0},message[4000]={0};
    double lat, lng;
    int val;
    char agreeToContinue[10]={0};
    int capacity=max;
    Treasure** treasures = malloc(capacity * sizeof(Treasure*));
    if (!treasures) {
        perror("Couldn't allocate memory to treasures!");
        exit(-1);
    }
    *size = 0;
    while (1) {
        printf("Want to continue to add a treasure? yes/no ");
        if (!fgets(agreeToContinue, 10, stdin)) break;
        toLowerCaseString(agreeToContinue);
        if (strcmp(agreeToContinue, "no") == 0 || strcmp(agreeToContinue, "yes\n") != 0) break;
        printf("Treasure id(string): ");
        if (!fgets(id, max, stdin)) break;
        id[strlen(id) - 1] = 0;
        printf("Username(string): ");
        if (!fgets(name, max, stdin)) break;
        name[strlen(name) - 1] = 0;
        printf("Gps coordinates(double double): ");
        if (scanf("%lf %lf", &lat, &lng) != 2) break;
        getchar();//consumam \n
        printf("Clue(string): ");
        if (!fgets(clue, max, stdin)) break;
        clue[strlen(clue) - 1] = 0;
        printf("Treasure value(integer): ");
        if (scanf("%d", &val) != 1) break;
        getchar();
        Treasure* newTreasure = malloc(sizeof(Treasure));
        strcpy(newTreasure->id, id);
        strcpy(newTreasure->name, name);
        strcpy(newTreasure->clue, clue);
        newTreasure->val = val;
        newTreasure->lat = lat;
        newTreasure->lng = lng;
        if(*size>=capacity){
        	capacity+=100;
        	treasures=realloc(treasures,capacity*sizeof(Treasure *));
        	if(!treasures) exit(-1);
        }
        treasures[(*size)++] = newTreasure;
        sprintf(message,"--add %s (%s,%s,%.2lf,%.2lf,%s,%d)\n",dirName,id,name,lat,lng,clue,val);
	writeInInternalLogFile(dirName,"logged_hunt.bin",message);
	
    }
    return treasures;
}

int isDirectory(char* dirName) {
    struct stat myStat;
    if (stat(dirName, &myStat) == -1)
        return 0;//nu exista
    if (S_ISDIR(myStat.st_mode)) return 1;
    return 0;
}

//functie pentru comanda "--add <huntId>"
void addHunt(char* dirName) {
    char internLogPath[100];
    sprintf(internLogPath, "%s/logged_hunt.bin", dirName);
    if (!isDirectory(dirName)) {//daca nu exista deja,il cream
        if (mkdir(dirName,0777) != 0) {
            perror("Couldn't create directory");
            exit(-1);
        }
        int logFD = open(internLogPath,O_CREAT, 0777);
        if(logFD==-1){
        	perror("Couldn't create log file!");
        	exit(-1);
        }
        char symbolicLog[100]={0};
        sprintf(symbolicLog,"logged_hunt_%s.bin",dirName);//fisierul din directorul de lucru
        int sl = symlink(internLogPath, symbolicLog);
        if (sl == -1) {
            perror("couldn't create symlink!");
            exit(-1);
        }
    }
    int size = 0;
    Treasure** treasures = readTreasures(&size,dirName);

    char filePath[max];
    sprintf(filePath, "%s/treasures.bin", dirName);
    int fd = open(filePath, O_RDWR | O_CREAT | O_APPEND, 0777);

    if (fd == -1) {
        perror("Couldn't create/open file!");
        exit(-1);
    }
    for (int i = 0; i < size; i++) {
        ssize_t written = write(fd, treasures[i], sizeof(Treasure));
        free(treasures[i]);
        if (written == -1) {
            perror("Couldn't write in file!");
            close(fd);
            exit(-1);
        }
    }
    free(treasures);
    closeFile(fd);


}


void printTreasures(char* dirName, char* fileName) {
    printf("\n----------------------------------------------------------------------------------------------\n");
    char path[max];
    sprintf(path, "%s/%s", dirName, fileName);
    struct stat myStat;
    if (stat(path, &myStat) == -1) {
        perror("Couldn't read file properties");
        exit(-1);
    }
    printf("Hunt id:%s\n", dirName);
    printf("File size:%ld\nLast modified:%s\n", myStat.st_size, ctime(&myStat.st_mtime));
    int fd = open(path, O_RDONLY, 0777);
    if (fd == -1) {
        perror("Couldn't open the treasures file!");
        exit(-1);
    }
    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %s\nName: %s\nCoordinates: (%.2lf, %.2lf)\nClue: %s\nValue: %d\n\n",
            t.id, t.name, t.lat, t.lng, t.clue, t.val);
    }
    closeFile(fd);

}

//functie pentru comanda "--list <huntId>"
void listHunt(char* dirName) {
    DIR* dir = opendir(dirName);
    if (!dir) {
        perror("Couldn't open directory!");
        exit(-1);
    }
    int found = 0;
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, "treasures.bin") == 0) {
            printTreasures(dirName, entry->d_name);
            found = 1;
            break;
        }
    }
    if (!found) {
        perror("Couldn't find the treasures file in the directory!");
        exit(-1);
    }
    closeDir(dir);
    char message[max];
    sprintf(message,"--list %s\n",dirName);
    writeInInternalLogFile(dirName,"logged_hunt.bin",message);
}

//functie pentru comanda "--view <huntId> <treasureId>"
void viewTreasure(char* huntId, char* treasureId) {
    DIR* dir = opendir(huntId);
    if (!dir) {
        perror("Couldn't open directory!");
        exit(-1);
    }
    char path[100];
    sprintf(path, "%s/%s", huntId, "treasures.bin");

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Couldn't create/open file!");
        exit(-1);
    }
    Treasure t;
    int found = 0;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (strcmp(t.id, treasureId) == 0) {
            found = 1;
            printf("ID: %s\nName: %s\nCoordinates: (%.2lf, %.2lf)\nClue: %s\nValue: %d\n\n",
                t.id, t.name, t.lat, t.lng, t.clue, t.val);
            break;
        }
    }
    if (!found) {
        printf("There is no treasure with that id!");
        exit(-1);
    }
    closeFile(fd);
    closeDir(dir);
    char message[max];
    sprintf(message,"--view %s %s\n",huntId,treasureId);
    writeInInternalLogFile(huntId,"logged_hunt.bin",message);
}

//functie pentru comanda "--remove_treasure <huntId> <treasureId>"
void removeTreasure(char* huntId, char* treasureId) {
    DIR* dir = opendir(huntId);
    if (!dir) {
        perror("Couldn't open directory!");
        exit(-1);
    }
    char pathTemp[100];
    sprintf(pathTemp, "%s/%s", huntId, "temp.bin");
    int fdTemp=open(pathTemp,O_RDWR|O_CREAT,0777);
    char path[100];
    sprintf(path, "%s/%s", huntId, "treasures.bin");
    int fd = open(path, O_RDONLY);
    if (fd == -1 || fdTemp==-1) {
        perror("Couldn't open file!");
        exit(-1);
    }
    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (strcmp(t.id, treasureId) != 0) {
           ssize_t written = write(fdTemp,&t,sizeof(t));
           if(written!=sizeof(t)){
           perror(NULL);
           exit(-1);
        }
        }
    }
    closeFile(fd);
    closeFile(fdTemp);
    unlink(path);
    rename(pathTemp,path);
    closeDir(dir);
    char message[max];
    sprintf(message,"--remove_treasure %s %s\n",huntId,treasureId);
    writeInInternalLogFile(huntId,"logged_hunt.bin",message);
}


void removeSymbolicLog(char *huntId){
	char name[100]={0};
	sprintf(name,"logged_hunt_%s.bin",huntId);
	unlink(name);
}
//functie pentru comanda "--remove_hunt <huntId>"
void removeHunt(char* path) {
    DIR* dir = opendir(path);
    if (!dir) {
        perror("Couldn't open directory!");
        exit(-1);
    }
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char newPath[1000];
        sprintf(newPath, "%s/%s", path, entry->d_name);
        if (isDirectory(newPath)) {
            removeHunt(newPath);
        }
        else {
            unlink(newPath);
        }
    }
    rmdir(path);

}


