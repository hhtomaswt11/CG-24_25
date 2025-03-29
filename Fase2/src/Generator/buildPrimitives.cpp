#include "../../include/Generator/buildPrimitives.h"


Primitive buildPlane(int length, int divisions, char axis = 'Y', float h = 0.0f, bool invertFaces = false, bool invertDiagonal = false) {

    if (length <= 0 || divisions <= 0) {
        std::cerr << "Erro: Parâmetros inválidos para gerar o plano." << std::endl;
        exit(1); 
    }
    Primitive plano = buildPrimitive();
    if (!plano) return plano;

    std::vector<Point> uniquePoints; // armazena os pontos únicos
    std::map<std::tuple<float, float, float>, int> pointIndexMap; // mapeia coordenadas para índices
    std::vector<int> indices; // armazena os índices que definem a ordem dos vértices

    float half = (float)length / 2; 
    float div_side = (float)length / divisions; // tamanho da subdivisao

    // gerar os pontos únicos
    for (int linha = 0; linha <= divisions; ++linha) {
        for (int coluna = 0; coluna <= divisions; ++coluna) {
            float x = -half + coluna * div_side;
            float z = -half + linha * div_side;

            Point p;
            if (axis == 'X')
                p = buildPoint(h, x, z);
            else if (axis == 'Y')
                p = buildPoint(x, h, z);
            else if (axis == 'Z')
                p = buildPoint(x, z, h);

            std::tuple<float, float, float> key = std::make_tuple(getX(p), getY(p), getZ(p));

            // verifica se o ponto já existe no mapa. se nao existir, adicionamos 
            if (pointIndexMap.find(key) == pointIndexMap.end()) {
                pointIndexMap[key] = uniquePoints.size();
                uniquePoints.push_back(p);
            }
        }
    }

    //geracao dos indices 
    for (int linha = 0; linha < divisions; ++linha) {
        for (int coluna = 0; coluna < divisions; ++coluna) {
            int i1 = linha * (divisions + 1) + coluna;         // Vértice inferior esquerdo
            int i2 = linha * (divisions + 1) + coluna + 1;     // Vértice inferior direito
            int i3 = (linha + 1) * (divisions + 1) + coluna;   // Vértice superior esquerdo
            int i4 = (linha + 1) * (divisions + 1) + coluna + 1; // Vértice superior direito

            if (invertDiagonal) {
                // para inverter a diagonal
                if (invertFaces) {
                    // inverte a ordem dos vértices (clockwise)
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
                // para manter a diagonal original
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

    // adicionar pontos e índices à primitiva
    for (const auto& p : uniquePoints) {
        addPoint(plano, p);
    }
    setIndices(plano, indices);

    return plano;
}



Primitive buildBox(int length, int divisions) {
    if (length <=0 || divisions <= 0) {
        std::cerr << "Erro: Parâmetros inválidos para gerar o cubo." << std::endl;
      
        exit(1); 
    }

    Primitive box = buildPrimitive();
    if (!box) return box;

    float half = (float)length / 2;

    // lista temporária para armazenar todos os pontos e índices
    std::vector<Point> allPoints; // armazena todos os pontos únicos
    std::vector<int> allIndices;  // define a ordem dos vértices para formar triângulos


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

        // adicionar pontos ao vetor global
        allPoints.insert(allPoints.end(), pontos.begin(), pontos.end());

        // adicionar índices
        for (auto index : indices) {
            allIndices.push_back(index + offset);
        }
    };

    // adicionar todas as faces ao cubo
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
    
   
    deletePrimitive2(faceCima);
    deletePrimitive2(faceBaixo);
    deletePrimitive2(faceFrente);
    deletePrimitive2(faceTras);
    deletePrimitive2(faceDireita);
    deletePrimitive2(faceEsquerda);

    return box;
}


Primitive buildSphere(int radius, int slices, int stacks) {
    if (radius <= 0 || slices < 3 || stacks < 1) {
        std::cerr << "Erro: Parâmetros inválidos para gerar a esfera." << std::endl;
        exit(1);
    }

    Primitive sphere = buildPrimitive();
    if (!sphere) return sphere;

    // std::cout << "Gerando esfera: raio = " << radius << ", slices = " << slices << ", stacks = " << stacks << std::endl;

    std::vector<Point> points;
    std::vector<int> indices;

    // adicionar polo norte
    Point poleNorth = buildPoint(0, radius, 0);
    points.push_back(poleNorth);
    int northIndex = 0;

    // geracao dos vertices 
    for (int stack = 1; stack < stacks; ++stack) {  // de 1 e vai até < stacks
        float phi = M_PI * stack / stacks; // Latitude
        float y = radius * cos(phi);
        float xy = radius * sin(phi);

        for (int slice = 0; slice < slices; ++slice) {
            float theta = 2 * M_PI * slice / slices; // Longitude
            float x = xy * cos(theta);
            float z = xy * sin(theta);

            Point p = buildPoint(x, y, z);
            points.push_back(p);
            // std::cout << "Ponto: (" << x << ", " << y << ", " << z << ")" << std::endl;
        }
    }

    // adicionar polo sul
    Point poleSouth = buildPoint(0, -radius, 0);
    points.push_back(poleSouth);
    int southIndex = points.size() - 1;


    // gerar índices para os triângulos

    // conectar ao Polo Norte
    for (int slice = 0; slice < slices; ++slice) {
        int nextSlice = (slice + 1) % slices;
        indices.push_back(northIndex);
        indices.push_back(1 + nextSlice);
        indices.push_back(1 + slice);
    }

    // conectar as stacks intermediárias! 
    for (int stack = 0; stack < stacks - 2; ++stack) {
        int currentStackStart = 1 + stack * slices;
        int nextStackStart = currentStackStart + slices;

        for (int slice = 0; slice < slices; ++slice) {
            int nextSlice = (slice + 1) % slices;

            // triângulo inferior
            indices.push_back(currentStackStart + slice);
            indices.push_back(currentStackStart + nextSlice);
            indices.push_back(nextStackStart + slice);

            // triângulo superior
            indices.push_back(currentStackStart + nextSlice);
            indices.push_back(nextStackStart + nextSlice);
            indices.push_back(nextStackStart + slice);
        }
    }

    // conectar à base (Polo Sul)
    int lastStackStart = 1 + (stacks - 2) * slices;
    for (int slice = 0; slice < slices; ++slice) {
        int nextSlice = (slice + 1) % slices;
        indices.push_back(southIndex);
        indices.push_back(lastStackStart + slice);
        indices.push_back(lastStackStart + nextSlice);
    }

  
    for (const auto& p : points) {   // adicionar todos os pontos à primitiva
        addPoint(sphere, p);
    }

    setIndices(sphere, indices);

    //std::cout << "Total de pontos: " << points.size() << std::endl;
    //std::cout << "Total de índices: " << indices.size() << std::endl;

    return sphere;
}


Primitive buildCone(int radius, int height, int slices, int stacks) {
    if (radius <= 0 || height <= 0 || slices < 3 || stacks < 1) {
        std::cerr << "Erro: Parâmetros inválidos para gerar o cone." << std::endl;
        exit(1); 
    }

    Primitive cone = buildPrimitive();
    if (!cone) return cone;

    std::vector<Point> points;
    std::vector<int> indices;

   
    auto addUniquePoint = [&](const Point& p) -> int {  // para verificar se um ponto já existe
        for (size_t i = 0; i < points.size(); ++i) {
            if (getX(points[i]) == getX(p) && getY(points[i]) == getY(p) && getZ(points[i]) == getZ(p)) {
                return i;  // retorna o índice do ponto existente
            }
        }
        points.push_back(p);  // se não encontrar, entao adiciona o ponto
        return points.size() - 1;  // índice do novo ponto
    };


    // base do cone
    Point center = buildPoint(0.0f, 0.0f, 0.0f);
    int centerIndex = addUniquePoint(center);  // adicionar o centro da base -> ponto único

    std::vector<int> baseIndices;
    for (int i = 0; i < slices; i++) {
        float angle = 2.0f * M_PI * i / slices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        Point p = buildPoint(x, 0.0f, z);
        int index = addUniquePoint(p); // adiciona vértice da borda e verifica se é único! 
        baseIndices.push_back(index); // armazena o índice do ponto da borda
    }

  
    for (int i = 0; i < slices; i++) {   // adicionar os triângulos para a base
        int next = (i + 1) % slices;
        indices.push_back(centerIndex);
        indices.push_back(baseIndices[i]);
        indices.push_back(baseIndices[next]);
    }

    // criar corpo do cone -> sem duplicar vértices da base
    std::vector<std::vector<int>> stackIndices(stacks);

     
    for (int stack = 0; stack < stacks; ++stack) { // gerar vértices para as camadas do corpo
        float currHeight = (float)stack / stacks * height;
        float currRadius = radius * (1.0f - (float)stack / stacks);

        for (int slice = 0; slice < slices; ++slice) {
            float theta = 2.0f * M_PI * slice / slices;
            float x = currRadius * cos(theta);
            float z = currRadius * sin(theta);

            Point p = buildPoint(x, currHeight, z);
            int index = addUniquePoint(p); // adicionar ponto do corpo, verificando se é único
            stackIndices[stack].push_back(index); // para armazenar o índice do ponto da camada
        }
    }

   
    for (int stack = 0; stack < stacks - 1; ++stack) {  // conectar as camadas do corpo
        for (int slice = 0; slice < slices; ++slice) {
            int nextSlice = (slice + 1) % slices;
            int i1 = stackIndices[stack][slice];
            int i2 = stackIndices[stack][nextSlice];
            int i3 = stackIndices[stack + 1][slice];
            int i4 = stackIndices[stack + 1][nextSlice];

            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i4);

            indices.push_back(i1);
            indices.push_back(i4);
            indices.push_back(i2);
        }
    }

    // criar topo do cone -> um único vértice no topo 
    Point top = buildPoint(0.0f, height, 0.0f);
    int topIndex = addUniquePoint(top); // adicionar o topo -> único ponto no topo  (garantir que usamos apenas os pontos minimos necessarios para a construcao)

   
    for (int slice = 0; slice < slices; ++slice) {  // conectar topo com a última camada -> sem duplicar vértices
        int curr = stackIndices[stacks - 1][slice]; // vértice do corpo na última camada
        int next = stackIndices[stacks - 1][(slice + 1) % slices]; // próximo vértice na última camada
        indices.push_back(topIndex);
        indices.push_back(next);
        indices.push_back(curr);
    }

    for (const auto& p : points) {
        addPoint(cone, p);
    }

    // definir os índices 
    setIndices(cone, indices);

    return cone;
}


Primitive buildSaturnRing(float innerRadius, float outerRadius, float height, int slices, int stacks) {
    if (innerRadius <= 0 || outerRadius <= innerRadius || slices < 3 || stacks < 1 || height < 0) {
        std::cerr << "Erro: Parâmetros inválidos para gerar o anel." << std::endl;
        exit(1); 
    }

    Primitive ring = buildPrimitive();
    if (!ring) return ring;

    std::vector<Point> points;
    std::vector<int> indices;

    auto addUniquePoint = [&](const Point& p) -> int {
        for (size_t i = 0; i < points.size(); ++i) {
            if (getX(points[i]) == getX(p) && getY(points[i]) == getY(p) && getZ(points[i]) == getZ(p)) {
                return i;
            }
        }
        points.push_back(p);
        return points.size() - 1;
    };

    float radiusStep = (outerRadius - innerRadius) / stacks;

    std::vector<std::vector<int>> stackIndices(stacks + 1);

    for (int stack = 0; stack <= stacks; ++stack) {
        float currRadius = innerRadius + stack * radiusStep;

        // pontos para as duas faces (acima e abaixo) de cada camada
        for (int slice = 0; slice < slices; ++slice) {
            float theta = 2.0f * M_PI * slice / slices;
            float x = currRadius * cos(theta);
            float z = currRadius * sin(theta);

            //  pontos para a face inferior (-height/2)
            Point pBottom = buildPoint(x, -height / 2, z);
            int indexBottom = addUniquePoint(pBottom);
            stackIndices[stack].push_back(indexBottom);

            //  pontos para a face superior (+height/2)
            Point pTop = buildPoint(x, height / 2, z);
            int indexTop = addUniquePoint(pTop);
            stackIndices[stack].push_back(indexTop);
        }
    }

    // índices para os triângulos que formam a superfície do anel
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int nextSlice = (slice + 1) % slices;

            // face superior do anel 
            int i1Top = stackIndices[stack][2 * slice + 1];
            int i2Top = stackIndices[stack][2 * nextSlice + 1];
            int i3Top = stackIndices[stack + 1][2 * slice + 1];
            int i4Top = stackIndices[stack + 1][2 * nextSlice + 1];

            // triangulos da face superior (top-facing surface)
            indices.push_back(i1Top);
            indices.push_back(i2Top);
            indices.push_back(i3Top);

            indices.push_back(i3Top);
            indices.push_back(i2Top);
            indices.push_back(i4Top);

            // indices para a face inferior do anel
            int i1Bottom = stackIndices[stack][2 * slice];
            int i2Bottom = stackIndices[stack][2 * nextSlice];
            int i3Bottom = stackIndices[stack + 1][2 * slice];
            int i4Bottom = stackIndices[stack + 1][2 * nextSlice];

            // triangulos da face inferior (bottom-facing surface)
            indices.push_back(i3Bottom);
            indices.push_back(i2Bottom);
            indices.push_back(i1Bottom);

            indices.push_back(i4Bottom);
            indices.push_back(i2Bottom);
            indices.push_back(i3Bottom);

            // triangulos laterais para as faces do anel (lado externo)
            indices.push_back(i1Top);
            indices.push_back(i3Bottom);
            indices.push_back(i1Bottom);

            indices.push_back(i1Top);
            indices.push_back(i2Top);
            indices.push_back(i3Bottom);

            indices.push_back(i3Top);
            indices.push_back(i4Bottom);
            indices.push_back(i3Bottom);

            indices.push_back(i3Top);
            indices.push_back(i2Top);
            indices.push_back(i4Bottom);
        }
    }

   
    for (const auto& p : points) {
        addPoint(ring, p);
    }

    
    setIndices(ring, indices);

    return ring;
}
