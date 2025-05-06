#include "../../include/Generator/bezier.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <tuple>
#include <cmath>
#include <cstring>

const float bezierMatrix[4][4] = {
    {-1.0f,  3.0f, -3.0f, 1.0f},
    { 3.0f, -6.0f,  3.0f, 0.0f},
    {-3.0f,  3.0f,  0.0f, 0.0f},
    { 1.0f,  0.0f,  0.0f, 0.0f}
};

struct Patch {
    int indices[16];
};

void readPatchFile(std::string filename, std::vector<Patch> &patches, std::vector<Point> &controlPoints) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro de patch: " << filename << std::endl;
        exit(1);
    }

    int nPatches;
    file >> nPatches;
    file.ignore();

    patches.resize(nPatches);
    for (int i = 0; i < nPatches; i++) {
        std::string line;
        std::getline(file, line);
        std::stringstream ss(line);
        std::string val;
        int j = 0;
        while (getline(ss, val, ',')) {
            patches[i].indices[j++] = stoi(val);
        }
    }

    int nPoints;
    file >> nPoints;
    controlPoints.resize(nPoints);

    for (int i = 0; i < nPoints; i++) {
        float x, y, z;
        file >> x;
        file.ignore(1);
        file >> y;
        file.ignore(1);
        file >> z;
        controlPoints[i] = buildPoint(x, y, z);
    }
}

float bezierBlend(const float m[4][4], float t, int row) {
    return m[row][0]*t*t*t + m[row][1]*t*t + m[row][2]*t + m[row][3];
}

Point computeBezierPoint(float u, float v, Point patchPoints[4][4]) {
    Point point = buildPoint(0, 0, 0);

    float bu[4], bv[4];
    for (int i = 0; i < 4; i++) {
        bu[i] = bezierBlend(bezierMatrix, u, i);
        bv[i] = bezierBlend(bezierMatrix, v, i);
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            Point p = patchPoints[i][j];
            float b = bu[i] * bv[j];
            Point temp = multiplyPoint(p, b);
            Point newPoint = addPoints(point, temp);
            freePoint(point);
            freePoint(temp);
            point = newPoint;
        }
    }

    return point;
}
void buildBezierPatchPrimitive(std::string patchFile, int tessellation, std::string outputFile) {
    std::vector<Patch> patches;
    std::vector<Point> controlPoints;
    readPatchFile(patchFile, patches, controlPoints);

    std::vector<Point> uniquePoints;
    std::vector<int> indices;
    int offset = 0;

    float baseLimitY = -1.4f; // limite para identificar a base

    for (const Patch &p : patches) {
        // Verifica se todos os pontos de controlo estão abaixo do limite Y
        bool isBasePatch = true;
        for (int i = 0; i < 16; i++) {
            Point cp = controlPoints[p.indices[i]];
            if (getY(cp) >= baseLimitY) {
                isBasePatch = false;
                break;
            }
        }

        if (isBasePatch)
            continue; // ignora patch da base

        Point patchPoints[4][4];
        for (int i = 0; i < 16; i++) {
            patchPoints[i / 4][i % 4] = controlPoints[p.indices[i]];
        }

        std::vector<std::vector<Point>> grid(tessellation + 1, std::vector<Point>(tessellation + 1));

        for (int i = 0; i <= tessellation; ++i) {
            float u = (float)i / tessellation;
            for (int j = 0; j <= tessellation; ++j) {
                float v = (float)j / tessellation;
                grid[i][j] = computeBezierPoint(u, v, patchPoints);
                uniquePoints.push_back(grid[i][j]);
            }
        }

        for (int i = 0; i < tessellation; ++i) {
            for (int j = 0; j < tessellation; ++j) {
                int i1 = offset + i * (tessellation + 1) + j;
                int i2 = offset + (i + 1) * (tessellation + 1) + j;
                int i3 = offset + i * (tessellation + 1) + (j + 1);
                int i4 = offset + (i + 1) * (tessellation + 1) + (j + 1);

                indices.push_back(i1);
                indices.push_back(i3);
                indices.push_back(i2);
                
                indices.push_back(i3);
                indices.push_back(i4);
                indices.push_back(i2);
                
            }
        }

        offset += (tessellation + 1) * (tessellation + 1);
    }

    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro de saída: " << outputFile << std::endl;
        exit(1);
    }

    out << "Vertices: " << uniquePoints.size() << "\n";
    for (const auto& p : uniquePoints)
        out << getX(p) << "," << getY(p) << "," << getZ(p) << "\n";

    out << "Indices: " << indices.size() << "\n";
    for (size_t i = 0; i < indices.size(); i += 3)
        out << indices[i] << "," << indices[i + 1] << "," << indices[i + 2] << "\n";

    out.close();
}
