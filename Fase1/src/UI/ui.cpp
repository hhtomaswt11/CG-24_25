#include "../../include/UI/ui.h"

#include <vector>
#include <map>
#include <stdio.h> 

void helper(){
    printf ("\t{-- MENU HELPER --}\n"); 
    printf ("PLANE [LENGTH] [DIVISIONS] plane.3d\n"); 
    printf ("BOX [LENGTH] [DIVISIONS PER SIDE] box.3d\n"); 
    printf ("SPHERE [RADIUS] [SLICES] [STACKS] sphere.3d\n"); 
    printf ("CONE [BOTTOM RADIUS] [HEIGHT] [SLICES] [STACKS] cone.3d\n"); 
}

void printSuccessMessage(int argc, char *argv[]) {
    printf("Executed successfully: %s ", argv[1]); 
    for (int i = 2; i < argc; i++) {
        printf("%s ", argv[i]);          
    }
    printf("\n");
}

