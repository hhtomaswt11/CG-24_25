#include "../../include/Generator/generator.h"
#include <vector>
#include <map>


Primitive buildPlane(int length, int divisions, char axis = 'Y', float h = 0.0f, int invertFaces = 0) {
    Primitive plano = newEmptyPrimitive();
    if (!plano) return plano;

    std::vector<Point> uniquePoints;
    std::map<std::tuple<float, float, float>, int> pointIndexMap;
    std::vector<int> indices;

    float half = (float)length / 2;
    float div_side = (float)length / divisions;

    // Gerar pontos únicos e armazenar índices corretos
    for (int linha = 0; linha <= divisions; ++linha) {
        for (int coluna = 0; coluna <= divisions; ++coluna) {
            float x = -half + coluna * div_side;
            float z = -half + linha * div_side;

            Point p;
            if (axis == 'X')
                p = newPoint(h, x, z);
            else if (axis == 'Y')
                p = newPoint(x, h, z);
            else if (axis == 'Z')
                p = newPoint(x, z, h);

            std::tuple<float, float, float> key = std::make_tuple(getX(p), getY(p), getZ(p));

            // Verifica se o ponto já existe no mapa
            if (pointIndexMap.find(key) == pointIndexMap.end()) {
                pointIndexMap[key] = uniquePoints.size();
                uniquePoints.push_back(p);
                std::cout << "Ponto gerado: (" << getX(p) << ", " << getY(p) << ", " << getZ(p) << ")" << std::endl;
            }
        }
    }

    std::cout << "Gerando índices para o plano:" << std::endl;
    for (int linha = 0; linha < divisions; ++linha) {
        for (int coluna = 0; coluna < divisions; ++coluna) {
            int i1 = linha * (divisions + 1) + coluna;         // Vértice inferior esquerdo
            int i2 = linha * (divisions + 1) + coluna + 1;     // Vértice inferior direito
            int i3 = (linha + 1) * (divisions + 1) + coluna;   // Vértice superior esquerdo
            int i4 = (linha + 1) * (divisions + 1) + coluna + 1; // Vértice superior direito

            if (invertFaces) {
                // Inverte a ordem dos vértices para CW (útil para faces opostas)
                indices.push_back(i1); // Inferior Esquerda
                indices.push_back(i3); // Superior Esquerda
                indices.push_back(i2); // Inferior Direita

                indices.push_back(i2); // Inferior Direita
                indices.push_back(i3); // Superior Esquerda
                indices.push_back(i4); // Superior Direita
            } else {
                // Ordem normal CCW
                indices.push_back(i1); // Inferior Esquerda
                indices.push_back(i2); // Inferior Direita
                indices.push_back(i3); // Superior Esquerda

                indices.push_back(i2); // Inferior Direita
                indices.push_back(i4); // Superior Direita
                indices.push_back(i3); // Superior Esquerda
            }

            std::cout << "Triângulo 1: " << i1 << ", " << i2 << ", " << i3 << std::endl;
            std::cout << "Triângulo 2: " << i2 << ", " << i4 << ", " << i3 << std::endl;
        }
    }

    for (const auto& p : uniquePoints) {
        addPoint(plano, p);
    }
    setIndices(plano, indices);

    for (int i = 0; i < indices.size(); i += 3) {
        std::cout << "Triângulo final: " 
                  << indices[i] << ", " 
                  << indices[i + 1] << ", " 
                  << indices[i + 2] << std::endl;
    }

    return plano;
}

Primitive buildBox(int length, int divisions) {
    Primitive box = newEmptyPrimitive();
    if (!box) return box;

    float dimension2 = (float)length / 2;
    char 
        x = 'X',
        y = 'Y',
        z = 'Z'; 

    // Lista temporária para armazenar todos os pontos e índices
    std::vector<Point> allPoints;
    std::vector<int> allIndices;

    // Gerar as 6 faces do cubo corretamente
    std::cout << "Gerando faces para o cubo:" << std::endl;

    // Face Superior (+Y)
    Primitive faceBaixo = buildPlane(length, divisions, y, -dimension2, 0);

    // Face Inferior (-Y), invertida para ficar virada para fora
    Primitive faceCima = buildPlane(length, divisions, y, dimension2, 1);

    // Face Frente (+Z)
    Primitive faceFrente = buildPlane(length, divisions, z, -dimension2, 1);

    // Face Trás (-Z), invertida para ficar virada para fora
    Primitive faceTras = buildPlane(length, divisions, z, dimension2, 0);

    // Face Direita (+X)
    Primitive faceDireita = buildPlane(length, divisions, x, -dimension2, 1);

    // Face Esquerda (-X)
    Primitive faceEsquerda = buildPlane(length, divisions, x, dimension2, 0);

    // Adicionar todos os pontos e índices
    auto addFaceToBox = [&](Primitive face) {
        auto pontos = getPoints(face);
        auto indices = getIndices(face);
        int offset = allPoints.size();

        // Adicionar pontos ao vetor global
        allPoints.insert(allPoints.end(), pontos.begin(), pontos.end());

        // Ajustar e adicionar índices
        for (auto index : indices) {
            allIndices.push_back(index + offset);
        }
    };

    addFaceToBox(faceCima);
    addFaceToBox(faceBaixo);
    addFaceToBox(faceFrente);
    addFaceToBox(faceTras);
    addFaceToBox(faceDireita);
    addFaceToBox(faceEsquerda);

    // Configurar os pontos e índices finais na primitiva do cubo
    for (const auto& p : allPoints) {
        addPoint(box, p);
    }
    setIndices(box, allIndices);

    // Limpeza da memória temporária
    deletePrimitive2(faceCima);
    deletePrimitive2(faceBaixo);
    deletePrimitive2(faceFrente);
    deletePrimitive2(faceTras);
    deletePrimitive2(faceDireita);
    deletePrimitive2(faceEsquerda);

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

        primitive = buildPlane(atoi(argv[2]), atoi(argv[3]), 'Y', 0,1); // tentar por o .3d a abrir no Models
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






