#ifndef BEZIER_H
#define BEZIER_H

#include <cstring>
#include <string>
#include <vector>

#include "../Utils/point.h"

typedef struct Patch patch; 

    void readPatchFile(std::string filename, std::vector<Patch> &patches, std::vector<Point> &controlPoints) ;
    float bezierBlend(const float m[4][4], float t, int row) ;
    Point computeBezierPoint(float u, float v, Point patchPoints[4][4]) ;
    void buildBezierPatchPrimitive(std::string patchFile, int tessellation, std::string outputFile) ;

#endif 
