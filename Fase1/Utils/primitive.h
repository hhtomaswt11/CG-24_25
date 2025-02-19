#ifndef PRIMITIVE
#define PRIMITIVE

#include <list>
#include <fstream>
#include <iostream>
#include "point.h"

typedef struct primitive* Primitive;

Primitive newEmptyPrimitive();

Primitive newPrimitive(const std::list<Point>& points);

void addPrimitive(Primitive &dest, const Primitive &src) ; 

void addTriangle(Primitive plano, const Point& p1, const Point& p2, const Point& p3); 

void addPoint(Primitive f, const Point& p);

void addPoints(Primitive f, Primitive toAdd);

void primitiveToFile(Primitive f, const char* path);

Primitive fileToPrimitive(const char* path);

const std::list<Point>& getPoints(Primitive f);

void deletePrimitiveSimple(Primitive f); 

void deletePrimitive(Primitive f);

void deletePrimitive2(Primitive f);

#endif // PRIMITIVE
