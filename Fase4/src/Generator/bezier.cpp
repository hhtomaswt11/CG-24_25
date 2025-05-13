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

Point cross(Point a, Point b) {
    float x = getY(a) * getZ(b) - getZ(a) * getY(b);
    float y = getZ(a) * getX(b) - getX(a) * getZ(b);
    float z = getX(a) * getY(b) - getY(a) * getX(b);
    return buildPoint(x, y, z);
}

void normalize(Point &p) {
    float length = sqrt(getX(p) * getX(p) + getY(p) * getY(p) + getZ(p) * getZ(p));
    if (length > 0.000001f) {
        float invLength = 1.0f / length;
        float x = getX(p) * invLength;
        float y = getY(p) * invLength;
        float z = getZ(p) * invLength;

        freePoint(p);
        p = buildPoint(x, y, z);
    }
}



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

void computeBezierDerivatives(float u, float v, Point patch[4][4], Point &dU, Point &dV) {
    Point du = buildPoint(0, 0, 0);
    Point dv = buildPoint(0, 0, 0);

    float bu[4], dbu[4];
    float bv[4], dbv[4];

    for (int i = 0; i < 4; ++i) {
        bu[i] = bezierBlend(bezierMatrix, u, i);
        dbu[i] = 3 * bezierMatrix[i][0]*u*u + 2 * bezierMatrix[i][1]*u + bezierMatrix[i][2];
        bv[i] = bezierBlend(bezierMatrix, v, i);
        dbv[i] = 3 * bezierMatrix[i][0]*v*v + 2 * bezierMatrix[i][1]*v + bezierMatrix[i][2];
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float bdu = dbu[i] * bv[j];
            float bdv = bu[i] * dbv[j];
            Point p = patch[i][j];

            Point pdU = multiplyPoint(p, bdu);
            Point pdV = multiplyPoint(p, bdv);

            Point newDu = addPoints(du, pdU);
            Point newDv = addPoints(dv, pdV);

            freePoint(du);
            freePoint(dv);
            freePoint(pdU);
            freePoint(pdV);

            du = newDu;
            dv = newDv;
        }
    }

    dU = du;
    dV = dv;
}


void buildBezierPatchPrimitive(std::string patchFile, int tessellation, std::string outputFile) {
    std::vector<Patch> patches;
    std::vector<Point> controlPoints;
    readPatchFile(patchFile, patches, controlPoints);

    std::vector<Point> uniquePoints;
    std::vector<Point> normals;
    std::vector<texCoord> texCoords;
    std::vector<int> indices;
    int offset = 0;

    float baseLimitY = -1.4f;

    for (const Patch &p : patches) {
        bool isBasePatch = true;
        for (int i = 0; i < 16; i++) {
            if (getY(controlPoints[p.indices[i]]) >= baseLimitY) {
                isBasePatch = false;
                break;
            }
        }

        if (isBasePatch)
            continue;

        Point patchPoints[4][4];
        for (int i = 0; i < 16; i++) {
            patchPoints[i / 4][i % 4] = controlPoints[p.indices[i]];
        }

        std::vector<std::vector<int>> indexGrid(tessellation + 1, std::vector<int>(tessellation + 1));

        for (int i = 0; i <= tessellation; ++i) {
            float u = (float)i / tessellation;
            for (int j = 0; j <= tessellation; ++j) {
                float v = (float)j / tessellation;

                Point pos = computeBezierPoint(u, v, patchPoints);
                Point dU, dV;
                computeBezierDerivatives(u, v, patchPoints, dU, dV);
                Point normal = cross(dV, dU);
                normalize(normal);
                normals.push_back(normal);
                

                uniquePoints.push_back(pos);
                texCoords.push_back({u, v});

                indexGrid[i][j] = offset++;
            }
        }

        for (int i = 0; i < tessellation; ++i) {
            for (int j = 0; j < tessellation; ++j) {
                int i1 = indexGrid[i][j];
                int i2 = indexGrid[i + 1][j];
                int i3 = indexGrid[i][j + 1];
                int i4 = indexGrid[i + 1][j + 1];

                indices.push_back(i1);
                indices.push_back(i3);
                indices.push_back(i2);

                indices.push_back(i3);
                indices.push_back(i4);
                indices.push_back(i2);
            }
        }
    }

    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro de saída: " << outputFile << std::endl;
        exit(1);
    }

    out << "Vertices: " << uniquePoints.size() << "\n";
    for (const auto& p : uniquePoints)
        out << getX(p) << "," << getY(p) << "," << getZ(p) << "\n";

    out << "Normals: " << normals.size() << "\n";
    for (const auto& n : normals)
        out << getX(n) << "," << getY(n) << "," << getZ(n) << "\n";

    out << "TexCoords: " << texCoords.size() << "\n";
    for (const auto& t : texCoords)
        out << t.u << "," << t.v << "\n";

    out << "Indices: " << indices.size() << "\n";
    for (size_t i = 0; i < indices.size(); i += 3)
        out << indices[i] << "," << indices[i + 1] << "," << indices[i + 2] << "\n";

    out.close();
}
