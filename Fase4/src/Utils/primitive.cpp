#include "../../include/Utils/primitive.h"

// struct Color {
//     float r, g, b;
// };


Primitive buildPrimitive() {
    Primitive p = new struct primitive;  
    if (p) {
        p->points.clear();  
        p->indices.clear(); 
    }
    return p;
}

void fromPrimitiveTo3dFile(const Primitive f, const char* path) {
    if (f->points.empty()) {
        std::cerr << "Primitive is empty.\n";
        return;
    }

    std::ofstream file(path);
    if (!file) {
        std::cerr << "Error opening file: " << path << "\n";
        return;
    }

    // escrever os pontos únicos 
    file << "Vertices: " << f->points.size() << "\n";
    for (const auto& p : f->points) {
        file << getX(p) << "," << getY(p) << "," << getZ(p) << "\n";
    }

    // escrever as normais
    file << "Normals: " << f->normals.size() << "\n";
    for (const auto& n : f->normals) {
        file << getX(n) << "," << getY(n) << "," << getZ(n) << "\n";
    }

    // escrever as coordenadas de textura
    file << "TexCoords: " << f->texCoords.size() << "\n";
    for (const auto& tex : f->texCoords) {
        file << tex.u << "," << tex.v << "\n";
    }

    // escrever índices para triângulos
    file << "Indices: " << f->indices.size() << "\n";
    for (size_t i = 0; i < f->indices.size(); i += 3) {
        file << f->indices[i] << "," << f->indices[i + 1] << "," << f->indices[i + 2] << "\n";
    }

    // escrever os índices das normais
    file << "NormalIndices: " << f->normalIndices.size() << "\n";
    for (size_t i = 0; i < f->normalIndices.size(); i+=3) {
        file << f->normalIndices[i] << "," << f->normalIndices[i+1] << "," << f->normalIndices[i+2] << "\n";
    }

    // escrever os índices das coordenadas de textura
    file << "TexCoordIndices: " << f->texCoordIndices.size() << "\n";
    for (size_t i = 0; i < f->texCoordIndices.size(); i+=3) {
        file << f->texCoordIndices[i] << "," << f->texCoordIndices[i+1] << "," <<  f->texCoordIndices[i+2] << "\n";
    }
}

Primitive from3dFileToPrimitive(const char* path) {
    Primitive f = buildPrimitive();
    std::ifstream file(path);

    if (!file) {
        std::cerr << "File does not exist. Error opening file: " << path << "\n";
        return f;
    }

    std::string line;
    int vertexCount, normalCount, texCoordCount, indexCount;

    // carregar os pontos da primitiva
    std::getline(file, line);
    sscanf(line.c_str(), "Vertices: %d", &vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
        std::getline(file, line);
        float x, y, z;
        sscanf(line.c_str(), "%f,%f,%f", &x, &y, &z);
        Point ponto = buildPoint(x, y, z);
        addPoint(f, ponto);
    }

    // carregar as normais
    std::getline(file, line);
    sscanf(line.c_str(), "Normals: %d", &normalCount);
    for (int i = 0; i < normalCount; ++i) {
        std::getline(file, line);
        float x, y, z;
        sscanf(line.c_str(), "%f,%f,%f", &x, &y, &z);
        Point normal = buildPoint(x, y, z);
        f->normals.push_back(normal);
    }

    // carregar as coordenadas de textura
    std::getline(file, line);
    sscanf(line.c_str(), "TexCoords: %d", &texCoordCount);
    for (int i = 0; i < texCoordCount; ++i) {
        std::getline(file, line);
        float u, v;
        sscanf(line.c_str(), "%f,%f", &u, &v);
        texCoord tex = {u, v};
        f->texCoords.push_back(tex);
    }

    // carregar os índices da primitiva
    std::getline(file, line);
    sscanf(line.c_str(), "Indices: %d", &indexCount);
    for (int i = 0; i < indexCount / 3; ++i) {
        int i1, i2, i3;
        std::getline(file, line);
        sscanf(line.c_str(), "%d,%d,%d", &i1, &i2, &i3);
        f->indices.push_back(i1);
        f->indices.push_back(i2);
        f->indices.push_back(i3);
    }

    // carregar os índices das normais
    std::getline(file, line);
    int normalIndexCount;
    sscanf(line.c_str(), "NormalIndices: %d", &normalIndexCount);
    for (int i = 0; i < normalIndexCount; ++i) {
        int normalIndex;
        std::getline(file, line);
        sscanf(line.c_str(), "%d", &normalIndex);
        f->normalIndices.push_back(normalIndex);
    }

    // carregar os índices das coordenadas de textura
    std::getline(file, line);
    int texCoordIndexCount;
    sscanf(line.c_str(), "TexCoordIndices: %d", &texCoordIndexCount);
    for (int i = 0; i < texCoordIndexCount; ++i) {
        int texCoordIndex;
        std::getline(file, line);
        sscanf(line.c_str(), "%d", &texCoordIndex);
        f->texCoordIndices.push_back(texCoordIndex);
    }

    return f;
}



