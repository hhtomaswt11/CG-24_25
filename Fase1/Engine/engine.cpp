#include "engine.h"

using namespace std;

#define WHITE 1.0f, 1.0f, 1.0f
#define PI M_PI

float camX, camY, camZ;
float lookAtx, lookAty, lookAtz;
float upx, upy, upz;
float fov, nearPlane, farPlane;
float windowWidth, windowHeight;
bool showAxes = true;
int mode = GL_LINE;

list<Primitive> primitives;

// angulos da cam
float Alpha, Beta;
float radius;

// constantes para movimento da cam
const float ANGLE_INCREMENT = PI / 30;
const float ZOOM_INCREMENT = 0.5f;

// calcula as coordenadas esféricas da câmera
void computeSphericalCoordinates() {
    float dx = camX - lookAtx;
    float dy = camY - lookAty;
    float dz = camZ - lookAtz;
    radius = sqrt(dx * dx + dy * dy + dz * dz);
    Beta = asin(dy / radius);
    Alpha = atan2(dx, dz);
}

// atualiza a posição da câmera
void updateCameraPosition() {
    camX = lookAtx + radius * cos(Beta) * sin(Alpha);
    camY = lookAty + radius * sin(Beta);
    camZ = lookAtz + radius * cos(Beta) * cos(Alpha);
}

void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, ratio, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
}

void drawPrimitives() {
    glColor3f(WHITE);
    glBegin(GL_TRIANGLES);
    for (const auto& p : primitives) {
        for (const auto& point : getPoints(p)) {
            glVertex3f(getX(point), getY(point), getZ(point));
        }
    }
    glEnd();
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ, lookAtx, lookAty, lookAtz, upx, upy, upz);

    if (showAxes) {
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

    glPolygonMode(GL_FRONT, mode);
    drawPrimitives();
    glutSwapBuffers();
}

void keyProc(unsigned char key, int, int) {
    if (key == 'w' || key == 'W') {
        Beta += ANGLE_INCREMENT;
        if (Beta > PI / 2.0f) Beta = PI / 2.0f;
    }
    else if (key == 's' || key == 'S') {
        Beta -= ANGLE_INCREMENT;
        if (Beta < -PI / 2.0f) Beta = -PI / 2.0f;
    }
    else if (key == 'a' || key == 'A') {
        Alpha -= ANGLE_INCREMENT;
    }
    else if (key == 'd' || key == 'D') {
        Alpha += ANGLE_INCREMENT;
    }
    else if (key == 'q' || key == 'Q') {
        radius -= ZOOM_INCREMENT;
        if (radius < 1.0f) radius = 1.0f;
    }
    else if (key == 'e' || key == 'E') {
        radius += ZOOM_INCREMENT;
    }
    else if (key == 'f' || key == 'F') mode = GL_FILL;
    else if (key == 'l' || key == 'L') mode = GL_LINE;
    else if (key == 'b' || key == 'B') mode = GL_POINT;
    else if (key == 'x' || key == 'X') showAxes = !showAxes;
    
    updateCameraPosition();
    glutPostRedisplay();
}

void initializeCameraAndWindow(XMLDataFormat* xmlData) {
    if (xmlData) {
        // Camera - Posicao e Orientacao
        camX = getXPosCam(xmlData);
        camY = getYPosCam(xmlData);
        camZ = getZPosCam(xmlData);

        lookAtx = getXLookAt(xmlData);
        lookAty = getYLookAt(xmlData);
        lookAtz = getZLookAt(xmlData);

        upx = getXUp(xmlData);
        upy = getYUp(xmlData);
        upz = getZUp(xmlData);

        // Parametros de projecao
        fov = getFov(xmlData);
        nearPlane = getNear(xmlData);
        farPlane = getFar(xmlData);

        // Tamanho da Janela
        windowWidth = getWidth(xmlData);
        windowHeight = getHeight(xmlData);
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <XML file>" << endl;
        return -1;
    }

    XMLDataFormat* xmlData = xmlToXMLDataFormat(argv[1]);
    if (!xmlData) {
        cerr << "Falha no parse do xml." << endl;
        return -1;
    }

    // os valores de camera parametros de projecao e tamanho de janela sao definidos com base no xml 
    initializeCameraAndWindow(xmlData);

    computeSphericalCoordinates();

    for (const string& model : getModels(xmlData)) {
        primitives.push_back(fileToPrimitive(model.c_str()));
    }

    deleteXMLDataFormat(xmlData);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Fase1");
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyProc);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glutMainLoop();

    return 0;
}
