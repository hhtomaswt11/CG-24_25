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
 // std::list<Point> points;
  std::vector<Point> points;  // Alterado de list para vector
  std::vector<int> indices;  // Adicionar esta linha
 // std::list<Color> colors;
};



Primitive newEmptyPrimitive() {
    Primitive p = new struct primitive;  // Using 'new' instead of 'malloc'
    if (p) {
        p->points.clear();  // Initializes an empty list
        p->indices.clear(); 
    }
    return p;
}

// Primitive newPrimitive(const std::list<Point>& points) {
//     Primitive p = newEmptyPrimitive();
//     if (p) {
//         p->points = points;  // Assign the list of points
//     }
//     return p;
// }

Primitive newPrimitive(const std::vector<Point>& points) {
    Primitive p = newEmptyPrimitive();
    if (p) {
        p->points = points;  // Assign the list of points
    }
    return p;
}


// void addPrimitive(Primitive &dest, const Primitive &src) {
//     for (const auto& ponto : src->points) { // Copia os pontos da `src` para `dest`
//         dest->points.push_back(ponto);
//     }
// }


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
        f->points.push_back(p);  // Adds a point to the list
    }
}

// void addPoints(Primitive f, const Primitive toAdd) {
//     if (f && toAdd) {
//         f->points.insert(f->points.end(), toAdd->points.begin(), toAdd->points.end());  // Insert all points from another primitive
//     }
// }

void addPoints(Primitive f, const Primitive toAdd) {
    if (f && toAdd) {
        f->points.insert(f->points.end(), toAdd->points.begin(), toAdd->points.end());
        f->indices.insert(f->indices.end(), toAdd->indices.begin(), toAdd->indices.end());
    }
}


// void primitiveToFile(const Primitive f, const char* path) {
//     if (!f || f->points.empty()) {
//         std::cerr << "Primitive is empty or NULL.\n";
//         return;
//     }
//     std::cout << "Path 3D na primitiveToFile: " << path << std::endl;

//     // lista de primitivas = getprimitivas(f); 

//     std::ofstream file(path);
//     if (!file) {
//         std::cerr << "Error opening file: " << path << "\n";
//         return;
//     }

//     file << f->points.size() << "\n";
//     for (const auto& p : f->points) {
//         file << getX(p) << "," << getY(p) << "," << getZ(p) << "\n";
//     }
// }

void primitiveToFile(const Primitive f, const char* path) {
    if (!f || f->points.empty()) {
        std::cerr << "Primitive is empty or NULL.\n";
        return;
    }

    std::ofstream file(path);
    if (!file) {
        std::cerr << "Error opening file: " << path << "\n";
        return;
    }

    // Escrever pontos únicos
    file << "Vertices: " << f->points.size() << "\n";
    for (const auto& p : f->points) {
        file << getX(p) << "," << getY(p) << "," << getZ(p) << "\n";
    }

    // Escrever índices para triângulos
    file << "Indices: " << f->indices.size() << "\n";
    for (size_t i = 0; i < f->indices.size(); i += 3) {
        file << f->indices[i] << "," << f->indices[i + 1] << "," << f->indices[i + 2] << "\n";
    }
}


// Primitive fileToPrimitive(const char* path) {
//     Primitive f = newEmptyPrimitive();
//     std::ifstream file(path);

//     // if (!file) {
//     //     std::cerr << "Error opening file: " << path << "\n";
//     //     return f;
//     // }

//     int vertexCount;
//     file >> vertexCount;
//     file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignora o resto da linha

//     std::string line;
//     int count = 0;

//     while (std::getline(file, line) && count < vertexCount) {
//         std::stringstream ss(line);
//         float x, y, z;
//         char comma1, comma2;

//         if (ss >> x >> comma1 >> y >> comma2 >> z && comma1 == ',' && comma2 == ',') {
//             addPoint(f, newPoint(x, y, z));
//             count++;
//         } else {
//             std::cerr << "Error processing line " << (count + 1) << ": " << line << "\n";
//         }
//     }

//     if (count != vertexCount) {
//         std::cerr << "Warning: Expected " << vertexCount << " vertices, but read " << count << "\n";
//     }

//     return f;
// }




// Primitive fileToPrimitive(const char* path) {
//     Primitive f = newEmptyPrimitive();
//     std::ifstream file(path);

//     if (!file) {
//         std::cerr << "Erro ao abrir o ficheiro: " << path << "\n";
//         return f;
//     }

