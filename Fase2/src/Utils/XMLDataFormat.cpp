// #include <tinyxml.h>
// #include <tinystr.h>

#include "../../include/Utils/XMLDataFormat.h"
#include <cstdlib>
#include <iostream>

XMLDataFormat* newXMLDataFormat() {
    XMLDataFormat* newData = new XMLDataFormat();
    if (newData) {
        newData->window = {512, 512}; 
        newData->poscamera = {0.0f, 0.0f, 0.0f};
        newData->lookat = {0.0f, 0.0f, 0.0f};
        newData->up = {0.0f, 0.0f, 0.0f};
        newData->projection = {0.0f, 0.0f, 0.0f};
    }
    return newData;
}


// BUILDERS 

void buildPosCamera(TiXmlElement* posCamera, PosCamera& pos) {
        pos.cam1 = atof(posCamera->Attribute("x"));
        pos.cam2 = atof(posCamera->Attribute("y"));
        pos.cam3 = atof(posCamera->Attribute("z"));
}

void buildLookAtCamera(TiXmlElement* lookAtCamera, LookAt& lookAt) {
        lookAt.lookat1 = atof(lookAtCamera->Attribute("x"));
        lookAt.lookat2 = atof(lookAtCamera->Attribute("y"));
        lookAt.lookat3 = atof(lookAtCamera->Attribute("z"));
}

void buildUpCamera(TiXmlElement* upCamera, Up& up) {
        up.up1 = atof(upCamera->Attribute("x"));
        up.up2 = atof(upCamera->Attribute("y"));
        up.up3 = atof(upCamera->Attribute("z"));
}

void buildProjectionCamera(TiXmlElement* projectionCamera, Projection& projection) {
        projection.fov = atof(projectionCamera->Attribute("fov"));
        projection.near = atof(projectionCamera->Attribute("near"));
        projection.far = atof(projectionCamera->Attribute("far"));
}

void buildTransform(TiXmlElement* transformElement, Transform& transform) {
    if (transformElement) {
        TiXmlElement* translateElement = transformElement->FirstChildElement("translate");
        if (translateElement) {
            transform.translate[0] = atof(translateElement->Attribute("x"));
            transform.translate[1] = atof(translateElement->Attribute("y"));
            transform.translate[2] = atof(translateElement->Attribute("z"));
        }

        TiXmlElement* rotateElement = transformElement->FirstChildElement("rotate");
        if (rotateElement) {
            transform.rotate[0] = atof(rotateElement->Attribute("angle"));
            transform.rotate[1] = atof(rotateElement->Attribute("x"));
            transform.rotate[2] = atof(rotateElement->Attribute("y"));
            transform.rotate[3] = atof(rotateElement->Attribute("z"));
        }

        TiXmlElement* scaleElement = transformElement->FirstChildElement("scale");
        if (scaleElement) {
            transform.scale[0] = atof(scaleElement->Attribute("x"));
            transform.scale[1] = atof(scaleElement->Attribute("y"));
            transform.scale[2] = atof(scaleElement->Attribute("z"));
        }
    }
}

void buildGroup(TiXmlElement* groupElement, Group& group) {
    TiXmlElement* transformElement = groupElement->FirstChildElement("transform");
    if (transformElement) {
        buildTransform(transformElement, group.transform);
    }

    TiXmlElement* modelsElement = groupElement->FirstChildElement("models");
    if (modelsElement) {
        for (TiXmlElement* modelElement = modelsElement->FirstChildElement("model");
             modelElement != nullptr; modelElement = modelElement->NextSiblingElement("model")) {
            const char* file = modelElement->Attribute("file");
            if (file) {
                group.models.push_back(file);  // Add the model file name to the group's models.
            }
        }
    }

    // Recursively handle child groups
    for (TiXmlElement* childGroupElement = groupElement->FirstChildElement("group");
         childGroupElement != nullptr; childGroupElement = childGroupElement->NextSiblingElement("group")) {
        Group childGroup;
        buildGroup(childGroupElement, childGroup);
        group.children.push_back(childGroup);  // Add child group to the current group
    }
}

