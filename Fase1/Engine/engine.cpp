#include "engine.h"
#include <GL/glut.h>
#include <iostream>
#include <list>

using namespace std;

#define WHITE 1.0f, 1.0f, 1.0f

// Variáveis da câmara
float alpha = M_PI / 4, 
      beta_ = M_PI / 4, 
      radius = 5.0f;

float lookAtx = 0.0f, 
      lookAty = 0.0f, 
      lookAtz = 0.0f;

float upx = 0.0f, 
      upy = 1.0f, 
      upz = 0.0f;

bool showAxes = true; 
int mode = GL_LINE;
list<Primitive> primitives;

void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = (float) w / (float) h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawPrimitives(const list<Primitive>& figs) {
    glColor3f(WHITE);
    glBegin(GL_TRIANGLES);
    for (const auto& fig : figs) {
        for (const auto& point : getPoints(fig)) {
            float 
            px = getX(point), 
            py = getY(point), 
            pz = getZ(point); 
            glVertex3f(px,py,pz);
        }
    }
    glEnd();
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(radius * cos(beta_) * sin(alpha), radius * sin(beta_), radius * cos(beta_) * cos(alpha),
              lookAtx, lookAty, lookAtz, 
              upx, upy, upz);

    // Eixos 
    if (showAxes){
    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);

    glEnd();
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, mode);
    drawPrimitives(primitives);

    glutSwapBuffers();
}

void keyProc(unsigned char key, int, int) {
    if(key == 119 || key == 87){ // w 
        beta_ += (beta_ <= 1.48f ? 0.1f : 0.0f); 
    }
    else if(key == 97 || key == 65){ // a 
        alpha -= 0.1f;  // alfa controla esq dir 
    }
    else if(key == 115 || key == 83){ // s
        beta_ -= (beta_ >= -1.48f ? 0.1f : 0.0f); 
    }
    else if(key == 100 || key == 68){ // d 
        alpha += 0.1f;  // alfa controla esq dir 
    }
    else if (key == 102 || key == 70){ // f 
        mode = GL_FILL;  // preencher imagem 
    }
    else if(key == 108 || key == 76){ // l 
        mode = GL_LINE;  // apenas linhas (default)
    }
    else if(key == 98 || key  == 66){ // b -> blank -> so os pontos 
        mode = GL_POINT;  // apenas pontos da figura
    }
    else if (key == 120 || key == 88){ // x 
        mode = showAxes = !showAxes;  // ocultar os eixos x y z pelo controlo da variavel showAxes no renderScene
    }

    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    std::list<std::string> models = {"box.3d", "plane.3d"};

    Primitive p = fileToPrimitive(argv[1]); 
    primitives.push_back(p);

    beta_ = asin(5.0f / radius);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Fase 1");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyProc); 

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE); // ao ativado, é descartado algumas faces dos triangulos com base na orientacao do winding order (ordem dos vertices)

    glutMainLoop();

    return 0;
}