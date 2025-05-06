#ifndef POINT
#define POINT
#include <math.h>
#include <stdlib.h>

typedef struct point* Point;

Point buildPoint(float x, float y, float z);

float getX(Point p);
float getY(Point p);
float getZ(Point p);

void freePoint(Point p);


Point addPoints(Point a, Point b) ; 
Point multiplyPoint(Point p, float scalar) ; 
#endif 