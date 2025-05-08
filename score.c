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
        exit(-1);
    }
    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        score+=t.val;
        printf("Id:%s-Value:%d\n",t.id,t.val);
    }
    printf("Total %s:%d\n",dirName,score);
    close(fd);
}

int main(int argc,char **argv){
    if(argc>1)
        calculateScore(argv[1]);
    else{
        printf("Not enough arguments!");
    }
    //getScoresForAllHunts();
    return 0;
}