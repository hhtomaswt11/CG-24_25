#include "../../include/Utils/point.h"

struct point{
    float x, y, z;
};

Point buildPoint(float x, float y, float z){
    Point p = (Point)malloc(sizeof(struct point));
    if(p != NULL){
        p->x = x; p->y = y; p->z = z;
    }
    return p;
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

void freePoint(Point p){
    free(p);
}
