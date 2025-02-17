#include "point.h"

struct point{
    float x, y, z;
};

Point newEmptyPoint(){
    return newPoint(0.0f, 0.0f, 0.0f);
}

Point newPoint(float x, float y, float z){
    Point p = (Point)malloc(sizeof(struct point));
    if(p != NULL){
        p->x = x; p->y = y; p->z = z;
    }
    return p;
}

Point newPointSph(float a, float b, float radius){
    float z = radius * cos(b) * cos(a);
    float x = radius * cos(b) * sin(a);
    float y = radius * sin(b);

    return newPoint(x, y, z);
}

float getX(Point p){
    if(p){
        return p->x;
    }
    return 0.0f;
}

float getY(Point p){
    if(p){
        return p->y;
    }
    return 0.0f;
}

float getZ(Point p){
    if(p){
        return p->z;
    }
    return 0.0f;
}

Point dupPoint(Point point){
    return newPoint(point->x, point->y, point->z);
}

void deletePoint(Point p){
    free(p);
}