#ifndef ENGINE_H
#define ENGINE_H



#include <list>
#include <GL/glut.h>
#include <iostream>
#include <list>
#include <cmath>
#include "../Utils/XMLDataFormat.h" 
#include "../Utils/primitive.h"
#include "../../tinyXML/tinyxml.h"
#include "../UI/ui.h"

void changeSize(int w, int h) ; 
void drawPrimitives(const std::list<std::string> figs) ;
void renderScene() ; 
void keyProc(unsigned char key, int, int) ; 
void updateCameraPosition(void) ;
void loadXMLFile(const char *filename) ;
void initializeCameraAndWindow(XMLDataFormat* xmlData);
void applyTransform(const Transform& transform);
void renderGroup(const Group& group);

extern XMLDataFormat* xmlData;


#endif //ENGINE_H
