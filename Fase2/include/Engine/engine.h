#ifndef ENGINE_H
#define ENGINE_H

#include <list>
#include <GL/glut.h>

#include "../Utils/XMLDataFormat.h" 
#include "../Utils/primitive.h"
#include "../../tinyXML/tinyxml.h"
#include "../UI/ui.h"

void changeSize(int w, int h) ; 
void drawPrimitives(const std::list<std::string> figs) ;
void renderScene() ; 
void processKeys(unsigned char key, int, int) ; 
void processSpecialKeys(int key, int , int ) ; 
void updateCameraPosition(void) ;
void alternate_color(float, float , float); 
void initializeCameraAndWindow(XMLDataFormat* xmlData);
void applyTransform(const Transform& transform);
void renderGroup(const Group& group);
void showCameraPosition(); 

#endif 
