#include "../../include/Engine/engine.h"
#define PI M_PI

XMLDataFormat* xmlData = nullptr;
std::list<Primitive> primitives;

// Camera
float camX, camY, camZ;
float lookAtx, lookAty, lookAtz;
float upx, upy, upz;
float fov, nearPlane, farPlane;

// Window
float windowWidth, windowHeight;

// Control axes
bool showAxes = true;

// Diff color
bool yellow = false;

// Rendering mode
int mode = GL_LINE;

// White
float colorR = 1.0f, colorG = 1.0f, colorB = 1.0f;

// Camera angles
float Alpha, Beta, radius;

// Constants for camera movement
const float ANGLE_INCREMENT = PI / 75;
const float ZOOM_INCREMENT = 0.5f;

// Animation timing
int startTime = 0; // Store the start time for animation
bool animationStarted = false; // Flag to initialize startTime

void drawAxes(){
    float axisLength = 500.0f;

    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); // X - red
    glVertex3f(-axisLength, 0.0f, 0.0f);
    glVertex3f(axisLength, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f); // Y - green
    glVertex3f(0.0f, -axisLength, 0.0f);
    glVertex3f(0.0f, axisLength, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f); // Z - blue
    glVertex3f(0.0f, 0.0f, -axisLength);
    glVertex3f(0.0f, 0.0f, axisLength);
    glEnd();
}

// Alternate between yellow and white
void alternate_color(float new_colorR, float new_colorG, float new_colorB) {
    colorR = new_colorR;
    colorG = new_colorG;
    colorB = new_colorB;
}

// Calculate spherical coordinates for the camera
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
    std::cout << std::endl;
}

// Update camera position based on spherical coordinates
void updateCameraPosition() {
    camX = lookAtx + radius * cos(Beta) * sin(Alpha);
    camY = lookAty + radius * sin(Beta);
    camZ = lookAtz + radius * cos(Beta) * cos(Alpha);
}

void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = (float)w / (float)h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, ratio, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
}

void getCatmullRomPoint(float t, float p0[3], float p1[3], float p2[3], float p3[3], float pos[3]) {
    float m[4][4] = {
        {-0.5f, 1.5f, -1.5f, 0.5f},
        {1.0f, -2.5f, 2.0f, -0.5f},
        {-0.5f, 0.0f, 0.5f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f}
    };

    for (int i = 0; i < 3; ++i) {
        float a0 = m[0][0] * p0[i] + m[0][1] * p1[i] + m[0][2] * p2[i] + m[0][3] * p3[i];
        float a1 = m[1][0] * p0[i] + m[1][1] * p1[i] + m[1][2] * p2[i] + m[1][3] * p3[i];
        float a2 = m[2][0] * p0[i] + m[2][1] * p1[i] + m[2][2] * p2[i] + m[2][3] * p3[i];
        float a3 = m[3][0] * p0[i] + m[3][1] * p1[i] + m[3][2] * p2[i] + m[3][3] * p3[i];

        pos[i] = a3 + a2 * t + a1 * t * t + a0 * t * t * t;
    }
}

void getGlobalCatmullRomPoint(float gt, const std::vector<std::array<float, 3>>& points, float pos[3]) {
    int numPoints = points.size();
    float t = gt * numPoints;
    int index = (int)floor(t);
    t = t - index;

    int indices[4];
    indices[0] = (index + numPoints - 1) % numPoints;
    indices[1] = (index + 0) % numPoints;
    indices[2] = (index + 1) % numPoints;
    indices[3] = (index + 2) % numPoints;

    float p0[3] = { points[indices[0]][0], points[indices[0]][1], points[indices[0]][2] };
    float p1[3] = { points[indices[1]][0], points[indices[1]][1], points[indices[1]][2] };
    float p2[3] = { points[indices[2]][0], points[indices[2]][1], points[indices[2]][2] };
    float p3[3] = { points[indices[3]][0], points[indices[3]][1], points[indices[3]][2] };

    getCatmullRomPoint(t, p0, p1, p2, p3, pos);
}

GLuint createVBO(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    GLuint vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

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

void renderCatmullRomCurve(const std::vector<std::array<float, 3>>& controlPoints) {
    float pos[3];

    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 1.0, 1.0); // White curve

    for (float t = 0; t <= 1.0; t += 0.001) {
        getGlobalCatmullRomPoint(t, controlPoints, pos);
        glVertex3f(pos[0], pos[1], pos[2]);
    }

    glEnd();
}

