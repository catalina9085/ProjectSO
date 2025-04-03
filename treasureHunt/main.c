#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hunt.h"

int main(int argc, char** argv) {
    if (argc <= 2) {
        printf("Not enough arguments!");
        exit(-1);
    }
    char* op = argv[1];
    if (argc > 2 && strcmp(op, "--add") == 0)
        addDirectory(argv[2]);
    else if (argc > 2 && strcmp(op, "--list") == 0)
        listDirectory(argv[2]);

    return 0;
}