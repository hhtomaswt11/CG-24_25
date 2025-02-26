#include "../../include/Generator/generator.h"
#include <vector>
#include <map>

int main(int argc, char* argv[]) {

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