void renderGroup(const Group& group) {
    glPushMatrix();

    // Handle rotation over time
    if (group.transform.rotationTime > 0.0f) {
        if (!animationStarted) {
            startTime = glutGet(GLUT_ELAPSED_TIME);
            animationStarted = true;
        }

        float elapsedTime = (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000.0f; // Time in seconds
        float rotationFraction = fmod(elapsedTime, group.transform.rotationTime) / group.transform.rotationTime;

        // Calculate the rotation angle based on time
        float rotationAngle = 360.0f * rotationFraction;

        // Apply rotation
        glRotatef(rotationAngle, group.transform.rotationAxis[0], group.transform.rotationAxis[1], group.transform.rotationAxis[2]);
    }

    // Render a curva primeiro, sem afetar a translação de animação
    if (group.transform.hasCurve) {
        renderCatmullRomCurve(group.transform.controlPoints);
    }

    // Handle curva de animação para o modelo (Curve Animation)
    if (group.transform.hasCurve) {
        if (!animationStarted) {
            startTime = glutGet(GLUT_ELAPSED_TIME);
            animationStarted = true;
        }

        float elapsedTime = (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000.0f;
        float time = group.transform.time;
        float t = (time > 0) ? fmod(elapsedTime, time) / time : 0.0f;

        float pos[3], next[3];
        getGlobalCatmullRomPoint(t, group.transform.controlPoints, pos);
        getGlobalCatmullRomPoint(t + 0.001, group.transform.controlPoints, next);

        float tangent[3] = { next[0] - pos[0], next[1] - pos[1], next[2] - pos[2] };
        float length = sqrt(tangent[0] * tangent[0] + tangent[1] * tangent[1] + tangent[2] * tangent[2]);

        if (length > 0.0001f) {
            tangent[0] /= length;
            tangent[1] /= length;
            tangent[2] /= length;

            float up[3] = { 0, 1, 0 }; // default up vector
            float side[3] = {
                up[1] * tangent[2] - up[2] * tangent[1],
                up[2] * tangent[0] - up[0] * tangent[2],
                up[0] * tangent[1] - up[1] * tangent[0]
            };

            float sideLen = sqrt(side[0] * side[0] + side[1] * side[1] + side[2] * side[2]);
            if (sideLen > 0.0001f) {
                side[0] /= sideLen;
                side[1] /= sideLen;
                side[2] /= sideLen;

                float newUp[3] = {
                    tangent[1] * side[2] - tangent[2] * side[1],
                    tangent[2] * side[0] - tangent[0] * side[2],
                    tangent[0] * side[1] - tangent[1] * side[0]
                };

                // Build rotation matrix
                float matrix[16] = {
                    side[0], side[1], side[2], 0.0f,
                    newUp[0], newUp[1], newUp[2], 0.0f,
                    tangent[0], tangent[1], tangent[2], 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
                };

                glTranslatef(pos[0], pos[1], pos[2]);
                glMultMatrixf(matrix);
            }
        }
    } else {
        // Apply static transform (rotate/translate/scale)
        applyTransform(group.transform);
    }

    // Check if alignToCurve is set, and apply alignment
    if (group.transform.alignToCurve) {
        glPushMatrix();
        glTranslatef(group.transform.translate[0], group.transform.translate[1], group.transform.translate[2]);

        // Alignment: Apply curve rotation to align the model orientation
        float alignmentTangent[3];
        getGlobalCatmullRomPoint(0.0f, group.transform.controlPoints, alignmentTangent); // Use t=0 for alignment point
        glRotatef(atan2(alignmentTangent[1], alignmentTangent[0]) * 180.0f / PI, 0.0f, 1.0f, 0.0f); // Rotate along the curve

        // Apply other transformations (rotate, scale, etc.)
        applyTransform(*getTransform(&group));

        glPopMatrix();
    }

    // Render child groups recursively
    for (const auto* child : getChildren(&group)) {
        glPushMatrix();
        renderGroup(*child);
        glPopMatrix();
    }

    // Render models
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (const auto& model : getModels(&group)) {
        Primitive prim = from3dFileToPrimitive(model.c_str());
        if (prim) {
            const auto& pontos = getPoints(prim);
            const auto& modelIndices = getIndices(prim);

            for (const auto& ponto : pontos) {
                vertices.push_back(getX(ponto));
                vertices.push_back(getY(ponto));
                vertices.push_back(getZ(ponto));
            }

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

  
    if (showAxes) {
        drawAxes();
    }

    glColor3f(1.0f, 1.0f, 1.0f);

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
            yellow = !yellow;
            break;
    }

    updateCameraPosition();
    glutPostRedisplay();
}

void processSpecialKeys(int key, int, int) {
    switch (key) {
        case GLUT_KEY_RIGHT:
            Alpha += ANGLE_INCREMENT;
            break;
        case GLUT_KEY_LEFT:
            Alpha -= ANGLE_INCREMENT;
            break;
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

void idleFunc() {
    glutPostRedisplay(); // Request redraw for animation
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
    glutIdleFunc(idleFunc); // Register idle function for animation
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutMainLoop();

    deleteXMLDataFormat(xmlData);

    return 0;
}