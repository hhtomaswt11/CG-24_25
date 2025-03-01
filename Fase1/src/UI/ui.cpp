#include "../../include/UI/ui.h"
#include <vector>
#include <map>
#include <stdio.h> 

const char* RESET = "\033[0m";
const char* RED = "\033[31m";
const char* GREEN = "\033[32m";
const char* YELLOW = "\033[33m";
const char* BLUE = "\033[34m";
const char* MAGENTA = "\033[35m";
const char* CYAN = "\033[36m";
const char* WHITE = "\033[37m";

void helper() {
    printf("%s\t{-- HELPER --}%s\n", CYAN, RESET);
    printf("%sPLANE %s[LENGTH] [DIVISIONS]%s plane.3d\n", RED, YELLOW, MAGENTA);
    printf("%sBOX %s[LENGTH] [DIVISIONS PER SIDE]%s box.3d\n", RED, YELLOW, MAGENTA);
    printf("%sSPHERE %s[RADIUS] [SLICES] [STACKS]%s sphere.3d\n", RED, YELLOW, MAGENTA);
    printf("%sCONE %s[BOTTOM RADIUS] [HEIGHT] [SLICES] [STACKS]%s cone.3d\n", RED, YELLOW, MAGENTA);
}

void printSuccessMessage(int argc, char *argv[]) {
    printf("%sExecuted successfully: %s%s ", GREEN, YELLOW, argv[1]); 
    for (int i = 2; i < argc; i++) {
        printf("%s%s ", YELLOW, argv[i]);          
    }
    printf("%s\n", RESET);
}
