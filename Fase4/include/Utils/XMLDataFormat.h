#ifndef XMLDATAFORMAT
#define XMLDATAFORMAT

#include <list>
#include <string>
#include <vector>
#include <array>
#include "../../tinyXML/tinyxml.h"
#include "../../tinyXML/tinystr.h"

typedef struct Window window; 
typedef struct PosCamera posCamera; 
typedef struct LookAt lookAt; 
typedef struct Up up; 
typedef struct Projection projection; 

struct Color {
    float diffuse[3] = {0.8f, 0.8f, 0.8f};
    float ambient[3] = {0.2f, 0.2f, 0.2f};
    float specular[3] = {0.0f, 0.0f, 0.0f};
    float emissive[3] = {0.0f, 0.0f, 0.0f};
    float shininess = 0.0f;
    std::string textureFile;
};

struct Model {
    std::string file;
    Color color;
    std::string texture; // Add texture support
};

struct Transform {
    bool hasCurve = false;
    bool alignToCurve = false;
    float time = 0.0f; // for translate animation only
    std::vector<std::array<float, 3>> controlPoints;

    float translate[3];
    float rotate[4]; // static rotation: angle, x, y, z
    float scale[3];

    float rotationTime = 0.0f;
    float rotationAxis[3] = {0.0f, 0.0f, 0.0f};

    Transform() {
        translate[0] = translate[1] = translate[2] = 0.0f;
        rotate[0] = rotate[1] = rotate[2] = rotate[3] = 0.0f;
        scale[0] = scale[1] = scale[2] = 1.0f;
    }
};

//////////////////New Feat: Lights
struct Light {
    std::string type = "directional";
    float position[4] = {0,0,0,1};
    float direction[3] = {1,0.7,0.5}; // For directional and spot lights
    float cutoff = 45.0f; // For spot lights
    float ambient[3] = {0.2f, 0.2f, 0.2f};
    float diffuse[3] = {1.0f, 1.0f, 1.0f};
    float specular[3] = {1.0f, 1.0f, 1.0f};
};
//////////////////////////////////

struct Group {
    Transform transform;
    std::list<Model> models;
    std::list<Group*> children; 

    ~Group() {
        for (auto* child : children) {
            delete child;
        }
    }
};
typedef struct XMLDataFormat xmlDataFormat; 


// BUILDERS 

XMLDataFormat* newXMLDataFormat();
XMLDataFormat* xmlToXMLDataFormat(const char* filePath);

// SETTER
void setCamPosition(XMLDataFormat* data, float x, float y, float z);

// GETTERS

////////////////lights
const std::list<Light>& getLights(const XMLDataFormat* data);
/////////////////////

const std::list<Model>& getModels(const Group* group);
std::list<std::string> getAllModelFiles(const XMLDataFormat* data);


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

const float* getRotate(const Transform* t);
const float* getTranslate(const Transform* t);
const float* getScale(const Transform* t);

const Group* getRootGroup(const XMLDataFormat* data);
const std::list<Group*>& getChildren(const Group* group);
const Transform* getTransform(const Group* group);

//new getters
const std::list<Light>& getLights(const XMLDataFormat* data);
const std::list<Model>& getGroupModels(const Group* group);
const Color* getModelColor(const Model* model);
const std::string& getModelTexture(const Model* model);

// BUILDERS
void buildPosCamera(TiXmlElement* posCamera, PosCamera& pos);
void buildLookAtCamera(TiXmlElement* lookAtCamera, LookAt& lookAt);
void buildUpCamera(TiXmlElement* upCamera, Up& up);
void buildProjectionCamera(TiXmlElement* projectionCamera, Projection& projection);
void buildTransform(TiXmlElement* transformElement, Transform& transform);
void buildGroup(TiXmlElement* groupElement, Group& group);

// Material parsing
Color parseColor(TiXmlElement* colorElement);
// Light parsing
Light parseLight(TiXmlElement* lightElement);
// DESTROYER
void deleteXMLDataFormat(XMLDataFormat* data);
void deleteGroup(Group* group); 


#endif