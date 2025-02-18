#include "generator.h"


Primitive buildPlane(int length, int divisions, float h = 0.0f, int y = 0){
    Primitive plano = newEmptyPrimitive();
    if (plano){
        float coordinate2 = (float)length / 2, div_side = (float)length / divisions,
              x1 = -coordinate2, z1 = -coordinate2,
              x2 = -coordinate2, z2 = -coordinate2 + div_side,
              x3 = -coordinate2 + div_side, z3 = -coordinate2,
              x4 = -coordinate2 + div_side, z4 = -coordinate2 + div_side;
        float arrx[4] = {x1, x2, x3, x4};
        float arrz[4] = {z1, z2, z3, z4};

        if (y == 1){
            arrx[1] = x3;
            arrx[2] = x2;
            arrz[1] = z3;
            arrz[2] = z2;
        }

        for (int linha = 0; linha < divisions; linha++){
            for (int coluna = 0; coluna < divisions; coluna++){
                // Primeiro triângulo do quadrado
                addPoint(plano, newPoint(arrx[0] + coluna * div_side, h, arrz[0]));
                addPoint(plano, newPoint(arrx[1] + coluna * div_side, h, arrz[1]));
                addPoint(plano, newPoint(arrx[2] + coluna * div_side, h, arrz[2]));
                // Segundo triângulo do quadrado
                addPoint(plano, newPoint(arrx[1] + coluna * div_side, h, arrz[1]));
                addPoint(plano, newPoint(arrx[3] + coluna * div_side, h, arrz[3]));
                addPoint(plano, newPoint(arrx[2] + coluna * div_side, h, arrz[2]));
            }
            arrz[0] += div_side;arrz[1] += div_side;arrz[2] += div_side;arrz[3] += div_side;
        }
    }
    return plano;
}

int main(int argc, char* argv[]) {

    if(argc == 1)  {
        std::cout << "Not enough arguments";
        return 1;
    }
    Primitive primitive;
    const char* path_3d;

    if (strcmp(argv[1] , "plane") == 0) {
        path_3d = argv[4];
        primitive = buildPlane(atof(argv[2]), atof(argv[3]), 0,0);
        primitiveToFile(primitive, path_3d);
        deletePrimitive(primitive);
    }
}