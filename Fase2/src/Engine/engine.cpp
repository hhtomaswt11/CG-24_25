#include "../../include/Engine/engine.h"

#define PI M_PI

#define WHITE 1.0f, 1.0f, 1.0f

XMLDataFormat* xmlData = nullptr;
std::list<Primitive> primitives;

// camera 
float camX, camY, camZ;
float lookAtx, lookAty, lookAtz;
float upx, upy, upz;
float fov, nearPlane, farPlane;

// window 
float windowWidth, windowHeight;

// control axes 
bool showAxes = true;
int mode = GL_LINE;

// white 
float colorR = 1.0f, colorG = 1.0f, colorB = 1.0f;

// angulos da cam
float Alpha, Beta, radius;

// constantes para movimento da cam
const float ANGLE_INCREMENT = PI / 75;
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

void showCameraPosition() {
    std::cout << "Camera Position: (" << camX << "," << camY << "," << camZ << ")" << std::endl;
    std::cout << "<position x=\"" << camX << "\" y=\"" << camY << "\" z=\"" << camZ << "\" />" << std::endl;
    std::cout << std::endl; // Linha extra para separar chamadas consecutivas
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

    // informa o opengl que toda a janela (com novo wxh) deve ser usada para desenhar 
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, ratio, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
}


void drawPrimitives(const std::list<std::string> figs) {
    for (const auto& model : figs) {
        Primitive prim = from3dFileToPrimitive(model.c_str());
        if (prim) {
            glPushMatrix();
            glColor3f(colorR, colorG, colorB);
            glPolygonMode(GL_FRONT_AND_BACK, mode);

            const auto& pontos = getPoints(prim);
            const auto& indices = getIndices(prim);

            std::vector<float> vertices;
            for (const auto& ponto : pontos) {
                vertices.push_back(getX(ponto));
                vertices.push_back(getY(ponto));
                vertices.push_back(getZ(ponto));
            }

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, vertices.data());

            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

            glDisableClientState(GL_VERTEX_ARRAY);
            glPopMatrix();
        } else {
            std::cerr << "Error: Could not load model: " << model << std::endl;
        }
    }
}

void applyTransform(const Transform& transform) {

    const float* rotate = getRotate(&transform);
    const float* translate = getTranslate(&transform);
    const float* scale = getScale(&transform);

    glRotatef(rotate[0], rotate[1], rotate[2], rotate[3]);
    glTranslatef(translate[0], translate[1], translate[2]);
    glScalef(scale[0], scale[1], scale[2]); 

}



void renderGroup(const Group& group) {
    glPushMatrix();
    applyTransform(*getTransform(&group));

    for (const auto* child : getChildren(&group)) {  
        glPushMatrix();
        renderGroup(*child); 
        glPopMatrix();
    }

    drawPrimitives(getModels(&group));

    glPopMatrix();
}



void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camX, camY, camZ, 
              lookAtx, lookAty, lookAtz, 
              upx, upy, upz);

    float axisLength = 500.0f;  // length dos eixos x y z 

    if (showAxes) {
        glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f); // X - vermelho
        glVertex3f(-axisLength, 0.0f, 0.0f);
        glVertex3f(axisLength, 0.0f, 0.0f);
        
        glColor3f(0.0f, 1.0f, 0.0f); // Y - verde
        glVertex3f(0.0f, -axisLength, 0.0f);
        glVertex3f(0.0f, axisLength, 0.0f);
        
        glColor3f(0.0f, 0.0f, 1.0f); // Z - azul
        glVertex3f(0.0f, 0.0f, -axisLength);
        glVertex3f(0.0f, 0.0f, axisLength);
        glEnd();
    }


    glPolygonMode(GL_FRONT, mode); 
    renderGroup(*getRootGroup(xmlData));

    glutSwapBuffers();
}



void processKeys(unsigned char key, int, int) {
    switch (tolower(key)) {
        case 'w':
            Beta += ANGLE_INCREMENT;
            if (Beta > PI / 2.0f) Beta = PI / 2.0f;
            break;

        case 'a':
            Alpha -= ANGLE_INCREMENT;
            break;

        case 's':
            Beta -= ANGLE_INCREMENT;
            if (Beta < -PI / 2.0f) Beta = -PI / 2.0f;
            break;

        case 'd':
            Alpha += ANGLE_INCREMENT;
            break;

        case '+':
            radius -= ZOOM_INCREMENT;
            if (radius < 1.0f) radius = 1.0f;
            break;

        case '-':
            radius += ZOOM_INCREMENT;
            break;

        case 'l':
            mode = GL_LINE;
            break;

        case 'b':
            mode = GL_POINT;
            break;

        case 'f':
            mode = GL_FILL;
            break;

        case 'x':
            showAxes = !showAxes;
            break;

        case 'c':
            showCameraPosition();
            break;

        case 'y':
            colorR = 1.0f;
            colorG = 1.0f;
            colorB = 0.0f;
            break;
    }

    updateCameraPosition();
    glutPostRedisplay();
}




void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {

	case GLUT_KEY_RIGHT:
		Alpha += ANGLE_INCREMENT; break;

	case GLUT_KEY_LEFT:
		Alpha -= ANGLE_INCREMENT; break;

	case GLUT_KEY_UP:
        Beta += ANGLE_INCREMENT;
        if (Beta > PI / 2.0f) Beta = PI / 2.0f;
        break;

	case GLUT_KEY_DOWN:
        Beta -= ANGLE_INCREMENT;
        if (Beta < -PI / 2.0f) Beta = -PI / 2.0f;
        break;
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

    xmlData = xmlToXMLDataFormat(argv[1]);
    if (!xmlData) {
        std::cerr << "Error parsing XML file." << std::endl;
        return -1;
    }

    // os valores de camera parametros de projecao e tamanho de janela sao definidos com base no xml 
    initializeCameraAndWindow(xmlData);

    computeSphericalCoordinates();

    for (const std::string& model : getModels(xmlData)) {
        Primitive prim = from3dFileToPrimitive(model.c_str());
        std::ifstream file(model.c_str());

        if (prim && file) {
            std::cout << "Loaded primitive. Results in: " << model << std::endl;
            primitives.push_back(prim);
        } 
 
    }


    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    // monitor 1920x1080
    int width = 1920,
        height = 1080,
        posX = (width - windowWidth) / 2,
        posY = (height - windowHeight) / 2;
        
    glutInitWindowPosition(posX, posY);

    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("CG @UMINHO - Fase 2 - Grupo 36");
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glutMainLoop();

    deleteXMLDataFormat(xmlData);

    return 0;
}


