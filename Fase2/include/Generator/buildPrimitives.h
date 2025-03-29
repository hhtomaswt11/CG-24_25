#ifndef BUILDPRIMITIVES_H
#define BUILDPRIMITIVES_H

#include <cstring>

#include "../Utils/primitive.h"

 Primitive buildPlane(int length, int divisions, char axis , float h , bool invertFaces , bool invertDiagonal); ; 

 Primitive buildBox(int length, int divisions); 

 Primitive buildSphere(int radius, int slices, int stacks);
 
 Primitive buildCone(int radius, int height, int slices, int stacks) ;

 Primitive buildSaturnRing(float innerRadius, float outerRadius, float height , int slices, int stacks);

#endif 
