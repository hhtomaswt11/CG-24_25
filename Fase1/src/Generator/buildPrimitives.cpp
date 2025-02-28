#include "../../include/Generator/buildPrimitives.h"


Primitive buildPlane(int length, int divisions, char axis = 'Y', float h = 0.0f, bool invertFaces = false, bool invertDiagonal = false) {
    Primitive plano = newEmptyPrimitive();
    if (!plano) return plano;

    std::vector<Point> uniquePoints; // Armazena os pontos únicos
    std::map<std::tuple<float, float, float>, int> pointIndexMap; // Mapeia coordenadas para índices
    std::vector<int> indices; // Armazena os índices que definem a ordem dos vértices

    float half = (float)length / 2; // Metade do comprimento do plano
    float div_side = (float)length / divisions; // Tamanho de cada subdivisão

    // Gerar pontos únicos
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
            }
        }
    }

    // Gerar índices
    for (int linha = 0; linha < divisions; ++linha) {
        for (int coluna = 0; coluna < divisions; ++coluna) {
            int i1 = linha * (divisions + 1) + coluna;         // Vértice inferior esquerdo
            int i2 = linha * (divisions + 1) + coluna + 1;     // Vértice inferior direito
            int i3 = (linha + 1) * (divisions + 1) + coluna;   // Vértice superior esquerdo
            int i4 = (linha + 1) * (divisions + 1) + coluna + 1; // Vértice superior direito

            if (invertDiagonal) {
                // Inverte a diagonal
                if (invertFaces) {
                    // Inverte a ordem dos vértices (clockwise)
                    indices.push_back(i1); // Inferior Esquerda
                    indices.push_back(i4); // Superior Direita
                    indices.push_back(i2); // Inferior Direita

                    indices.push_back(i1); // Inferior Esquerda
                    indices.push_back(i3); // Superior Esquerda
                    indices.push_back(i4); // Superior Direita
                } else {
                    // Ordem normal (counter-clockwise)
                    indices.push_back(i1); // Inferior Esquerda
                    indices.push_back(i2); // Inferior Direita
                    indices.push_back(i4); // Superior Direita

                    indices.push_back(i1); // Inferior Esquerda
                    indices.push_back(i4); // Superior Direita
                    indices.push_back(i3); // Superior Esquerda
                }
            } else {
                // Mantém a diagonal original
                if (invertFaces) {
                    // Inverte a ordem dos vértices (clockwise)
                    indices.push_back(i1); // Inferior Esquerda
                    indices.push_back(i3); // Superior Esquerda
                    indices.push_back(i2); // Inferior Direita

                    indices.push_back(i2); // Inferior Direita
                    indices.push_back(i3); // Superior Esquerda
                    indices.push_back(i4); // Superior Direita
                } else {
                    // Ordem normal (counter-clockwise)
                    indices.push_back(i1); // Inferior Esquerda
                    indices.push_back(i2); // Inferior Direita
                    indices.push_back(i3); // Superior Esquerda

                    indices.push_back(i2); // Inferior Direita
                    indices.push_back(i4); // Superior Direita
                    indices.push_back(i3); // Superior Esquerda
                }
            }
        }
    }

    // Adicionar pontos e índices à primitiva
    for (const auto& p : uniquePoints) {
        addPoint(plano, p);
    }
    setIndices(plano, indices);

    return plano;
}



Primitive buildBox(int length, int divisions) {
    Primitive box = newEmptyPrimitive();
    if (!box) return box;

    float half = (float)length / 2;

    // Lista temporária para armazenar todos os pontos e índices
    std::vector<Point> allPoints; // Armazena todos os pontos únicos
    std::vector<int> allIndices;  // Define a ordem dos vértices para formar triângulos

    // Gerar as 6 faces do cubo
    std::cout << "Building box faces:" << std::endl;

    // Face Superior (+Y)
    Primitive faceCima = buildPlane(length, divisions, 'Y', half, true, true);

    // Face Inferior (-Y)
    Primitive faceBaixo = buildPlane(length, divisions, 'Y', -half, false, false);

    // Face Frente (+Z)
    Primitive faceFrente = buildPlane(length, divisions, 'Z', -half, true, false);

    // Face Trás (-Z)
    Primitive faceTras = buildPlane(length, divisions, 'Z', half, false, true);

    // Face Direita (+X)
    Primitive faceDireita = buildPlane(length, divisions, 'X', -half, true, false);

    // Face Esquerda (-X)
    Primitive faceEsquerda = buildPlane(length, divisions, 'X', half, false, true);

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

    // Adicionar todas as faces ao cubo
    addFaceToBox(faceCima);
    addFaceToBox(faceBaixo);
    addFaceToBox(faceFrente);
    addFaceToBox(faceTras);
    addFaceToBox(faceDireita);
    addFaceToBox(faceEsquerda);

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



Primitive buildSphere(int radius, int slices, int stacks) {
    Primitive sphere = newEmptyPrimitive();
    if (!sphere) return sphere;

    std::cout << "Gerando esfera: raio = " << radius << ", slices = " << slices << ", stacks = " << stacks << std::endl;

    std::vector<Point> points;
    std::vector<int> indices;

    // Gerar os vértices da esfera
    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = M_PI * stack / stacks;
        for (int slice = 0; slice <= slices; ++slice) {
            float theta = 2 * M_PI * slice / slices;
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            Point p = newPoint(x, y, z);
            points.push_back(p);
            std::cout << "Ponto: (" << x << ", " << y << ", " << z << ")" << std::endl;
        }
    }

    // Gerar os índices para os triângulos
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int current = stack * (slices + 1) + slice;
            int next = current + slices + 1;

            // Primeiro triângulo
            indices.push_back(current);
            indices.push_back(current + 1);
            indices.push_back(next);

            // Segundo triângulo
            indices.push_back(current + 1);
            indices.push_back(next + 1);
            indices.push_back(next);

            std::cout << "Triângulos: " 
                      << current << ", " << (current + 1) << ", " << next << " e "
                      << (current + 1) << ", " << (next + 1) << ", " << next << std::endl;
        }
    }

    // Adicionar todos os pontos à primitiva
    for (const auto& p : points) {
        addPoint(sphere, p);
    }

    // Configurar os índices
    setIndices(sphere, indices);

    std::cout << "Total de pontos: " << points.size() << std::endl;
    std::cout << "Total de índices: " << indices.size() << std::endl;

    return sphere;
}