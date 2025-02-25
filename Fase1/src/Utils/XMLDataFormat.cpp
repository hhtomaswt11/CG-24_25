// #include <tinyxml.h>
// #include <tinystr.h>

#include "../../include/Utils/XMLDataFormat.h"
#include "../../tinyXML/tinyxml.h"
#include "../../tinyXML/tinystr.h"
#include <cstdlib>
#include <iostream>

struct Window{
    int width, height;
};  

struct PosCamera{ // valores inteiros ou manter float ? 
    float cam1, cam2, cam3; 
}; 

struct LookAt{
    float lookat1, lookat2, lookat3; 
}; 


struct Up{
    float up1, up2, up3; 
}; 

struct Projection{ // pode tomar valores float 
    float fov, near, far; 
}; 

struct XMLDataFormat {
    Window window; 
    PosCamera poscamera; 
    LookAt lookat; 
    Up up; 
    Projection projection; 

    std::list<std::string> models; // List of model file paths

};

XMLDataFormat* newXMLDataFormat() {
    XMLDataFormat* newData = new XMLDataFormat();
    if (newData) {
        // Inicializa os valores da estrutura com 0.0f
        newData->window = {512, 512}; 
        newData->poscamera = {0.0f, 0.0f, 0.0f};
        newData->lookat = {0.0f, 0.0f, 0.0f};
        newData->up = {0.0f, 0.0f, 0.0f};
        newData->projection = {0.0f, 0.0f, 0.0f};

        // A lista de modelos já é inicializada vazia por padrão
    }
    return newData;
}


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

              // Parametros da janela - leitura 
              TiXmlElement* windowElement = root->FirstChildElement("window");
              if (windowElement) {
                  result->window.width = atoi(windowElement->Attribute("width"));
                  result->window.height = atoi(windowElement->Attribute("height"));
              }

            // Parametros da camera 
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

            // Parsing models! 
            TiXmlElement* group = root->FirstChildElement("group");
            if (group) {
                TiXmlElement* models = group->FirstChildElement("models");
                if (models) {
                    for (TiXmlElement* model = models->FirstChildElement("model"); model; model = model->NextSiblingElement("model")) {
                        const char* file = model->Attribute("file");
                        if (file) {
                            result->models.push_back(file);
                        }
                    }   
                }
            }
        } else {
            std::cerr << "Error loading XML file: " << filePath << std::endl;
        }
    }
    return result;
}


std::list<std::string>& getModels(XMLDataFormat* data) {
    if (data) {
        return data->models;
    }
    static std::list<std::string> emptyList; 
    return emptyList;
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

int getWidth(XMLDataFormat* data) {
    return data ? data->window.width : 0.;
}

int getHeight(XMLDataFormat* data) {
    return data ? data->window.height: 0;
}

// Funções para obter a posição da câmera
float getXPosCam(XMLDataFormat* data) {
    return data ? data->poscamera.cam1 : 0.0f;
}

float getYPosCam(XMLDataFormat* data) {
    return data ? data->poscamera.cam2 : 0.0f;
}

float getZPosCam(XMLDataFormat* data) {
    return data ? data->poscamera.cam3 : 0.0f;
}

// Funções para obter os valores do LookAt
float getXLookAt(XMLDataFormat* data) {
    return data ? data->lookat.lookat1 : 0.0f;
}

float getYLookAt(XMLDataFormat* data) {
    return data ? data->lookat.lookat2 : 0.0f;
}

float getZLookAt(XMLDataFormat* data) {
    return data ? data->lookat.lookat3 : 0.0f;
}

// Funções para obter os valores do Up
float getXUp(XMLDataFormat* data) {
    return data ? data->up.up1 : 0.0f;
}

float getYUp(XMLDataFormat* data) {
    return data ? data->up.up2 : 0.0f;
}

float getZUp(XMLDataFormat* data) {
    return data ? data->up.up3 : 0.0f;
}

float getFov(XMLDataFormat* data) {
    return data ? data->projection.fov : 0.0f;
}
float getNear(XMLDataFormat* data) {
    return data ? data->projection.near : 0.0f;
}
float getFar(XMLDataFormat* data) {
    return data ? data->projection.far : 0.0f;
}

const std::list<std::string>& getModels(const XMLDataFormat* data) {
    if (data) {
        return data->models;
    }
    // Retorna uma lista vazia se `data` for nulo
    static const std::list<std::string> emptyList;
    return emptyList;
}


void deleteXMLDataFormat(XMLDataFormat* data) {
    if (data) {
        // Since std::list automatically handles memory management, we don't need to manually free the items
        data->models.clear(); // Clears the list
        delete data;  // Delete the struct itself
    }
}
