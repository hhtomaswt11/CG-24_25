#include <iostream>
#include "primitive.h"  // Replace with your correct include
#include "point.h"      // Replace with your correct include
//Teste
int main() {
    // Test the functionality of your Utils code (primitive, point)
    Point p1 = newPoint(1.0f, 2.0f, 3.0f);
    Point p2 = newPoint(4.0f, 5.0f, 6.0f);

    Primitive prim = newEmptyPrimitive();
    addPoint(prim, p1);
    addPoint(prim, p2);

    std::cout << "Primitive points: " << getPoints(prim).size() << std::endl;


    // Clean up
    deletePrimitive(prim);

    return 0;
}
