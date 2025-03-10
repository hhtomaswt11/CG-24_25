#ifndef XMLDATAFORMAT
#define XMLDATAFORMAT

#include <list>
#include <string>
#include "../../tinyXML/tinyxml.h"
#include "../../tinyXML/tinystr.h"

// Define all structures
struct Window {
    int width, height;
};

struct PosCamera {
    float cam1, cam2, cam3;
};

struct LookAt {
    float lookat1, lookat2, lookat3;
};

struct Up {
    float up1, up2, up3;
};

struct Projection {
    float fov, near, far;
};

struct Transform {
    float translate[3] = {0.0f, 0.0f, 0.0f}; // x, y, z
    float rotate[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // angle, x, y, z
    float scale[3] = {1.0f, 1.0f, 1.0f}; // x, y, z
};

struct Group {
    Transform transform;
    std::list<std::string> models;
    std::list<Group> children;
};

struct XMLDataFormat {
    Window window;
    PosCamera poscamera;
    LookAt lookat;
    Up up;
    Projection projection;
    std::list<std::string> models;
    Group rootGroup;
};

// Function declarations
XMLDataFormat* newXMLDataFormat();
XMLDataFormat* xmlToXMLDataFormat(const char* filePath);

std::list<std::string>& getModels(XMLDataFormat* data);
const std::list<std::string>& getModels(const XMLDataFormat* data);

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

float getFov(XMLDataFormat* data);
float getNear(XMLDataFormat* data);
float getFar(XMLDataFormat* data);

void buildPosCamera(TiXmlElement* posCamera, PosCamera& pos);
void buildLookAtCamera(TiXmlElement* lookAtCamera, LookAt& lookAt);
void buildUpCamera(TiXmlElement* upCamera, Up& up);
void buildProjectionCamera(TiXmlElement* projectionCamera, Projection& projection);

void buildTransform(TiXmlElement* transformElement, Transform& transform);
void buildGroup(TiXmlElement* groupElement, Group& group);

void deleteXMLDataFormat(XMLDataFormat* data);

#endif