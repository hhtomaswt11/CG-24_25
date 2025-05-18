#include "../../include/Utils/XMLDataFormat.h"
#include <iostream>
#include <cstring> 
#include <functional>

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
    std::list<Light> lights;
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

Color parseColor(TiXmlElement* colorElement) {
    Color color;
    if (colorElement) {
        TiXmlElement* diffuse = colorElement->FirstChildElement("diffuse");
        if (diffuse) {
            color.diffuse[0] = atof(diffuse->Attribute("R")) / 255.0f;
            color.diffuse[1] = atof(diffuse->Attribute("G")) / 255.0f;
            color.diffuse[2] = atof(diffuse->Attribute("B")) / 255.0f;
        }
        
        TiXmlElement* ambient = colorElement->FirstChildElement("ambient");
        if (ambient) {
            color.ambient[0] = atof(ambient->Attribute("R")) / 255.0f;
            color.ambient[1] = atof(ambient->Attribute("G")) / 255.0f;
            color.ambient[2] = atof(ambient->Attribute("B")) / 255.0f;
        }
        
        TiXmlElement* specular = colorElement->FirstChildElement("specular");
        if (specular) {
            color.specular[0] = atof(specular->Attribute("R")) / 255.0f;
            color.specular[1] = atof(specular->Attribute("G")) / 255.0f;
            color.specular[2] = atof(specular->Attribute("B")) / 255.0f;
        }
        
        TiXmlElement* emissive = colorElement->FirstChildElement("emissive");
        if (emissive) {
            color.emissive[0] = atof(emissive->Attribute("R")) / 255.0f;
            color.emissive[1] = atof(emissive->Attribute("G")) / 255.0f;
            color.emissive[2] = atof(emissive->Attribute("B")) / 255.0f;
        }
        
        TiXmlElement* shininess = colorElement->FirstChildElement("shininess");
        if (shininess) {
            color.shininess = atof(shininess->Attribute("value"));
        }
    }
    return color;
}


Light parseLight(TiXmlElement* lightElement) {
    Light light;
    if (lightElement) {
        const char* type = lightElement->Attribute("type");
        if (type) light.type = type;

        if (light.type == "directional") {
            light.position[0] = lightElement->Attribute("dirx") ? atof(lightElement->Attribute("dirx")) : 0.0f;
            light.position[1] = lightElement->Attribute("diry") ? atof(lightElement->Attribute("diry")) : 0.0f;
            light.position[2] = lightElement->Attribute("dirz") ? atof(lightElement->Attribute("dirz")) : 0.0f;
            light.position[3] = 0.0f; // Directional light
        } else {
            light.position[0] = lightElement->Attribute("posx") ? atof(lightElement->Attribute("posx")) : 0.0f;
            light.position[1] = lightElement->Attribute("posy") ? atof(lightElement->Attribute("posy")) : 0.0f;
            light.position[2] = lightElement->Attribute("posz") ? atof(lightElement->Attribute("posz")) : 0.0f;
            light.position[3] = 1.0f; // Point or Spot light

            if (light.type == "spot") {
                // Spotlight-specific attributes
                light.direction[0] = lightElement->Attribute("dirx") ? atof(lightElement->Attribute("dirx")) : 0.0f;
                light.direction[1] = lightElement->Attribute("diry") ? atof(lightElement->Attribute("diry")) : -1.0f;
                light.direction[2] = lightElement->Attribute("dirz") ? atof(lightElement->Attribute("dirz")) : 0.0f;

                light.cutoff = lightElement->Attribute("cutoff") ? atof(lightElement->Attribute("cutoff")) : 30.0f;
            }
        }
    }
    return light;
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

            TiXmlElement* lightsElement = root->FirstChildElement("lights");
            if (lightsElement) {
                for (TiXmlElement* lightElement = lightsElement->FirstChildElement("light");
                     lightElement != nullptr;
                     lightElement = lightElement->NextSiblingElement("light")) {
                    result->lights.push_back(parseLight(lightElement));
                }
            }

            //Grupos
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

    for (TiXmlElement* elem = transformElement->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement()) {
        std::string elemName = elem->Value();

        if (elemName == "translate") {
            const char* timeAttr = elem->Attribute("time");

            if (timeAttr) {
                transform.hasCurve = true;
                transform.time = atof(timeAttr);

                for (TiXmlElement* point = elem->FirstChildElement("point");
                     point != nullptr;
                     point = point->NextSiblingElement("point")) {
                    float x = atof(point->Attribute("x"));
                    float y = atof(point->Attribute("y"));
                    float z = atof(point->Attribute("z"));
                    transform.controlPoints.push_back({x, y, z});
                }

                const char* alignAttr = elem->Attribute("align");
                transform.alignToCurve = (alignAttr && strcmp(alignAttr, "true") == 0);
            } else {
                TransformStep step;
                step.type = TransformType::TRANSLATE;
                step.translate[0] = elem->Attribute("x") ? atof(elem->Attribute("x")) : 0.0f;
                step.translate[1] = elem->Attribute("y") ? atof(elem->Attribute("y")) : 0.0f;
                step.translate[2] = elem->Attribute("z") ? atof(elem->Attribute("z")) : 0.0f;
                transform.orderedSteps.push_back(step);
            }
        }

        else if (elemName == "rotate") {
            const char* timeAttr = elem->Attribute("time");

            if (timeAttr) {
                transform.rotationTime = atof(timeAttr);
                transform.rotationAxis[0] = elem->Attribute("x") ? atof(elem->Attribute("x")) : 0.0f;
                transform.rotationAxis[1] = elem->Attribute("y") ? atof(elem->Attribute("y")) : 0.0f;
                transform.rotationAxis[2] = elem->Attribute("z") ? atof(elem->Attribute("z")) : 0.0f;
            } else {
                TransformStep step;
                step.type = TransformType::ROTATE;
                step.rotate[0] = elem->Attribute("angle") ? atof(elem->Attribute("angle")) : 0.0f;
                step.rotate[1] = elem->Attribute("x") ? atof(elem->Attribute("x")) : 0.0f;
                step.rotate[2] = elem->Attribute("y") ? atof(elem->Attribute("y")) : 0.0f;
                step.rotate[3] = elem->Attribute("z") ? atof(elem->Attribute("z")) : 0.0f;
                transform.orderedSteps.push_back(step);
            }
        }

        else if (elemName == "scale") {
            TransformStep step;
            step.type = TransformType::SCALE;
            step.scale[0] = elem->Attribute("x") ? atof(elem->Attribute("x")) : 1.0f;
            step.scale[1] = elem->Attribute("y") ? atof(elem->Attribute("y")) : 1.0f;
            step.scale[2] = elem->Attribute("z") ? atof(elem->Attribute("z")) : 1.0f;
            transform.orderedSteps.push_back(step);
        }
    }
}




