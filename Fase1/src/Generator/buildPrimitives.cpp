#include "../../include/Generator/buildPrimitives.h"


Primitive buildPlane(int length, int divisions, char axis = 'Y', float h = 0.0f, int invertFaces = 0) {
    Primitive plano = newEmptyPrimitive();
    if (!plano) return plano;

    std::vector<Point> uniquePoints; // armazena os pontos unicos 

    std::map<std::tuple<float, float, float>, int> pointIndexMap; // mapeia as coordenadas para um indice de modo a evitar pontos duplicados
    // mapa: tuplo (coordenadas x y z ) -> indice do ponto no vetor uniquePoints 


    std::vector<int> indices; // armazena os indices que definem a ordem dos vertices para formar triangulos 

    //  plane 4 4 
    float half = (float)length / 2; // metade do comprimento do plano para centraliza lo  2 
    float div_side = (float)length / divisions; // tamanho de cada subdivisao na malha  2 

    // Gerar pontos únicos e armazenar índices corretos
    for (int linha = 0; linha <= divisions; ++linha) {
        for (int coluna = 0; coluna <= divisions; ++coluna) {
            float x = -half + coluna * div_side;
            float z = -half + linha * div_side;

            Point p;
            if (axis == 'X')
                p = newPoint(h, x, z);
            else if (axis == 'Y') // padrao 
                p = newPoint(x, h, z);
            else if (axis == 'Z')
                p = newPoint(x, z, h);

            std::tuple<float, float, float> key = std::make_tuple(getX(p), getY(p), getZ(p));

            // Verifica se o ponto já existe no mapa
            if (pointIndexMap.find(key) == pointIndexMap.end()) { // verifica se o ponto existe no mapa pointIndexMap (que armazena pontos unicos)
                // se o ponto nao existir, adicionamos ao mapa e tambem ao vetor uniquePoints
                pointIndexMap[key] = uniquePoints.size(); // adiciona o ponto ao pointIndexMap. O valor armazenado é o indice do ponto no vetor uniquePoints 
                
                uniquePoints.push_back(p);
                std::cout << "New point: (" << getX(p) << ", " << getY(p) << ", " << getZ(p) << ")" << std::endl;
            } 
        }
    }

    // std::cout << "Loading indexes for plane:" << std::endl;
    for (int linha = 0; linha < divisions; ++linha) {
        for (int coluna = 0; coluna < divisions; ++coluna) {   // (divisions + 1) = número de vertices por linha -> para considerar todos os vertices das linhas anteriores 
            int i1 = linha * (divisions + 1) + coluna;         // Vértice inferior esquerdo
            int i2 = linha * (divisions + 1) + coluna + 1;     // Vértice inferior direito
            int i3 = (linha + 1) * (divisions + 1) + coluna;   // Vértice superior esquerdo
            int i4 = (linha + 1) * (divisions + 1) + coluna + 1; // Vértice superior direito

            if (invertFaces) {
                // Inverte a ordem dos vértices para CW (para faces opostas)
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

            //std::cout << "Triângulo 1: " << i1 << ", " << i2 << ", " << i3 << std::endl;
            //std::cout << "Triângulo 2: " << i2 << ", " << i4 << ", " << i3 << std::endl;

            // Usar índices economiza memória porque reaproveita vértices.
            // É a forma mais eficiente de desenhar superfícies 3D em OpenGL 
        }
    }

    for (const auto& p : uniquePoints) {
        addPoint(plano, p);
        }

    setIndices(plano, indices);

    // // debug 
    // for (std::vector<int>::size_type i = 0; i < indices.size(); i += 3) {
    // std::cout << "Triangle connection: " 
    // << indices[i] << ", " 
    // << indices[i + 1] << ", " 
    // << indices[i + 2] << std::endl;
    // }


    return plano;
}




Primitive buildBox(int length, int divisions) {
    Primitive box = newEmptyPrimitive();
    if (!box) return box;

    float half = (float)length / 2;
    char 
        x = 'X',
        y = 'Y',
        z = 'Z'; 

    // Lista temporária para armazenar todos os pontos e índices
    std::vector<Point> allPoints; // armazena todos os pontos unicos 
    std::vector<int> allIndices; // define a ordem dos vertices para formar triangulos em todas as faces do cubo 

    // Gerar as 6 faces do cubo corretamente
    std::cout << "Gerando faces para o cubo:" << std::endl;

    // Face Superior (+Y)
    Primitive faceBaixo = buildPlane(length, divisions, y, -half, 0);

    // Face Inferior (-Y), invertida para ficar virada para fora
    Primitive faceCima = buildPlane(length, divisions, y, half, 1);

    // Face Frente (+Z)
    Primitive faceFrente = buildPlane(length, divisions, z, -half, 1);

    // Face Trás (-Z), invertida para ficar virada para fora
    Primitive faceTras = buildPlane(length, divisions, z, half, 0);

    // Face Direita (+X)
    Primitive faceDireita = buildPlane(length, divisions, x, -half, 1);

    // Face Esquerda (-X)
    Primitive faceEsquerda = buildPlane(length, divisions, x, half, 0);

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