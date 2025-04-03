#pragma once
#define max 5000

typedef struct {
	char id[max];
	char name[max];
	double lat, lng;
	char clue[max];
	int val;
}Treasure;

Treasure** readTreasures(int* size);
int isDirectory(char* dirName);
void addDirectory(char* dirName);
void printTreasures(char* dirName, char* fileName);
void listDirectory(char* dirName);


