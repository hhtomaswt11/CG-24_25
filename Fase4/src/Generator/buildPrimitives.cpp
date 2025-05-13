#include "../../include/Generator/buildPrimitives.h"


Primitive buildPlane(int length, int divisions, char axis = 'Y', float h = 0.0f, bool invertFaces = false, bool invertDiagonal = false) {
    if (length <= 0 || divisions <= 0) {
        std::cerr << "Erro: Parâmetros inválidos para gerar o plano." << std::endl;
        exit(1); 
    }

    Primitive plano = buildPrimitive();
    if (!plano) return plano;

    float half = (float)length / 2;
    float div_side = (float)length / divisions;

    std::vector<Point> gridPoints;
    std::vector<Point> gridNormals;
    std::vector<texCoord> gridTexCoords;

    // Define normal
    Point normal;
    switch (axis) {
        case 'X': normal = buildPoint(h >= 0 ? 1.0f : -1.0f, 0.0f, 0.0f); break;
        case 'Y': normal = buildPoint(0.0f, h >= 0 ? 1.0f : -1.0f, 0.0f); break;
        case 'Z': normal = buildPoint(0.0f, 0.0f, h >= 0 ? 1.0f : -1.0f); break;
        default:  normal = buildPoint(0.0f, 1.0f, 0.0f); break;
    }

    for (int linha = 0; linha <= divisions; ++linha) {
        for (int coluna = 0; coluna <= divisions; ++coluna) {
            float u = (float)coluna / divisions;
            float v = (float)linha / divisions;

            float x = -half + coluna * div_side;
            float z = -half + linha * div_side;

            Point p;
            texCoord tex;

            switch (axis) {
                case 'X':
                    p = buildPoint(h, x, z);
                    tex = { v, u }; // mapeamento: u = X, v = Z
                    break;

                case 'Y':
                    p = buildPoint(x, h, z);
                    tex = { u, 1.0f - v }; // mapeamento: u = X, v = Z (invertido)
                    break;

                case 'Z':
                    p = buildPoint(x, z, h);
                    tex = { u, 1.0f - v }; // mapeamento: u = X, v = Y (invertido)
                    break;

                default:
                    p = buildPoint(x, h, z);
                    tex = { u, 1.0f - v };
                    break;
            }

            gridPoints.push_back(p);
            gridNormals.push_back(normal);
            gridTexCoords.push_back(tex);
        }
    }

    std::vector<int> indices;

    for (int linha = 0; linha < divisions; ++linha) {
        for (int coluna = 0; coluna < divisions; ++coluna) {
            int i1 = linha * (divisions + 1) + coluna;
            int i2 = i1 + 1;
            int i3 = i1 + (divisions + 1);
            int i4 = i3 + 1;

            if (invertDiagonal) {
                if (invertFaces) {
                    indices.insert(indices.end(), {i1, i4, i2, i1, i3, i4});
                } else {
                    indices.insert(indices.end(), {i1, i2, i4, i1, i4, i3});
                }
            } else {
                if (invertFaces) {
                    indices.insert(indices.end(), {i1, i3, i2, i2, i3, i4});
                } else {
                    indices.insert(indices.end(), {i1, i2, i3, i2, i4, i3});
                }
            }
        }
    }

    // Popular primitiva
    plano->points = gridPoints;
    plano->normals = gridNormals;
    plano->texCoords = gridTexCoords;
    plano->indices = indices;

    return plano;
}




