#include "XMLDataFormat.h"
#include "../tinyXML/tinyxml.h"
#include <cstdlib>
#include <iostream>

XMLDataFormat* newXMLDataFormat() {
    XMLDataFormat* newData = new XMLDataFormat();
    if (newData) {
        // Initialize any values you need, if required
        std::fill(std::begin(newData->poscam), std::end(newData->poscam), 0.0f);
        std::fill(std::begin(newData->lookAt), std::end(newData->lookAt), 0.0f);
        std::fill(std::begin(newData->up), std::end(newData->up), 0.0f);
        std::fill(std::begin(newData->projection), std::end(newData->projection), 0.0f);
    }
    return newData;
}

XMLDataFormat* xmlToXMLDataFormat(const char* filePath) {
    XMLDataFormat* result = newXMLDataFormat();
    if (result) {
        TiXmlDocument doc;
        if (doc.LoadFile(filePath)) {
            TiXmlElement* root = doc.FirstChildElement("world");

            // Camera parameters
            TiXmlElement* camera = root->FirstChildElement("camera");
            TiXmlElement* posCamera = camera->FirstChildElement("position");
            TiXmlElement* lookAtCamera = camera->FirstChildElement("lookAt");
            TiXmlElement* upCamera = camera->FirstChildElement("up");
            TiXmlElement* projectionCamera = camera->FirstChildElement("projection");

            result->poscam[0] = atof(posCamera->Attribute("x"));
            result->poscam[1] = atof(posCamera->Attribute("y"));
            result->poscam[2] = atof(posCamera->Attribute("z"));

            result->lookAt[0] = atof(lookAtCamera->Attribute("x"));
            result->lookAt[1] = atof(lookAtCamera->Attribute("y"));
            result->lookAt[2] = atof(lookAtCamera->Attribute("z"));

            result->up[0] = atof(upCamera->Attribute("x"));
            result->up[1] = atof(upCamera->Attribute("y"));
            result->up[2] = atof(upCamera->Attribute("z"));

            result->projection[0] = atof(projectionCamera->Attribute("fov"));
            result->projection[1] = atof(projectionCamera->Attribute("near"));
            result->projection[2] = atof(projectionCamera->Attribute("far"));

            // Parsing models
            TiXmlElement* group = root->FirstChildElement("group");
            TiXmlElement* models = group->FirstChildElement("models");
            for (TiXmlElement* model = models->FirstChildElement("model"); model; model = model->NextSiblingElement("model")) {
                result->models.push_back(model->Attribute("file"));
            }
        } else {
            std::cerr << "Error loading XML file: " << filePath << std::endl;
        }
    }
    return result;
}

// You should return a reference to the models list, but only if the XMLDataFormat instance is valid
std::list<std::string>& getModels(XMLDataFormat* data) {
    if (data) {
        return data->models;
    }
    static std::list<std::string> emptyList; // Return an empty list if data is null
    return emptyList;
}

void setCamPosition(XMLDataFormat* data, float x, float y, float z) {
    if (data) {
        data->poscam[0] = x;
        data->poscam[1] = y;
        data->poscam[2] = z;
    }
}

float getXPosCam(XMLDataFormat* data) {
    return data ? data->poscam[0] : 0.0f;
}

float getYPosCam(XMLDataFormat* data) {
    return data ? data->poscam[1] : 0.0f;
}

float getZPosCam(XMLDataFormat* data) {
    return data ? data->poscam[2] : 0.0f;
}

float getXLookAt(XMLDataFormat* data) {
    return data ? data->lookAt[0] : 0.0f;
}

float getYLookAt(XMLDataFormat* data) {
    return data ? data->lookAt[1] : 0.0f;
}

float getZLookAt(XMLDataFormat* data) {
    return data ? data->lookAt[2] : 0.0f;
}

float getXUp(XMLDataFormat* data) {
    return data ? data->up[0] : 0.0f;
}

float getYUp(XMLDataFormat* data) {
    return data ? data->up[1] : 0.0f;
}

float getZUp(XMLDataFormat* data) {
    return data ? data->up[2] : 0.0f;
}

void deleteXMLDataFormat(XMLDataFormat* data) {
    if (data) {
        // Since std::list automatically handles memory management, we don't need to manually free the items
        data->models.clear(); // Clears the list
        delete data;  // Delete the struct itself
    }
}
