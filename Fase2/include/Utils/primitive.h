#ifndef PRIMITIVE
#define PRIMITIVE

#include <list>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

typedef struct primitive* Primitive;
// typedef struct  color* Color; 

Primitive newEmptyPrimitive();

// Primitive newPrimitive(const std::list<Point>& points);
Primitive newPrimitive(const std::vector<Point>& points) ; 
void addPrimitive(Primitive &dest, const Primitive &src) ; 

void addTriangle(Primitive plano, const Point& p1, const Point& p2, const Point& p3); 

void addPoint(Primitive f, const Point& p);

void addPoints(Primitive f, Primitive toAdd);

void fromPrimitiveTo3dFile(Primitive f, const char* path);

Primitive from3dFileToPrimitive(const char* path);

void setIndices(Primitive f, const std::vector<int>& indices) ; 

const std::vector<int>& getIndices(const Primitive f) ; 

Point getPoint(const Primitive f, std::vector<Point>::size_type index) ; 

// Point getPoint(const Primitive f, int index) ; 

//const std::list<Point>& getPoints(Primitive f);

const std::vector<Point>& getPoints(const Primitive f) ; 

void deletePrimitiveSimple(Primitive f); 

void deletePrimitive(Primitive f);

void deletePrimitive2(Primitive f);

#endif // PRIMITIVE
