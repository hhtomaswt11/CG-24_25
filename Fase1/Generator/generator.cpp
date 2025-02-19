#include "generator.h"

Primitive buildPlane(int length, int divisions, char axis = 'Y', float h = 0.0f, int invert = 0) {
    Primitive plano = newEmptyPrimitive();
    if (!plano) return plano; // Retorna se houve erro

    float coordinate2 = (float)length / 2;
    float div_side = (float)length / divisions;

    float x1 = -coordinate2, z1 = -coordinate2;
    float x2 = -coordinate2, z2 = -coordinate2 + div_side;
    float x3 = -coordinate2 + div_side, z3 = -coordinate2;
    float x4 = -coordinate2 + div_side, z4 = -coordinate2 + div_side;

    float arrx[4] = {x1, x2, x3, x4};
    float arrz[4] = {z1, z2, z3, z4};

    if (invert) { // Se "invert" for 1, inverte os índices dos triângulos
        arrx[1] = x3;
        arrx[2] = x2;
        arrz[1] = z3;
        arrz[2] = z2;
    }

    for (int linha = 0; linha < divisions; ++linha) {
        for (int coluna = 0, step = 0; coluna < divisions; ++coluna, step += div_side) {
            float xOffset = coluna * div_side;

            if (axis == 'X') {  // Plano YZ
                addPoint(plano, newPoint(h, arrx[0] + xOffset, arrz[0] + step));
                addPoint(plano, newPoint(h, arrx[1] + xOffset, arrz[1] + step));
                addPoint(plano, newPoint(h, arrx[2] + xOffset, arrz[2] + step));

                addPoint(plano, newPoint(h, arrx[1] + xOffset, arrz[1] + step));
                addPoint(plano, newPoint(h, arrx[3] + xOffset, arrz[3] + step));
                addPoint(plano, newPoint(h, arrx[2] + xOffset, arrz[2] + step));
            } 
            else if (axis == 'Y') { // Plano XZ
                addPoint(plano, newPoint(arrx[0] + xOffset, h, arrz[0] + step));
                addPoint(plano, newPoint(arrx[1] + xOffset, h, arrz[1] + step));
                addPoint(plano, newPoint(arrx[2] + xOffset, h, arrz[2] + step));

                addPoint(plano, newPoint(arrx[1] + xOffset, h, arrz[1] + step));
                addPoint(plano, newPoint(arrx[3] + xOffset, h, arrz[3] + step));
                addPoint(plano, newPoint(arrx[2] + xOffset, h, arrz[2] + step));
            } 
            else if (axis == 'Z') { // Plano XY
                addPoint(plano, newPoint(arrx[0] + xOffset, arrz[0] + step, h));
                addPoint(plano, newPoint(arrx[1] + xOffset, arrz[1] + step, h));
                addPoint(plano, newPoint(arrx[2] + xOffset, arrz[2] + step, h));

                addPoint(plano, newPoint(arrx[1] + xOffset, arrz[1] + step, h));
                addPoint(plano, newPoint(arrx[3] + xOffset, arrz[3] + step, h));
                addPoint(plano, newPoint(arrx[2] + xOffset, arrz[2] + step, h));
            }
        }

        // atualizar coordenadas Z de forma modular
        for (float& z : arrz) {
            z += div_side;
        }
    }

    return plano;
}



Primitive buildBox(int length, int divisions){
    Primitive box = newEmptyPrimitive();
    if (box){// se deu NULL, é porque houve erros
        float dimension2 = (float)length / 2;

        // Gerar cada face da caixa usando a função genérica buildPlane
        Primitive faceCima    = buildPlane(length, divisions, 'Y',  dimension2, 0),
                  faceBaixo   = buildPlane(length, divisions, 'Y', -dimension2, 1),

                  faceLateral1 = buildPlane(length, divisions, 'X', -dimension2, 0), // as de lado
                  faceLateral2 = buildPlane(length, divisions, 'X',  dimension2, 1),

                  faceLateral3 = buildPlane(length, divisions, 'Z', -dimension2, 0), // as de frente e tras 
                  faceLateral4 = buildPlane(length, divisions, 'Z',  dimension2, 1);

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






