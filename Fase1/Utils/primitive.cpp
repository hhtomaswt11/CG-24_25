#include "primitive.h"

struct primitive {
  std::list<Point> points;
};

Primitive newEmptyPrimitive() {
    Primitive p = new struct primitive;  // Using 'new' instead of 'malloc'
    if (p) {
        p->points.clear();  // Initializes an empty list
    }
    return p;
}

Primitive newPrimitive(const std::list<Point>& points) {
    Primitive p = newEmptyPrimitive();
    if (p) {
        p->points = points;  // Assign the list of points
    }
    return p;
}

void addPoint(Primitive f, const Point& p) {
    if (f) {
        f->points.push_back(p);  // Adds a point to the list
    }
}

void addPoints(Primitive f, const Primitive toAdd) {
    if (f && toAdd) {
        f->points.insert(f->points.end(), toAdd->points.begin(), toAdd->points.end());  // Insert all points from another primitive
    }
}

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

    file << f->points.size() << "\n";
    for (const auto& p : f->points) {
        file << getX(p) << "," << getY(p) << "," << getZ(p) << "\n";
    }
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

Primitive fileToPrimitive(const char* path) {
    Primitive f = newEmptyPrimitive();
    std::ifstream file(path);

    if (!file) {
        std::cerr << "Error opening file: " << path << "\n";
        return f;
    }

    int vertexCount;
    file >> vertexCount;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignora o resto da linha

    std::string line;
    int count = 0;

    while (std::getline(file, line) && count < vertexCount) {
        std::stringstream ss(line);
        float x, y, z;
        char comma1, comma2;

        if (ss >> x >> comma1 >> y >> comma2 >> z && comma1 == ',' && comma2 == ',') {
            addPoint(f, newPoint(x, y, z));
            count++;
        } else {
            std::cerr << "Error processing line " << (count + 1) << ": " << line << "\n";
        }
    }

    if (count != vertexCount) {
        std::cerr << "Warning: Expected " << vertexCount << " vertices, but read " << count << "\n";
    }

    return f;
}


const std::list<Point>& getPoints(const Primitive f) {
    return f->points;
}

void deletePrimitive(Primitive f) {
    if (f) {
        for (auto& p : f->points) {
            deletePoint(p);  // Clean up each point in the list
        }
        f->points.clear();  // Clear the list of points
        delete f;  // Use 'delete' instead of 'free'
    }
}

void deletePrimitive2(Primitive f) {
    if (f) {
        f->points.clear();  // Just clear the list
        delete f;  // Use 'delete' instead of 'free'
    }
}