Primitive buildBox(int length, int divisions) {
    if (length <= 0 || divisions <= 0) {
        std::cerr << "Erro: Parâmetros inválidos para gerar o cubo." << std::endl;
        exit(1); 
    }

    Primitive box = buildPrimitive();
    if (!box) return box;

    float half = (float)length / 2;

    // criar as faces
    Primitive faces[6] = {
        buildPlane(length, divisions, 'Y',  half, true,  true),   // Cima
        buildPlane(length, divisions, 'Y', -half, false, false),  // Baixo
        buildPlane(length, divisions, 'Z', -half, true,  false),  // Frente
        buildPlane(length, divisions, 'Z',  half, false, true),   // Trás
        buildPlane(length, divisions, 'X', -half, true,  false),  // Direita
        buildPlane(length, divisions, 'X',  half, false, true)    // Esquerda
    };

    int pointOffset = 0;
    int normalOffset = 0;
    int texCoordOffset = 0;

    for (int i = 0; i < 6; ++i) {
        Primitive face = faces[i];

        // Adicionar pontos
        const auto& pts = getPoints(face);
        box->points.insert(box->points.end(), pts.begin(), pts.end());

        // Adicionar normais
        const auto& norms = getNormals(face);
        box->normals.insert(box->normals.end(), norms.begin(), norms.end());

        // Adicionar texCoords
        const auto& texs = getTexCoords(face);
        box->texCoords.insert(box->texCoords.end(), texs.begin(), texs.end());

        // Corrigir e adicionar índices
        for (int idx : getIndices(face)) {
            box->indices.push_back(idx + pointOffset);
        }

        // atualizar offsets
        pointOffset += pts.size();
        normalOffset += norms.size();
        texCoordOffset += texs.size();

        deletePrimitive2(face); // limpar memória da face
    }

    return box;
}



Primitive buildSphere(int radius, int slices, int stacks) {
    if (radius <= 0 || slices < 3 || stacks < 2) {
        std::cerr << "Erro: Parâmetros inválidos para gerar a esfera." << std::endl;
        exit(1);
    }

    Primitive sphere = buildPrimitive();
    if (!sphere) return sphere;

    std::vector<Point> points;
    std::vector<Point> normals;
    std::vector<texCoord> texCoords;
    std::vector<int> indices;

    // Geração dos vértices
    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = M_PI * (float)stack / stacks; // de 0 a pi
        float y = radius * cos(phi);
        float r = radius * sin(phi);

        float v = 1.0f - (float)stack / stacks;

        for (int slice = 0; slice <= slices; ++slice) {
            float theta = 2.0f * M_PI * (float)slice / slices; // de 0 a 2pi
            float x = r * sin(theta);
            float z = r * cos(theta);

            Point p = buildPoint(x, y, z);
            points.push_back(p);

            normals.push_back(buildPoint(x / radius, y / radius, z / radius));

            float u = (float)slice / slices;
            texCoords.push_back({u, v});
        }
    }

    // Geração dos índices
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int first = stack * (slices + 1) + slice;
            int second = first + slices + 1;

            // Triângulo 1
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            // Triângulo 2
            indices.push_back(first + 1);
            indices.push_back(second);
            indices.push_back(second + 1);
        }
    }

    // Finalização
    for (const auto& p : points) addPoint(sphere, p);
    for (const auto& n : normals) addNormal(sphere, n);
    for (const auto& t : texCoords) addTexCoord(sphere, t);
    setIndices(sphere, indices);

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
    std::vector<Point> normals;
    std::vector<texCoord> texCoords;

    // -------------------------
    // BASE DO CONE
    // -------------------------
    Point baseCenter = buildPoint(0.0f, 0.0f, 0.0f);
    int baseCenterIndex = points.size();
    points.push_back(baseCenter);
    normals.push_back(buildPoint(0.0f, -1.0f, 0.0f));
    texCoords.push_back({0.5f, 0.5f});

    std::vector<int> baseIndices;
    for (int i = 0; i < slices; i++) {
        float angle = 2.0f * M_PI * i / slices;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        points.push_back(buildPoint(x, 0.0f, z));
        normals.push_back(buildPoint(0.0f, -1.0f, 0.0f));
        texCoords.push_back({0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle)});
        baseIndices.push_back(points.size() - 1);
    }

    for (int i = 0; i < slices; i++) {
        int next = (i + 1) % slices;
        indices.push_back(baseCenterIndex);
        indices.push_back(baseIndices[next]);
        indices.push_back(baseIndices[i]);
    }

    // -------------------------
    // CORPO DO CONE
    // -------------------------
    std::vector<std::vector<int>> bodyIndices(stacks + 1);

    for (int stack = 0; stack <= stacks; stack++) {
        float h = (float)stack / stacks * height;
        float r = radius * (1.0f - (float)stack / stacks);

        for (int slice = 0; slice < slices; slice++) {
            float angle = 2.0f * M_PI * slice / slices;
            float x = r * cos(angle);
            float z = r * sin(angle);
            float u = (float)slice / slices;
            float v = (float)stack / stacks;

            Point pos = buildPoint(x, h, z);
            points.push_back(pos);
            texCoords.push_back({u, v});

            // normal da lateral (em direção ao lado da superfície)
            float nx = cos(angle);
            float ny = radius / sqrt(radius * radius + height * height);
            float nz = sin(angle);
            float len = sqrt(nx * nx + ny * ny + nz * nz);
            normals.push_back(buildPoint(nx / len, ny / len, nz / len));

            bodyIndices[stack].push_back(points.size() - 1);
        }
    }

    for (int stack = 0; stack < stacks; stack++) {
        for (int slice = 0; slice < slices; slice++) {
            int next = (slice + 1) % slices;

            int i1 = bodyIndices[stack][slice];
            int i2 = bodyIndices[stack + 1][slice];
            int i3 = bodyIndices[stack + 1][next];
            int i4 = bodyIndices[stack][next];

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);

            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i4);
        }
    }

    // -------------------------
    // TOPO DO CONE
    // -------------------------
    int topIndex = points.size();
    points.push_back(buildPoint(0.0f, height, 0.0f));
    normals.push_back(buildPoint(0.0f, 1.0f, 0.0f));
    texCoords.push_back({0.5f, 1.0f});

    for (int slice = 0; slice < slices; slice++) {
        int next = (slice + 1) % slices;
        int curr = bodyIndices[stacks][slice];
        int nxt = bodyIndices[stacks][next];

        indices.push_back(topIndex);
        indices.push_back(curr);
        indices.push_back(nxt);
    }

    // -------------------------
    // FINALIZAÇÃO DA PRIMITIVA
    // -------------------------
    for (const auto& p : points) {
        addPoint(cone, p);
    }

    for (const auto& n : normals) {
        addNormal(cone, n);
    }

    for (const auto& t : texCoords) {
        addTexCoord(cone, t);
    }

    setIndices(cone, indices);

    return cone;
}



