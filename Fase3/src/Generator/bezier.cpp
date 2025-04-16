#include "../../include/Generator/bezier.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
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
    file.ignore();  // ignora newline

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

// Point computeBezierPoint(float u, float v, Point patchPoints[4][4]) {
//     //Point point(0,0,0);
//     Point point = buildPoint(0, 0, 0);


//     float bu[4], bv[4];
//     for (int i = 0; i < 4; i++) {
//         bu[i] = bezierBlend(bezierMatrix, u, i);
//         bv[i] = bezierBlend(bezierMatrix, v, i);
//     }

//     for (int i = 0; i < 4; i++) {
//         for (int j = 0; j < 4; j++) {
//             Point p = patchPoints[i][j];
//             float b = bu[i] * bv[j];
//             point = point + (p * b);
//         }
//     }

//     return point;
// }


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

            Point temp = multiplyPoint(p, b);           // p * b
            Point newPoint = addPoints(point, temp);    // point + temp

            freePoint(point);  // libertar o anterior
            freePoint(temp);   // libertar o temp
            point = newPoint;  // atualizar o acumulador
        }
    }

    return point;
}


std::string pointToStr(Point p) {
    return std::to_string(getX(p)) + " " + std::to_string(getY(p)) + " " + std::to_string(getZ(p));
}

void buildBezierPatchPrimitive(std::string patchFile, int tessellation, std::string outputFile) {
    std::vector<Patch> patches;
    std::vector<Point> controlPoints;
    readPatchFile(patchFile, patches, controlPoints);

    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro de saída: " << outputFile << std::endl;
        exit(1);
    }

    int totalTriangles = patches.size() * (tessellation * tessellation * 2);
    out << totalTriangles * 3 << std::endl; // cada triângulo tem 3 vértices

    for (Patch &p : patches) {
        Point patchPoints[4][4];

        // organizar os 16 pontos de controlo numa matriz 4x4
        for (int i = 0; i < 16; i++) {
            patchPoints[i / 4][i % 4] = controlPoints[p.indices[i]];
        }

        for (int i = 0; i < tessellation; i++) {
            float u = (float)i / tessellation;
            float uNext = (float)(i + 1) / tessellation;

            for (int j = 0; j < tessellation; j++) {
                float v = (float)j / tessellation;
                float vNext = (float)(j + 1) / tessellation;

                Point p1 = computeBezierPoint(u, v, patchPoints);
                Point p2 = computeBezierPoint(uNext, v, patchPoints);
                Point p3 = computeBezierPoint(u, vNext, patchPoints);
                Point p4 = computeBezierPoint(uNext, vNext, patchPoints);

                // triângulo 1
                out <<  pointToStr(p1)  << std::endl;
                out <<  pointToStr(p2) << std::endl;
                out <<  pointToStr(p3)  << std::endl;

                // triângulo 2
                out <<  pointToStr(p3) << std::endl;
                out <<  pointToStr(p2) << std::endl;
                out <<  pointToStr(p4)  << std::endl;
            }
        }
    }

    out.close();
}
