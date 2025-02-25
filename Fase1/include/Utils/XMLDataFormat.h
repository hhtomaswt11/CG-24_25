#ifndef XMLDATAFORMAT
#define XMLDATAFORMAT

#include <list>
#include <string>
#include "../../tinyXML/tinyxml.h"
#include "../../tinyXML/tinystr.h"

typedef struct XMLDataFormat xmlDataFormat; 
typedef struct PosCamera camera; 
typedef struct LookAt lookat; 
typedef struct Up up; 
typedef struct Projection projection; 
typedef struct Window window; 


XMLDataFormat* newXMLDataFormat();
XMLDataFormat* xmlToXMLDataFormat(const char* filePath);

std::list<std::string>& getModels(XMLDataFormat* data);
const std::list<std::string>& getModels(const XMLDataFormat* data) ; 

// SETTER 
void setCamPosition(XMLDataFormat* data, float x, float y, float z);

// GETTERS 
int getWidth(XMLDataFormat* data); 
int getHeight(XMLDataFormat* data); 

float getXPosCam(XMLDataFormat* data);
float getYPosCam(XMLDataFormat* data);
float getZPosCam(XMLDataFormat* data);

float getXLookAt(XMLDataFormat* data);
float getYLookAt(XMLDataFormat* data);
float getZLookAt(XMLDataFormat* data);

float getXUp(XMLDataFormat* data);
float getYUp(XMLDataFormat* data);
float getZUp(XMLDataFormat* data);

float getFov(XMLDataFormat* data) ; 
float getNear(XMLDataFormat* data) ; 
float getFar(XMLDataFormat* data) ; 


void buildPosCamera(TiXmlElement* posCamera, PosCamera& pos) ; 
void buildLookAtCamera(TiXmlElement* lookAtCamera, LookAt& lookAt); 
void buildUpCamera(TiXmlElement* upCamera, Up& up); 
void buildProjectionCamera(TiXmlElement* projectionCamera, Projection& projection); 

void deleteXMLDataFormat(XMLDataFormat* data);

#endif 