void buildGroup(TiXmlElement* groupElement, Group& group) {
    if (!groupElement) return;

    TiXmlElement* transformElement = groupElement->FirstChildElement("transform");
    if (transformElement) {
        buildTransform(transformElement, group.transform);
    }

TiXmlElement* modelsElement = groupElement->FirstChildElement("models");
if (modelsElement) {
    for (TiXmlElement* modelElement = modelsElement->FirstChildElement(); 
         modelElement != nullptr; 
         modelElement = modelElement->NextSiblingElement()) {

        std::string tag = modelElement->Value();

        if (tag == "model") {
            Model model;
            const char* file = modelElement->Attribute("file");
            if (file) model.file = file;

            TiXmlElement* colorElement = modelElement->FirstChildElement("color");
            if (colorElement) {
                model.color = parseColor(colorElement);
            }
            TiXmlElement* textureElement = modelElement->FirstChildElement("texture");
            if (textureElement) {
                const char* texFile = textureElement->Attribute("file");
                if (texFile) model.texture = texFile;
            }

            group.models.push_back(model);
        }

        else if (tag == "texture") {
            const char* texFile = modelElement->Attribute("file");
            if (!group.models.empty() && texFile) {
                group.models.back().texture = texFile;
            }
        }

        else if (tag == "color") {
            if (!group.models.empty()) {
                group.models.back().color = parseColor(modelElement);
            }
        }
    }
}

    for (TiXmlElement* childGroupElement = groupElement->FirstChildElement("group");
         childGroupElement != nullptr;
         childGroupElement = childGroupElement->NextSiblingElement("group")) {

        Group* childGroup = new Group();
        buildGroup(childGroupElement, *childGroup);
        group.children.push_back(childGroup);
    }
}

const std::list<Model>& getModels(const Group* group) {
    static std::list<Model> empty;
    return group ? group->models : empty;
}

std::list<std::string> getAllModelFiles(const XMLDataFormat* data) {
    std::list<std::string> modelFiles;
    if (!data) return modelFiles;

    auto collectModels = [&modelFiles](const Group* group, auto&& self) -> void {
        for (const auto& model : group->models) {
            modelFiles.push_back(model.file);
        }
        for (const auto* child : group->children) {
            self(child, self);
        }
    };

    collectModels(&data->rootGroup, collectModels);
    return modelFiles;
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

const Group* getRootGroup(const XMLDataFormat* data) {
    return &data->rootGroup;
}

const std::list<Group*>& getChildren(const Group* group) {
    return group->children;
}

const Transform* getTransform(const Group* group) {
    return &group->transform;
}


const std::list<Model>& getGroupModels(const Group* group) {
    static std::list<Model> emptyList; 
    return group ? group->models : emptyList;
}

const Color* getModelColor(const Model* model) {
    return model ? &(model->color) : nullptr;
}

const std::string& getModelTexture(const Model* model) {
    static std::string emptyString; 
    return model ? model->texture : emptyString;
}

const std::list<Light>& getLights(const XMLDataFormat* data) {
    static std::list<Light> emptyList; 
    return data ? data->lights : emptyList;
}

// DESTROYER 

void deleteXMLDataFormat(XMLDataFormat* data) {
    if (data) {
        delete data;  
    }
}

