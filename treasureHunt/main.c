#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hunt.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Not enough arguments!");
        exit(-1);
    }
    createLogFile("myLog.txt");
    char* op = argv[1];
    if(strcmp(op,"--print")==0)
	  readLogFile("myLog.txt");
    if (argc > 2) {
        char message[1000] = { 0 };
        sprintf(message, "%s %s\n", op, argv[2]);
        if (strcmp(op, "--add") == 0)
	  addHunt(argv[2],message);
        else if (strcmp(op, "--list") == 0)
	  listHunt(argv[2],message);
        else if (strcmp(op, "--remove_hunt") == 0){
            removeHunt(argv[2]);
	    writeInExternalLogFile(message);
	}
    }
    if (argc > 3) {
        if (strcmp(op, "--view") == 0)
            viewTreasure(argv[2], argv[3]);
        else if (strcmp(op, "--remove_treasure") == 0)
            removeTreasure(argv[2], argv[3]);
        char message[1000] = { 0 };
        sprintf(message, "%s %s %s\n", op, argv[2], argv[3]);
        writeInInternalLogFile(argv[2], "interLog.txt", message);
    }
    return 0;
}