//     int vertexCount;
//     file >> vertexCount;
//     file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignora o resto da linha

//     std::string line;
//     int count = 0;

//     // Carregar pontos
//     std::cout << "Carregando pontos do ficheiro: " << path << std::endl;
//     while (std::getline(file, line) && count < vertexCount) {
//         std::stringstream ss(line);
//         float x, y, z;
//         char comma1, comma2;

//         if (ss >> x >> comma1 >> y >> comma2 >> z && comma1 == ',' && comma2 == ',') {
//             Point ponto = newPoint(x, y, z);
//             addPoint(f, ponto);
//             std::cout << "Ponto carregado: (" << x << ", " << y << ", " << z << ")" << std::endl;
//             count++;
//         } else {
//             std::cerr << "Erro ao processar linha " << (count + 1) << ": " << line << "\n";
//         }
//     }

//     if (count != vertexCount) {
//         std::cerr << "Aviso: Esperados " << vertexCount << " vértices, mas foram lidos " << count << "\n";
//     }

//     // Carregar índices
//     std::cout << "Carregando índices do ficheiro: " << path << std::endl;
//     while (std::getline(file, line)) {
//         std::stringstream ss(line);
//         int i1, i2, i3;
//         char comma1, comma2;

//         if (ss >> i1 >> comma1 >> i2 >> comma2 >> i3 && comma1 == ',' && comma2 == ',') {
//             std::cout << "Índice carregado: " << i1 << ", " << i2 << ", " << i3 << std::endl;
//             f->indices.push_back(i1);
//             f->indices.push_back(i2);
//             f->indices.push_back(i3);
//         } else {
//             std::cerr << "Erro ao processar linha de índice: " << line << "\n";
//         }
//     }

//     return f;
// }



Primitive fileToPrimitive(const char* path) {
    Primitive f = newEmptyPrimitive();
    std::ifstream file(path);

    if (!file) {
        std::cerr << "Erro ao abrir o ficheiro: " << path << "\n";
        return f;
    }

    std::string line;
    int vertexCount;
    
    // Ler a quantidade de vértices
    std::getline(file, line);
    sscanf(line.c_str(), "Vertices: %d", &vertexCount);

    // Carregar pontos
    std::cout << "Carregando pontos do ficheiro: " << path << std::endl;
    for (int i = 0; i < vertexCount; ++i) {
        std::getline(file, line);
        float x, y, z;
        sscanf(line.c_str(), "%f,%f,%f", &x, &y, &z);
        Point ponto = newPoint(x, y, z);
        addPoint(f, ponto);
        std::cout << "Ponto carregado: (" << x << ", " << y << ", " << z << ")" << std::endl;
    }

    // Carregar índices
    int indexCount;
    std::getline(file, line);
    sscanf(line.c_str(), "Indices: %d", &indexCount);

    std::cout << "Carregando índices do ficheiro: " << path << std::endl;
    for (int i = 0; i < indexCount / 3; ++i) {
        int i1, i2, i3;
        std::getline(file, line);
        sscanf(line.c_str(), "%d,%d,%d", &i1, &i2, &i3);
        f->indices.push_back(i1);
        f->indices.push_back(i2);
        f->indices.push_back(i3);
        std::cout << "Triângulo: " << i1 << ", " << i2 << ", " << i3 << std::endl;
    }

    return f;
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


// const std::list<Point>& getPoints(const Primitive f) {
//     return f->points;
// }

const std::vector<Point>& getPoints(const Primitive f) {
    return f->points;
}

Point getPoint(const Primitive f, int index) {
    if (f && index >= 0 && index < f->points.size()) {
        return f->points[index];
    }
    return nullptr;  // Retorna NULL se o índice estiver fora dos limites
}


void deletePrimitiveSimple(Primitive f) {
    if (f) {
        f->points.clear();  //limpa os pontos
        f->indices.clear(); 
        delete f;  // Libera a estrutura Primitive
    }
}

void deletePrimitive(Primitive f) {
    if (f) {
        for (auto& p : f->points) {
            deletePoint(p);  // Clean up each point in the list
        }
        f->points.clear();  // Clear the list of points
        f->indices.clear(); 
        delete f;  
    }
}

void deletePrimitive2(Primitive f) {
    if (f) {
        f->points.clear();  // Just clear the list
        f->indices.clear(); 
        delete f; 
    }
}
