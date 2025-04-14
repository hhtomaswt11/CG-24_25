#include "../../include/Engine/engine.h"

#define PI M_PI

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

// diff color 
bool yellow = false;  

int mode = GL_LINE;

// white 
float colorR = 1.0f, colorG = 1.0f, colorB = 1.0f;

// angulos da cam
float Alpha, Beta, radius;

// constantes para movimento da cam
const float ANGLE_INCREMENT = PI / 75;
const float ZOOM_INCREMENT = 0.5f;

// alternar entre amarelo e branco 
void alternate_color(float new_colorR, float new_colorG, float new_colorB){
    colorR  = new_colorR; 
    colorG = new_colorG; 
    colorB = new_colorB;  
}

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

GLuint createVBO(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    GLuint vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // VBO para os vértices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // EBO para os índices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Definir o formato dos dados dos vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return vao;
}

void drawPrimitiveVBO(GLuint vao, GLuint ebo, size_t indexCount) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    
    for (const auto& model : getModels(&group)) {
        Primitive prim = from3dFileToPrimitive(model.c_str());
        if (prim) {
            const auto& pontos = getPoints(prim);
            const auto& modelIndices = getIndices(prim);

            // Carregar os pontos
            for (const auto& ponto : pontos) {
                vertices.push_back(getX(ponto));
                vertices.push_back(getY(ponto));
                vertices.push_back(getZ(ponto));
            }

            // Carregar os índices
            for (const auto& index : modelIndices) {
                indices.push_back(index);
            }
        }
    }

    GLuint vao = createVBO(vertices, indices);
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    drawPrimitiveVBO(vao, ebo, indices.size());

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

    glColor3f(1.0f, 1.0f, 1.0f);

    glPolygonMode(GL_FRONT, mode); 
    renderGroup(*getRootGroup(xmlData));  // Chama renderGroup para renderizar a cena

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
            yellow = !yellow; 
            break;
    }

    updateCameraPosition();
    glutPostRedisplay();
}

void processSpecialKeys(int key, int , int ) {
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
        return -1;
    }

    xmlData = xmlToXMLDataFormat(argv[1]);
    if (!xmlData) {
        std::cerr << "Error parsing XML file." << std::endl;
        return -1;
    }

    initializeCameraAndWindow(xmlData);
    computeSphericalCoordinates();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    int width = 1920, height = 1080;
    int posX = (width - windowWidth) / 2, posY = (height - windowHeight) / 2;
    glutInitWindowPosition(posX, posY);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("CG @UMINHO - Fase 3 - Grupo 36");
    glewInit();
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
