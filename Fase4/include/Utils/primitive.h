#ifndef PRIMITIVE
#define PRIMITIVE

#include <list>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include "point.h"

typedef struct primitive* Primitive;

struct texCoord {
    float u, v;
};

struct primitive {
    std::vector<Point> points;
    std::vector<Point> normals;
    std::vector<texCoord> texCoords;

    std::vector<int> indices;
};
typedef struct texCoord* TexCoord;
typedef struct primitive* Primitive;

// BUILDERS
Primitive buildPrimitive();

// 3d BASED 

void fromPrimitiveTo3dFile(Primitive f, const char* path);
Primitive from3dFileToPrimitive(const char* path);

// ADD 
void addPrimitive(Primitive &dest, const Primitive &src) ; 
void addTriangle(Primitive plano, const Point& p1, const Point& p2, const Point& p3); 
void addPoint(Primitive f, const Point& p);
void addPoints(Primitive f, Primitive toAdd);
void addNormal(Primitive& f, const Point& normal);
void addTexCoord(Primitive& f, const texCoord& texCoord);

// SETTERS 
void setIndices(Primitive f, const std::vector<int>& indices) ; 

// GETTERS 
const std::vector<int>& getIndices(const Primitive f) ; 

Point getPoint(const Primitive f, std::vector<Point>::size_type index) ; 

const std::vector<Point>& getPoints(const Primitive f) ; 

const std::vector<Point>& getNormals(const Primitive f);
const std::vector<texCoord>& getTexCoords(const Primitive f);

// DESTROYER 
void deletePrimitive(Primitive f);

void deletePrimitive2(Primitive f);

#endif 
