#include "../../include/Engine/engine.h"

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
float colorR = 1.0f, colorG = 1.0f, colorB = 1.0f;


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
    glPushMatrix();
    glColor3f(colorR, colorG, colorB);

    glPolygonMode(GL_FRONT_AND_BACK, mode); 

    for (const auto& p : primitives) {
        const auto& pontos = getPoints(p);
        const auto& indices = getIndices(p);

        
        std::vector<float> vertices;
        for (const auto& ponto : pontos) {
            vertices.push_back(getX(ponto));
            vertices.push_back(getY(ponto));
            vertices.push_back(getZ(ponto));
        }

        
        glEnableClientState(GL_VERTEX_ARRAY); // para ativar e especificar o formato dos vértices
        glVertexPointer(3, GL_FLOAT, 0, vertices.data());

        // desenhar com os índices 
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

        // Desativar arrays de vértices
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    glPopMatrix();
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
    else if (key == 'a' || key == 'A') {
        Alpha -= ANGLE_INCREMENT;
    }
    else if (key == 's' || key == 'S') {
        Beta -= ANGLE_INCREMENT;
        if (Beta < -PI / 2.0f) Beta = -PI / 2.0f;
    }
    else if (key == 'd' || key == 'D') {
        Alpha += ANGLE_INCREMENT;
    }
    else if (key == '+'){
        radius -= ZOOM_INCREMENT;
        if (radius < 1.0f) radius = 1.0f;
    }
    else if (key == '-'){
        radius += ZOOM_INCREMENT;
    }
    else if (key == 'l' || key == 'L') mode = GL_LINE; // line 
    else if (key == 'b' || key == 'B') mode = GL_POINT; // blank 
    else if (key == 'f' || key == 'F') mode = GL_FILL; // filled 
    else if (key == 'x' || key == 'X') showAxes = !showAxes;
    else if (key == 'y' || key == 'Y') {
        colorR = 1.0f;
        colorG = 1.0f;
        colorB = 0.0f;
    }
    
    updateCameraPosition();
    glutPostRedisplay();
}

void initializeCameraAndWindow(XMLDataFormat* xmlData) {
    if (xmlData) {
        camX = getXPosCam(xmlData);
        camY = getYPosCam(xmlData);
        camZ = getZPosCam(xmlData);

        lookAtx = getXLookAt(xmlData);
        lookAty = getYLookAt(xmlData);
        lookAtz = getZLookAt(xmlData);

        upx = getXUp(xmlData);
        upy = getYUp(xmlData);
        upz = getZUp(xmlData);

        fov = getFov(xmlData);
        nearPlane = getNear(xmlData);
        farPlane = getFar(xmlData);

        windowWidth = getWidth(xmlData);
        windowHeight = getHeight(xmlData);
    }

}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        helper_engine();
       // cerr << "Uso: " << argv[0] << " <XML file>" << endl;
        return -1;
    }

    XMLDataFormat* xmlData = xmlToXMLDataFormat(argv[1]);
    if (!xmlData) {
        cerr << "Error parsing XML file." << endl;
        return -1;
    }

    // os valores de camera parametros de projecao e tamanho de janela sao definidos com base no xml 
    initializeCameraAndWindow(xmlData);

    computeSphericalCoordinates();

    for (const string& model : getModels(xmlData)) {
        Primitive prim = from3dFileToPrimitive(model.c_str());
        std::ifstream file(model.c_str());

        if (prim && file) {
            std::cout << "Loaded primitive. Results in: " << model << std::endl;
            primitives.push_back(prim);
        } 
        // else {
        //     std::cerr << "Error. Could not load file: " << model << std::endl;
        // }
    }

    deleteXMLDataFormat(xmlData);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("CG @UMINHO - Fase 1 - Grupo 36");
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(keyProc);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glutMainLoop();

    return 0;
}
