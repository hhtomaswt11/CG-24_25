#include "../../include/Generator/generator.h"
#include <vector>
#include <map>


void helper(){
    printf ("\t{-- MENU HELPER --}\n"); 
    printf ("PLANE [LENGTH] [DIVISIONS] plane.3d\n"); 
    printf ("BOX [LENGTH] [DIVISIONS PER SIDE] box.3d\n"); 
    printf ("SPHERE [RADIUS] [SLICES] [STACKS] sphere.3d\n"); 
    printf ("CONE [BOTTOM RADIUS] [HEIGHT] [SLICES] [STACKS] cone.3d\n"); 
}




int main(int argc, char* argv[]) {

    if (strcmp(argv[1], "--help") == 0 ){
       helper(); 
    }

    if(argc < 2)  {
        std::cout << "Not enough arguments";
        return EXIT_FAILURE;
    }
    Primitive primitive;
    const char* path_3d;

    if (strcmp(argv[1] , "plane") == 0) {
        path_3d = argv[4];
        // std::cout << "Path 3D: " << path_3d << std::endl;

        primitive = buildPlane(atoi(argv[2]), atoi(argv[3]), 'Y', 0 , 1); // tentar por o .3d a abrir no Models
        // std::cout << "DEBUG: Saving file to -> " << path_3d << std::endl; 
        primitiveToFile(primitive, path_3d);
        deletePrimitive(primitive);
    }
    else if(strcmp(argv[1], "box") == 0){
        path_3d = argv[4];
       primitive = buildBox(atoi(argv[2]), atoi(argv[3])); // tentar por o .3d a abrir no Models 
       std::cout << "DEBUG: Saving file to -> " << path_3d << std::endl;
       primitiveToFile(primitive, path_3d);
       deletePrimitive(primitive);
    }
}






