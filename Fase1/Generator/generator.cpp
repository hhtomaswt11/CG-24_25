#include "generator.h"

Primitive buildPlane(int length, int divisions, char axis = 'Y', float h = 0.0f, int invert = 0) {
    Primitive plano = newEmptyPrimitive();
    if (!plano) return plano; // Retorna se houve erro
    // length = 4
    // div = 1 

    float half = (float) length / 2; // 2  -> 1               // 2 -> 1 
    float div_side = (float) length / divisions; // 4 -> 2    // 2 -> 1 

    float x1,x2,x3,x4,z1,z2,z3,z4; 


     x1 = -half, z1 = -half; // -1 -1 
     x2 = -half, z2 = -half + div_side; // -1 -1+2 -> -1 1
     x3 = -half + div_side, z3 = -half; // -1 + 2 , -1 -> 1 , -1 
     x4 = -half + div_side, z4 = -half + div_side; // 1 1 


    // float arrx[4] = {x1, x2, x3, x4}, arrz[4] = {z1, z2, z3, z4};

    // if (invert) { // Se "invert" for 1, inverte os índices dos triângulos
    //     arrx[1] = x3;
    //     arrx[2] = x2;
    //     arrz[1] = z3;
    //     arrz[2] = z2;
    // }

    for (int linha = 0; linha < divisions; ++linha) {
        for (int coluna = 0; coluna < divisions; ++coluna) {
            float xStart = -half + coluna * div_side;
            float zStart = -half + linha * div_side;
            
            // Definindo os 4 vértices do quadrado atual
            x1 = xStart;
            z1 = zStart;
            
            x2 = xStart;
            z2 = zStart + div_side;
            
            x3 = xStart + div_side;
            z3 = zStart;
            
            x4 = xStart + div_side;
            z4 = zStart + div_side;
            
            // Armazenando os valores nos arrays
            float arrx[4] = {x1, x2, x3, x4};
            float arrz[4] = {z1, z2, z3, z4};
    
            if (invert) { // Se "invert" for 1, inverte os índices dos triângulos
                arrx[1] = x3;
                arrx[2] = x2;
                arrz[1] = z3;
                arrz[2] = z2;
            }
    
            // Adicionando os pontos para o plano no eixo correto
            if (axis == 'X') {
                addPoint(plano, newPoint(h, arrx[0], arrz[0]));
                addPoint(plano, newPoint(h, arrx[1], arrz[1]));
                addPoint(plano, newPoint(h, arrx[2], arrz[2]));
    
                addPoint(plano, newPoint(h, arrx[1], arrz[1]));
                addPoint(plano, newPoint(h, arrx[3], arrz[3]));
                addPoint(plano, newPoint(h, arrx[2], arrz[2]));
            } 
            else if (axis == 'Y') {
                addPoint(plano, newPoint(arrx[0], h, arrz[0]));
                addPoint(plano, newPoint(arrx[1], h, arrz[1]));
                addPoint(plano, newPoint(arrx[2], h, arrz[2]));
    
                addPoint(plano, newPoint(arrx[1], h, arrz[1]));
                addPoint(plano, newPoint(arrx[3], h, arrz[3]));
                addPoint(plano, newPoint(arrx[2], h, arrz[2]));
            } 
            else if (axis == 'Z') {
                addPoint(plano, newPoint(arrx[0], arrz[0], h));
                addPoint(plano, newPoint(arrx[1], arrz[1], h));
                addPoint(plano, newPoint(arrx[2], arrz[2], h));
    
                addPoint(plano, newPoint(arrx[1], arrz[1], h));
                addPoint(plano, newPoint(arrx[3], arrz[3], h));
                addPoint(plano, newPoint(arrx[2], arrz[2], h));
            }
        }
    }
    

    return plano;
}


Primitive buildBox(int length, int divisions){
    Primitive box = newEmptyPrimitive();
    if (box){// se deu NULL, é porque houve erros
        float dimension2 = (float)length / 2;
        char 
         x = 'X',
         y = 'Y',
         z = 'Z'; 

        // Gerar cada face da caixa usando a função genérica buildPlane
        Primitive faceCima    = buildPlane(length, divisions, y, dimension2, 0),
                  faceBaixo   = buildPlane(length, divisions, y, -dimension2, 1),

                  faceLateral1 = buildPlane(length, divisions, x , -dimension2, 0), // as de lado
                  faceLateral2 = buildPlane(length, divisions, x ,  dimension2, 1),

                  faceLateral3 = buildPlane(length, divisions, z, -dimension2, 0), // as de frente e tras 
                  faceLateral4 = buildPlane(length, divisions, z,  dimension2, 1);

        // Adicionar todas as faces ao objeto "box"
        addPrimitive(box, faceCima);
        addPrimitive(box, faceBaixo);

        addPrimitive(box, faceLateral1);
        addPrimitive(box, faceLateral2);
        addPrimitive(box, faceLateral3);
        addPrimitive(box, faceLateral4);

        addPoints(box, faceCima);  // face de baixo do cubo
        addPoints(box, faceBaixo); // face de cima do cubo
        addPoints(box, faceLateral1);
        addPoints(box, faceLateral2);
        addPoints(box, faceLateral3);
        addPoints(box, faceLateral4);
        deletePrimitive2(faceCima);
        deletePrimitive2(faceBaixo);
        deletePrimitive2(faceLateral1);
        deletePrimitive2(faceLateral2);
        deletePrimitive2(faceLateral3);
        deletePrimitive2(faceLateral4);
    }
    return box;
}

int main(int argc, char* argv[]) {

    if(argc < 2)  {
        std::cout << "Not enough arguments";
        return EXIT_FAILURE;
    }
    Primitive primitive;
    const char* path_3d;

    if (strcmp(argv[1] , "plane") == 0) {
        path_3d = argv[4];
         std::cout << "Path 3D: " << path_3d << std::endl;

        primitive = buildPlane(atoi(argv[2]), atoi(argv[3]), 'Y', 0,0); // tentar por o .3d a abrir no Models
        std::cout << "DEBUG: Saving file to -> " << path_3d << std::endl; 
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