// ADD 

void addPrimitive(Primitive &dest, const Primitive &src) {
    if (dest && src) {
        dest->points.insert(dest->points.end(), src->points.begin(), src->points.end());
        dest->indices.insert(dest->indices.end(), src->indices.begin(), src->indices.end());
    }
}

void addTriangle(Primitive plano, const Point& p1, const Point& p2, const Point& p3) {
    addPoint(plano, p1);
    addPoint(plano, p2);
    addPoint(plano, p3);
}

void addPoint(Primitive f, const Point& p) {
    if (f) {
        f->points.push_back(p);  
    }
}


void addPoints(Primitive f, const Primitive toAdd) {
    if (f && toAdd) {
        f->points.insert(f->points.end(), toAdd->points.begin(), toAdd->points.end());
        f->indices.insert(f->indices.end(), toAdd->indices.begin(), toAdd->indices.end());
    }
}

void addNormal(Primitive& f, const Point& normal) {
    if (f) {
        f->normals.push_back(normal);
    }
}

void addTexCoord(Primitive& f, const texCoord& texCoord) {
    if (f) {
        f->texCoords.push_back(texCoord);
    }
}



// SETTERS 

void setIndices(Primitive f, const std::vector<int>& indices) {
    if (f) {
        f->indices = indices;
    }
}

void setNormalIndices(Primitive f, const std::vector<int>& normalIndices) {
    if (f) {
        f->normalIndices = normalIndices;
    }
}

void setTexCoordIndices(Primitive f, const std::vector<int>& texCoordIndices) {
    if (f) {
        f->texCoordIndices = texCoordIndices;
    }
}


// GETTERS 

const std::vector<int>& getIndices(const Primitive f) {
    return f->indices;
}


const std::vector<Point>& getPoints(const Primitive f) {
    return f->points;
}

Point getPoint(const Primitive f, std::vector<Point>::size_type index) {
    if (f && index < f->points.size()) {
        return f->points[index];
    }
    return nullptr;  
}

const std::vector<Point>& getNormals(const Primitive f) {
    return f->normals;
}

const std::vector<texCoord>& getTexCoords(const Primitive f) {
    return f->texCoords;
}

const std::vector<int>& getNormalIndices(const Primitive f) {
    return f->normalIndices;
}

const std::vector<int>& getTexCoordIndices(const Primitive f) {
    return f->texCoordIndices;
}

Point getNormal(const Primitive f, std::vector<Point>::size_type index) {
    if (f && index < f->normals.size()) {
        return f->normals[index];
    }
    return nullptr;
}

texCoord getTexCoord(const Primitive f, std::vector<TexCoord>::size_type index) {
    if (f && index < f->texCoords.size()) {
        return f->texCoords[index];
    }
    return {0.0f, 0.0f};  // valor default seguro
}



// DESTROYER 

void deletePrimitive(Primitive f) {
    if (f) {
        for (auto& p : f->points) {
            freePoint(p); 
        }
        f->points.clear();  
        f->indices.clear(); 
        delete f;  
    }
}

void deletePrimitive2(Primitive f) {
    if (f) {
        f->points.clear();  
        f->indices.clear(); 
        delete f; 
    }
}
