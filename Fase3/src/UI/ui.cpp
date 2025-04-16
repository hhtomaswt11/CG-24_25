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
    printf("%s╔═══════════════════════════════════════════════╗%s\n", CYAN, RESET);
    printf("%s║           -- HELP GENERATOR MENU --           ║%s\n", CYAN, RESET);
    printf("%s╚═══════════════════════════════════════════════╝%s\n", CYAN, RESET);
    printf("%s» %sPLANE    %s[LENGTH] [DIVISIONS]%s        → plane.3d\n", RED, YELLOW, MAGENTA, RESET);
    printf("%s» %sBOX      %s[LENGTH] [DIVISIONS PER SIDE]%s → box.3d\n", RED, YELLOW, MAGENTA, RESET);
    printf("%s» %sSPHERE   %s[RADIUS] [SLICES] [STACKS]%s   → sphere.3d\n", RED, YELLOW, MAGENTA, RESET);
    printf("%s» %sCONE     %s[BOTTOM RADIUS] [HEIGHT] [SLICES] [STACKS]%s → cone.3d\n", RED, YELLOW, MAGENTA, RESET);
    printf("%s» %sRING     %s[INNER RADIUS] [OUTER RADIUS] [HEIGHT][SLICES] [STACKS]%s → ring.3d\n", RED, YELLOW, MAGENTA, RESET);
    printf("%s» %sPATCH    %s[PATCH FILE] [TESSELLATION]%s → bezier.3d\n", RED, YELLOW, MAGENTA, RESET);
    printf("%s══════════════════════════════════════════════════════════════════════════════%s\n", CYAN, RESET);
    // printf("%s║               -- END OF MENU --               ║%s\n", CYAN, RESET);
    // printf("%s╚═══════════════════════════════════════════════╝%s\n", CYAN, RESET);
}


void printSuccessMessage(int argc, char *argv[]) {
    printf("%s╔═══════════════════════════════════════════╗%s\n", GREEN, RESET);
    printf("%s║           EXECUTED SUCCESSFULLY!          ║%s\n", GREEN, RESET);
    printf("%s╚═══════════════════════════════════════════╝%s\n", GREEN, RESET);

    printf("%s» %sCommand:%s %s%s ", GREEN, CYAN, YELLOW, argv[1], RESET);
    
    for (int i = 2; i < argc; i++) {
        printf("%s%s ", YELLOW, argv[i]);
    }
    
    printf("%s\n", RESET);

    printf("%s═════════════════════════════════════════════════════════════%s\n", GREEN, RESET);

}


void helper_engine() {
    printf("%s╔══════════════════════════════════════════════╗%s\n", CYAN, RESET);
    printf("%s║            -- HELP ENGINE MENU --            ║%s\n", CYAN, RESET);
    printf("%s╚══════════════════════════════════════════════╝%s\n", CYAN, RESET);
    printf("%s Make sure that the primitives in the XML file have already been generated.\n", RED);
    printf("%s» %s./engine %s[XML FILE]\n", RED, YELLOW, RESET);
    printf("%s═══════════════════════════════════════════════════════════════════════════════════%s\n", CYAN, RESET);
    // printf("%s║               -- END OF MENU --              ║%s\n", CYAN, RESET);
    // printf("%s╚══════════════════════════════════════════════╝%s\n", CYAN, RESET);
}

