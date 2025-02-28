#ifndef BUILDPRIMITIVES_H
#define BUILDPRIMITIVES_H

#include <cstring>
#include "../Utils/primitive.h"


 Primitive buildPlane(int length, int divisions, char axis , float h , int invert) ; 

 Primitive buildBox(int length, int divisions); 

 Primitive buildSphere(int radius, int slices, int stacks);
 
#endif 
