#ifndef ENGINE_H
#define ENGINE_H



#include "../Utils/primitive.h"
#include "../Utils/point.h"
#include "../Utils/XMLDataFormat.h"
#include "../tinyXML/tinyxml.h"
#include <list>
#include <GL/glut.h>
#include <iostream>
#include <list>
#include <cmath>

void changeSize(int w, int h) ; 
void drawPrimitives(const std::list<std::string> figs) ;
void renderScene(void) ; 
void keyProc(unsigned char key, int, int) ; 
void updateCameraPosition(void) ;
void loadXMLFile(const char *filename) ;


#endif //ENGINE_H