Primitive buildSaturnRing(float innerRadius, float outerRadius, int slices, int stacks) {
    if (innerRadius <= 0 || outerRadius <= innerRadius || slices < 3 || stacks < 1) {
        std::cerr << "Erro: Parâmetros inválidos para gerar o anel." << std::endl;
        exit(1);
    }

    Primitive ring = buildPrimitive();
    if (!ring) return ring;

    std::vector<Point> points;
    std::vector<Point> normals;
    std::vector<texCoord> texCoords;
    std::vector<int> indices;

    // map de índices (stack * slices + slice)
    auto getIndex = [&](int stack, int slice) {
        return stack * slices + slice;
    };

    // gerar pontos
    for (int stack = 0; stack <= stacks; ++stack) {
        float radius = innerRadius + (float)stack / stacks * (outerRadius - innerRadius);

        for (int slice = 0; slice < slices; ++slice) {
            float angle = 2.0f * M_PI * slice / slices;
            float x = radius * cos(angle);
            float z = radius * sin(angle);

            Point p = buildPoint(x, 0.0f, z);
            points.push_back(p);

            // normal para cima
            normals.push_back(buildPoint(0.0f, 1.0f, 0.0f));

            // coordenada de textura
            texCoords.push_back({ (float)slice / slices, (float)stack / stacks });
        }
    }

    // gerar índices (dois triângulos por célula da grade)
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            int curr = getIndex(stack, slice);
            int next = getIndex(stack, (slice + 1) % slices);
            int currUp = getIndex(stack + 1, slice);
            int nextUp = getIndex(stack + 1, (slice + 1) % slices);

            // primeiro triângulo
            indices.push_back(curr);
            indices.push_back(currUp);
            indices.push_back(nextUp);

            // segundo triângulo
            indices.push_back(curr);
            indices.push_back(nextUp);
            indices.push_back(next);
        }
    }

    // carregar dados na primitiva
    for (const auto& p : points) addPoint(ring, p);
    for (const auto& n : normals) addNormal(ring, n);
    for (const auto& t : texCoords) addTexCoord(ring, t);
    setIndices(ring, indices);

    return ring;
}


