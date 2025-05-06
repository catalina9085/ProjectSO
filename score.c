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
#define max 1000
typedef struct {
	char id[max];
	char name[max];
	double lat, lng;
	char clue[max];
	int val;
}Treasure;
/*
void printTreasures(char* dirName, char* fileName) {
    printf("\n----------------------------------------------------------------------------------------------\n");
    char path[max];
    sprintf(path, "%s/%s", dirName, fileName);
    struct stat myStat;
    if (stat(path, &myStat) == -1) {
        perror("Couldn't read file properties");
        exit(-1);
    }
    int treasureCnt=myStat.st_size/sizeof(Treasure);
    printf("Hunt id:%s\n", dirName);
    printf("File size:%ld\nLast modified:%sNumber or treasures:%d\n\n", myStat.st_size, ctime(&myStat.st_mtime),treasureCnt);
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
    close(fd);

}
*/
int isDirectory(char* dirName) {
    struct stat myStat;
    if (stat(dirName, &myStat) == -1)
        return 0;//nu exista
    if (S_ISDIR(myStat.st_mode)) return 1;
    return 0;
}
void calculateScore(char *dirName){
    int score=0;
    char path[max];
    sprintf(path, "%s/%s", dirName,"treasures.bin");
    int fd = open(path, O_RDONLY, 0777);
    if (fd == -1) {
        perror("Eroare la deschiderea fisierului treasures.bin");
        return 0;
    }
    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        score+=t.val;
        printf("Value:%d\n",t.val);
    }
    printf("%s:%d\n",dirName,score);
    close(fd);
}

int main(int argc,char **argv){
    if(argc>2)
        calculateScore(argv[1]);
    //getScoresForAllHunts();
    return 0;
}