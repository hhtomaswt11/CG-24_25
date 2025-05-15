#include "../../include/Generator/generator.h"

#include <vector>
#include <map>

int main(int argc, char* argv[]) {
    if(argc < 2)  {
        std::cout << "Falta de argumentos\n";
        helper(); 
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "--help") == 0 ){
        helper(); 
     }
    Primitive primitive;
    const char* path_3d;
    const char* path_patch; 


    if (strcmp(argv[1] , "plane") == 0) {
        path_3d = argv[4];
        int length = atoi(argv[2]), 
            divisions = atoi(argv[3]); 
            
        primitive = buildPlane(length, divisions, 'Y', 0, true, true); 

        fromPrimitiveTo3dFile(primitive, path_3d);
        deletePrimitive(primitive);
        if (argc == 5 )
        printSuccessMessage(argc, argv); 
        else helper(); 

    }
    else if(strcmp(argv[1], "box") == 0){
       path_3d = argv[4];
       int length = atoi(argv[2]), 
           divisions = atoi(argv[3]); 

       primitive = buildBox(length, divisions);

       fromPrimitiveTo3dFile(primitive, path_3d);
       deletePrimitive(primitive);
       if( argc == 5)
       printSuccessMessage(argc, argv); 
       else helper(); 

    }
    else if(strcmp(argv[1], "sphere") == 0){
        path_3d = argv[5];
        int radius = atoi(argv[2]), 
            slices = atoi(argv[3]) , 
            stacks = atoi(argv[4]); 

        primitive = buildSphere(radius, slices, stacks); 

        fromPrimitiveTo3dFile(primitive, path_3d);
        deletePrimitive(primitive);
        if (argc == 6)
        printSuccessMessage(argc, argv); 
        else helper(); 

    }

    else if (strcmp(argv[1], "cylinder") == 0) {
    path_3d = argv[6];
    int radius = atoi(argv[2]);
    int height = atoi(argv[3]);
    int slices = atoi(argv[4]);
    int stacks = atoi(argv[5]);

    primitive = buildCylinder(radius, height, slices, stacks);

    fromPrimitiveTo3dFile(primitive, path_3d);
    deletePrimitive(primitive);
    if (argc == 7)
        printSuccessMessage(argc, argv);
    else
        helper();
}


     else if(strcmp(argv[1], "cone") == 0){
        path_3d = argv[6];
        int radius = atoi(argv[2]), 
            height = atoi(argv[3]), 
            slices = atoi(argv[4]) , 
            stacks = atoi(argv[5]); 

        primitive = buildCone( radius, height, slices , stacks ) ;

        fromPrimitiveTo3dFile(primitive, path_3d);
        deletePrimitive(primitive);
        if (argc == 7)
        printSuccessMessage(argc, argv); 
        else helper(); 

         }
        

     else if(strcmp(argv[1], "ring") == 0){
        path_3d = argv[6];
        float innerRadius = atof(argv[2]), 
              outerRadius = atof(argv[3]);
        int    slices = atoi(argv[4]) , 
               stacks = atoi(argv[5]); 
 
        primitive = buildSaturnRing(innerRadius, outerRadius, slices ,stacks ) ;

        fromPrimitiveTo3dFile(primitive, path_3d);
        deletePrimitive(primitive);
        printSuccessMessage(argc, argv); 
     }


     else if (strcmp(argv[1], "patch") == 0){
        path_patch = argv[2]; 
        int tessellation = atoi(argv[3]);

        std::string output_file = argv[4];

        buildBezierPatchPrimitive(path_patch, tessellation, output_file); 
        printSuccessMessage(argc, argv); 
    }

    // gcc cometgen.c -o comet_generator -lm
    // ./comet_generator 
    // cd build 
    // cmake .. ; cmake --build . 
    // ./generator patch ../comet.patch 10 comet.3d


}




