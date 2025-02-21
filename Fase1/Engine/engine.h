#ifndef ENGINE_H
#define ENGINE_H



#include "../Utils/primitive.h"
#include "../Utils/point.h"
#include "../Utils/XMLDataFormat.h"
#include "../tinyXML/tinyxml.h"
#include "math.h"
#include <list>

void changeSize(int w, int h) ; 
void drawPrimitives(const std::list<std::string> figs) ;
void renderScene(void) ; 
void keyProc(unsigned char key, int, int) ; 


#endif //ENGINE_H
