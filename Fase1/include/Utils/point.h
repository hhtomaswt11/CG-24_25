#ifndef POINT
#define POINT
#include <math.h>
#include <stdlib.h>

typedef struct point* Point;

Point newEmptyPoint();
Point newPoint(float x, float y, float z);
Point newPointSph(float a, float b, float radius);

float getX(Point p);
float getY(Point p);
float getZ(Point p);

Point dupPoint(Point);
void deletePoint(Point p);

#endif // POINT