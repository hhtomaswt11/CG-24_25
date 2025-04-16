#include "../../include/Utils/XMLDataFormat.h"
#include <iostream>

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

struct XMLDataFormat {
    Window window;
    PosCamera poscamera;
    LookAt lookat;
    Up up;
    Projection projection;
    std::list<std::string> models;
    Group rootGroup;
};

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

// parser 
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
    if (!transformElement) return;

    // --- TRANSLATE ---
    TiXmlElement* translateElement = transformElement->FirstChildElement("translate");
    if (translateElement) {
        const char* timeAttr = translateElement->Attribute("time");

        if (timeAttr) {
            transform.hasCurve = true;
            transform.time = atof(timeAttr);  // Time for Catmull-Rom animation

            // Read control points for Catmull-Rom path
            for (TiXmlElement* point = translateElement->FirstChildElement("point");
                 point != nullptr;
                 point = point->NextSiblingElement("point")) {

                float x = atof(point->Attribute("x"));
                float y = atof(point->Attribute("y"));
                float z = atof(point->Attribute("z"));
                transform.controlPoints.push_back({x, y, z});
            }

            // Align attribute (optional)
            const char* alignAttr = translateElement->Attribute("align");
            transform.alignToCurve = (alignAttr && strcmp(alignAttr, "true") == 0);
        } else {
            // Static translation
            transform.translate[0] = translateElement->Attribute("x") ? atof(translateElement->Attribute("x")) : 0.0f;
            transform.translate[1] = translateElement->Attribute("y") ? atof(translateElement->Attribute("y")) : 0.0f;
            transform.translate[2] = translateElement->Attribute("z") ? atof(translateElement->Attribute("z")) : 0.0f;
        }
    }

    // --- ROTATE ---
    TiXmlElement* rotateElement = transformElement->FirstChildElement("rotate");
    if (rotateElement) {
        const char* timeAttr = rotateElement->Attribute("time");

        if (timeAttr) {
            // Animated rotation
            transform.rotationTime = atof(timeAttr);
            transform.rotationAxis[0] = rotateElement->Attribute("x") ? atof(rotateElement->Attribute("x")) : 0.0f;
            transform.rotationAxis[1] = rotateElement->Attribute("y") ? atof(rotateElement->Attribute("y")) : 0.0f;
            transform.rotationAxis[2] = rotateElement->Attribute("z") ? atof(rotateElement->Attribute("z")) : 0.0f;
        } else {
            // Static rotation
            transform.rotate[0] = rotateElement->Attribute("angle") ? atof(rotateElement->Attribute("angle")) : 0.0f;
            transform.rotate[1] = rotateElement->Attribute("x") ? atof(rotateElement->Attribute("x")) : 0.0f;
            transform.rotate[2] = rotateElement->Attribute("y") ? atof(rotateElement->Attribute("y")) : 0.0f;
            transform.rotate[3] = rotateElement->Attribute("z") ? atof(rotateElement->Attribute("z")) : 0.0f;
        }
    }

    // --- SCALE ---
    TiXmlElement* scaleElement = transformElement->FirstChildElement("scale");
    if (scaleElement) {
        transform.scale[0] = scaleElement->Attribute("x") ? atof(scaleElement->Attribute("x")) : 1.0f;
        transform.scale[1] = scaleElement->Attribute("y") ? atof(scaleElement->Attribute("y")) : 1.0f;
        transform.scale[2] = scaleElement->Attribute("z") ? atof(scaleElement->Attribute("z")) : 1.0f;
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
             modelElement != nullptr;
             modelElement = modelElement->NextSiblingElement("model")) {

            const char* file = modelElement->Attribute("file");
            if (file) {
                group.models.push_back(file);
            }
        }
    }

    for (TiXmlElement* childGroupElement = groupElement->FirstChildElement("group");
         childGroupElement != nullptr;
         childGroupElement = childGroupElement->NextSiblingElement("group")) {

        Group childGroup;
        buildGroup(childGroupElement, childGroup);
        group.children.push_back(new Group(childGroup));
    }
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

const float* getRotate(const Transform* t) {
    return t->rotate;
}

const float* getTranslate(const Transform* t) {
    return t->translate;
}

const float* getScale(const Transform* t) {
    return t->scale;
}


const Group* getRootGroup(const XMLDataFormat* data) {
    return &data->rootGroup;
}

const std::list<Group*>& getChildren(const Group* group) {
    return group->children;
}

const std::list<std::string>& getModels(const Group* group) {
    return group->models;
}

const Transform* getTransform(const Group* group) {
    return &group->transform;
}

// GET MODELS 

std::list<std::string>& getModels(XMLDataFormat* data) {
    if (data) {
        return data->models;
    }
    static std::list<std::string> emptyList; 
    return emptyList;
}


// const std::list<std::string>& getModels(const XMLDataFormat* data) {
//     if (data) {
//         return data->models;
//     }
//     static const std::list<std::string> emptyList;
//     return emptyList;
// }

std::list<std::string> getAllModels(const Group* group) {
    std::list<std::string> allModels;

    // Add models from the current group
    for (const auto& model : group->models) {
        allModels.push_back(model);
    }

    // Recursively get models from child groups
    for (const auto* childGroup : group->children) {
        auto childModels = getAllModels(childGroup);  // Recursive call for child groups
        allModels.insert(allModels.end(), childModels.begin(), childModels.end());
    }

    return allModels;
}

std::list<std::string> getAllModels(const XMLDataFormat* data) {
    return getAllModels(&data->rootGroup);  // Start recursion from the root group
}


// DESTROYER 

void deleteXMLDataFormat(XMLDataFormat* data) {
    if (data) {
        data->models.clear(); 
        delete data;  // apagar a struct em si 
    }
}

// void deleteGroup(Group* group) {
//     for (auto* child : group->children) {
//         deleteGroup(child);
//     }
//     delete group;
// }