XMLDataFormat* xmlToXMLDataFormat(const char* filePath) {
    XMLDataFormat* result = newXMLDataFormat();
    if (result) {
        TiXmlDocument doc;
        if (doc.LoadFile(filePath)) {
            TiXmlElement* root = doc.FirstChildElement("world");

            if (!root) {
                std::cerr << "Error: <world> element not found in XML file: " << filePath << std::endl;
                return result;
            }

              // parametros da janela - leitura 
              TiXmlElement* windowElement = root->FirstChildElement("window");
              if (windowElement) {
                  result->window.width = atoi(windowElement->Attribute("width"));
                  result->window.height = atoi(windowElement->Attribute("height"));
              }

            // parametros da camera 
            TiXmlElement* camera = root->FirstChildElement("camera");
            if (camera) {
                TiXmlElement* posCamera = camera->FirstChildElement("position");
                TiXmlElement* lookAtCamera = camera->FirstChildElement("lookAt");
                TiXmlElement* upCamera = camera->FirstChildElement("up");
                TiXmlElement* projectionCamera = camera->FirstChildElement("projection");

                if (posCamera && lookAtCamera && upCamera && projectionCamera){ buildPosCamera(posCamera, result->poscamera);
                buildLookAtCamera(lookAtCamera, result->lookat); 
                buildUpCamera(upCamera, result->up); 
                buildProjectionCamera(projectionCamera, result->projection); 
                }
                else{
                    std::cerr << "Error: Invalid camera parameters "<< std::endl;
                }
        }

             // Parse root group
            TiXmlElement* groupElement = root->FirstChildElement("group");
            if (groupElement) {
                buildGroup(groupElement, result->rootGroup);
            }
        } else {
            std::cerr << "XML file does not exist. Error loading XML file: " << filePath << std::endl;
            exit(1);
        }
    }
    return result;
}

// SETTERS 

void setCamPosition(XMLDataFormat* data, float x, float y, float z) {
    if (data) {
        data->poscamera.cam1 = x;
        data->poscamera.cam2 = y;
        data->poscamera.cam3 = z;
    }
}


// GETTERS

// CAMERA
int getWidth(XMLDataFormat* data) {
    return data ? data->window.width : 0.;
}

int getHeight(XMLDataFormat* data) {
    return data ? data->window.height: 0;
}

// COORDINATES 
float getXPosCam(XMLDataFormat* data) {
    return data ? data->poscamera.cam1 : 0.0f;
}

float getYPosCam(XMLDataFormat* data) {
    return data ? data->poscamera.cam2 : 0.0f;
}

float getZPosCam(XMLDataFormat* data) {
    return data ? data->poscamera.cam3 : 0.0f;
}

// LOOKAT 
float getXLookAt(XMLDataFormat* data) {
    return data ? data->lookat.lookat1 : 0.0f;
}

float getYLookAt(XMLDataFormat* data) {
    return data ? data->lookat.lookat2 : 0.0f;
}

float getZLookAt(XMLDataFormat* data) {
    return data ? data->lookat.lookat3 : 0.0f;
}

// UP 
float getXUp(XMLDataFormat* data) {
    return data ? data->up.up1 : 0.0f;
}

float getYUp(XMLDataFormat* data) {
    return data ? data->up.up2 : 0.0f;
}

float getZUp(XMLDataFormat* data) {
    return data ? data->up.up3 : 0.0f;
}

// PROJECTION 
float getFov(XMLDataFormat* data) {
    return data ? data->projection.fov : 0.0f;
}
float getNear(XMLDataFormat* data) {
    return data ? data->projection.near : 0.0f;
}
float getFar(XMLDataFormat* data) {
    return data ? data->projection.far : 0.0f;
}

// GET MODELS 

std::list<std::string>& getModels(XMLDataFormat* data) {
    if (data) {
        return data->models;
    }
    static std::list<std::string> emptyList; 
    return emptyList;
}


const std::list<std::string>& getModels(const XMLDataFormat* data) {
    if (data) {
        return data->models;
    }
    static const std::list<std::string> emptyList;
    return emptyList;
}



// DESTROYER 

void deleteXMLDataFormat(XMLDataFormat* data) {
    if (data) {
        data->models.clear(); 
        delete data;  // apagar a struct em si 
    }
}
