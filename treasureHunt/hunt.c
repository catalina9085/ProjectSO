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

void toLowerCaseString(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
    }
}

Treasure** readTreasures(int* size) {
    char id[max];
    char name[max];
    char clue[max];
    double lat, lng;
    int val;
    char agree[10];
    Treasure** treasures = malloc(max * sizeof(Treasure*));
    *size = 0;
    while (1) {
        printf("Want to continue and add a treasure?yes/no ");
        if (scanf("%s", agree) != 1) break;
        toLowerCaseString(agree);
        if (strcmp(agree, "no") == 0 || strcmp(agree,"yes")!=0) break;
        printf("Treasure id(string): ");
        if (scanf("%s", id) != 1) break;
        printf("Username(string): ");
        if (scanf("%s", name) != 1) break;
        printf("Gps coordinates(double double): ");
        if (scanf("%lf %lf", &lat, &lng) != 2) break;
        printf("Clue(string): ");
        if (scanf("%s", clue) != 1) break;
        printf("Treasure value(integer): ");
        if (scanf("%d", &val) != 1) break;
        Treasure* newTreasure = malloc(sizeof(Treasure));
        strcpy(newTreasure->id, id);
        strcpy(newTreasure->name, name);
        strcpy(newTreasure->clue, clue);
        newTreasure->val = val;
        newTreasure->lat = lat;
        newTreasure->lng = lng;
        printf("Original:%s\n", newTreasure->id);
        treasures[(*size)++] = newTreasure;
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

void addDirectory(char* dirName) {
    if (!isDirectory(dirName)) {
        if (mkdir(dirName/*,0777*/) != 0) {
            printf("couldn't create directory");
            exit(-1);
        }
    }
    int size = 0;
    Treasure** treasures = readTreasures(&size);
    char filePath[max];
    sprintf(filePath, "%s/treasures", dirName);
    int fd = open(filePath, O_RDWR | O_CREAT | O_APPEND, 0777);
    if (fd == -1) {
        printf("Couldn't create/open file!");
        exit(-1);
    }
    for (int i = 0; i < size; i++) {
        ssize_t written = write(fd, treasures[i], sizeof(Treasure));
        if (written == -1) {
            printf("Couldn't write in file!");
            close(fd);
            exit(-1);
        }
    }

    close(fd);

}

void printTreasures(char* dirName, char* fileName) {
    printf("\n----------------------------------------------------------------------------------------------\n");
    char path[max];
    sprintf(path, "%s/%s", dirName, fileName);
    struct stat myStat;
    if (stat(path, &myStat) == -1) {
        printf("couldn't read file properties");
        exit(-1);
    }
    printf("File size:%ld\nLast modified:%s\n", myStat.st_size, ctime(&myStat.st_mtime));
    int fd = open(path, O_RDONLY, 0777);
    if (fd == -1) {
        printf("Couldn't open the treasures file!");
        exit(-1);
    }
    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %s\nName: %s\nCoordinates: (%.2lf, %.2lf)\nClue: %s\nValue: %d\n\n",
            t.id, t.name, t.lat, t.lng, t.clue, t.val);
    }

}
void listDirectory(char* dirName) {
    DIR* dir = opendir(dirName);
    if (!dir) {
        printf("couldn't open dir!");
        exit(-1);
    }
    int found = 0;
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, "treasures") == 0) {
            printTreasures(dirName, entry->d_name);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Couldn't find the treasures file in the directory!");
        exit(-1);
    }
    closedir(dir);
}