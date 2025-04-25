#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hunt.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Not enough arguments!");
         for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    exit(-1);
    }
    char* op = argv[1];
    if (argc == 3) {
        if (strcmp(op, "--add") == 0)
	        addHunt(argv[2]);
        else if (strcmp(op, "--list") == 0){
            if(strcmp(argv[2],"--all")==0) listAllHunts();
	        else listHunt(argv[2]);
        }
        else if (strcmp(op, "--remove_hunt") == 0){
            removeHunt(argv[2]);
            removeSymbolicLog(argv[2]);
        }
    }
    if (argc == 4) {
        if (strcmp(op, "--view") == 0)
            viewTreasure(argv[2], argv[3]);
        else if (strcmp(op, "--remove_treasure") == 0)
            removeTreasure(argv[2], argv[3]);
    }
    return 0;
}
