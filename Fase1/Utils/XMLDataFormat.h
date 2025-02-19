#ifndef XMLDATAFORMAT
#define XMLDATAFORMAT

#include <list>
#include <string>


typedef struct XMLDataFormat xmlDataFormat; 
typedef struct PosCamera camera; 
typedef struct LookAt lookat; 
typedef struct Up up; 
typedef struct Projection projection; 
typedef struct Window window; 

// Function declarations
XMLDataFormat* newXMLDataFormat();
XMLDataFormat* xmlToXMLDataFormat(const char* filePath);
std::list<std::string>& getModels(XMLDataFormat* data);
void setCamPosition(XMLDataFormat* data, float x, float y, float z);
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
void deleteXMLDataFormat(XMLDataFormat* data);

#endif 
