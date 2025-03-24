#include "../../include/Utils/primitive.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <vector>
#include <map>

// struct Color {
//     float r, g, b;
// };


struct primitive {
  std::vector<Point> points; 
  std::vector<int> indices;  
};



Primitive newEmptyPrimitive() {
    Primitive p = new struct primitive;  
    if (p) {
        p->points.clear();  
        p->indices.clear(); 
    }
    return p;
}



Primitive newPrimitive(const std::vector<Point>& points) {
    Primitive p = newEmptyPrimitive();
    if (p) {
        p->points = points;  
    }
    return p;
}


void fromPrimitiveTo3dFile(const Primitive f, const char* path) {
    if (!f || f->points.empty()) {
        std::cerr << "Primitive is empty or NULL.\n";
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

    // escrever índices para triângulos
    file << "Indices: " << f->indices.size() << "\n";
   // file << "Triangulos: " << f->indices.size() / 3 << "\n";
    for (size_t i = 0; i < f->indices.size(); i += 3) {
        file << f->indices[i] << "," << f->indices[i + 1] << "," << f->indices[i + 2] << "\n";
    }
    
}


Primitive from3dFileToPrimitive(const char* path) {
    Primitive f = newEmptyPrimitive();
    std::ifstream file(path);

    if (!file) {
        std::cerr << "File does not exist. Error opening file: " << path << "\n";
         // exit(EXIT_FAILURE);
         return f;
    }

    std::string line;
    int vertexCount;
    
 
    std::getline(file, line);
    sscanf(line.c_str(), "Vertices: %d", &vertexCount);

    // carregar os pontos da primitiva 
    //std::cout << "Loading points from file: " << path << std::endl;
    for (int i = 0; i < vertexCount; ++i) {
        std::getline(file, line);
        float x, y, z;
        sscanf(line.c_str(), "%f,%f,%f", &x, &y, &z);
        Point ponto = newPoint(x, y, z);
        addPoint(f, ponto);
        // std::cout << "Loaded point: (" << x << ", " << y << ", " << z << ")" << std::endl;
    }

    // carregar os índices da primitiva 
    int indexCount;
    std::getline(file, line);
    sscanf(line.c_str(), "Indices: %d", &indexCount);

    //std::cout << "Loading indexes from file: " << path << std::endl;
    for (int i = 0; i < indexCount / 3; ++i) {
        int i1, i2, i3;
        std::getline(file, line);
        sscanf(line.c_str(), "%d,%d,%d", &i1, &i2, &i3);
        f->indices.push_back(i1);
        f->indices.push_back(i2);
        f->indices.push_back(i3);
        // std::cout << "Triangle connection: " << i1 << ", " << i2 << ", " << i3 << std::endl;
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


// SETTERS 

void setIndices(Primitive f, const std::vector<int>& indices) {
    if (f) {
        f->indices = indices;
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


// DESTROYER 

void deletePrimitiveSimple(Primitive f) {
    if (f) {
        f->points.clear();  
        f->indices.clear(); 
        delete f; 
    }
}

void deletePrimitive(Primitive f) {
    if (f) {
        for (auto& p : f->points) {
            deletePoint(p); 
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
