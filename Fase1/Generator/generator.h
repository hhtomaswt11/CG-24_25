#ifndef GENERATOR_H
#define GENERATOR_H

#include <cstring>
#include "../Utils/primitive.h"

 // Primitive buildPlane(int length, int divisions, float h, int y);

 Primitive buildPlane(int length, int divisions, char axis , float h , int invert) ; 

 Primitive buildBox(int length, int divisions); 
 
#endif 
