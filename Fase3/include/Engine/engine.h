#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glew.h>

#include <list>
#include <GL/glut.h>
#include <unordered_map>
#include <sstream> 


#include "../Utils/XMLDataFormat.h" 
#include "../Utils/primitive.h"
#include "../../tinyXML/tinyxml.h"
#include "../UI/ui.h"

struct ModelData {
    GLuint vao;
    GLuint ebo;
    size_t indexCount;
};


void computeSphericalCoordinates();
void drawAxes(); 
void changeSize(int w, int h) ; 
void drawPrimitives(const std::list<std::string> figs) ;
void renderScene() ; 
void processKeys(unsigned char key, int, int) ; 
void processSpecialKeys(int key, int , int ) ; 
void updateCameraPosition(void) ;
void alternate_color(float, float , float); 
void initializeCameraAndWindow(XMLDataFormat* xmlData);
void applyTransform(const Transform& transform);
void renderGroup(const Group& group);
void showCameraPosition(); 
void idleFunc();
ModelData loadModel(const std::string& modelName);
void renderCatmullRomCurve(const std::vector<std::array<float, 3>>& controlPoints); 
void drawPrimitiveVBO(GLuint vao, GLuint ebo, size_t indexCount);
GLuint createVBO(const std::string& modelName, const std::vector<float>& vertices, const std::vector<unsigned int>& indices); 
void getGlobalCatmullRomPoint(float gt, const std::vector<std::array<float, 3>>& points, float pos[3]);
void getGlobalCatmullRomPointWithDeriv(float gt, const std::vector<std::array<float, 3>>& points, float pos[3], float deriv[3]);
void getCatmullRomPoint(float t, float p0[3], float p1[3], float p2[3], float p3[3], float pos[3]);
void getCatmullRomPointWithDeriv(float t, float p0[3], float p1[3], float p2[3], float p3[3], float pos[3], float deriv[3]);
void buildRotMatrix(const float *x, const float *y, const float *z, float *m);
void updateFPS();
void cross(const float* a, const float* b, float* result);
void normalize(float* vec); 



#endif 
