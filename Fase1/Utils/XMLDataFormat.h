#ifndef XMLDATAFORMAT
#define XMLDATAFORMAT

#include <list>
#include <string>

struct XMLDataFormat {
    // Camera
    float poscam[3];
    float lookAt[3];
    float up[3];
    float projection[3]; // fov, near, far
    std::list<std::string> models; // List of model file paths
};

// Function declarations
XMLDataFormat* newXMLDataFormat();
XMLDataFormat* xmlToXMLDataFormat(const char* filePath);
std::list<std::string>& getModels(XMLDataFormat* data);
void setCamPosition(XMLDataFormat* data, float x, float y, float z);
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

#endif // XMLDATAFORMAT
