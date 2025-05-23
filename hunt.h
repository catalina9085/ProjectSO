#pragma once
#define max 1000

typedef struct {
	char id[max];
	char name[max];
	double lat, lng;
	char clue[max];
	int val;
}Treasure;

Treasure** readTreasures(int* size,char *dirName);
int isDirectory(char* dirName);
void addHunt(char* dirName);
void printTreasures(char* dirName, char* fileName);
void listHunt(char* dirName);
void viewTreasure(char* hundId, char* treasureId);
void removeTreasure(char* hundId, char* treasureId);
void removeHunt(char* huntId);
void writeInInternalLogFile(char* dirName, char* internalLogName, char *message);
void removeSymbolicLog(char *huntID);
void listAllHunts();
