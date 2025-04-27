    #include "../../include/Engine/engine.h"
    #define PI M_PI

    XMLDataFormat* xmlData = nullptr;
    std::list<Primitive> primitives;

    std::map<std::string, GLuint> vboCache;
    std::map<std::string, ModelData> modelCache;

    void normalize(float* vec) {
        float length = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
        if (length > 0.0001f) { // Evita divisão por zero
            for (int i = 0; i < 3; ++i) {
                vec[i] /= length;
            }
        }
    }
    
    void cross(const float* a, const float* b, float* result) {
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
    }

    std::array<float, 3> yAxis() {
        return { 0.0f, 1.0f, 0.0f };
    }
    
    
    int frameCount = 0;
    float lastTime = 0.0f;
    float fps = 0.0f;

    void updateFPS() {
        frameCount++;
        float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;  // em segundos
        float delta = currentTime - lastTime;

        if (delta >= 1.0f) {
            fps = frameCount / delta;
            frameCount = 0;
            lastTime = currentTime;

            char title[64];
            sprintf(title, "FPS: %.2f", fps);
            glutSetWindowTitle(title);
        }
    }

    void buildRotMatrix(const float *x, const float *y, const float *z, float *m) {
        m[0]  = x[0]; m[1]  = x[1]; m[2]  = x[2]; m[3]  = 0;
        m[4]  = y[0]; m[5]  = y[1]; m[6]  = y[2]; m[7]  = 0;
        m[8]  = z[0]; m[9]  = z[1]; m[10] = z[2]; m[11] = 0;
        m[12] =    0; m[13] =    0; m[14] =    0; m[15] = 1;
    }

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

    void getCatmullRomPointWithDeriv(float t, float p0[3], float p1[3], float p2[3], float p3[3], float pos[3], float deriv[3]) {
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
            deriv[i] = a2 + 2 * a1 * t + 3 * a0 * t * t;
        }
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

    void getGlobalCatmullRomPointWithDeriv(float gt, const std::vector<std::array<float, 3>>& points, float pos[3], float deriv[3]) {
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
    
        getCatmullRomPointWithDeriv(t, p0, p1, p2, p3, pos, deriv);
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

    GLuint createVBO(const std::string& modelName, const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
        // Check if VBO for this model is already cached
        if (vboCache.find(modelName) != vboCache.end()) {
            return vboCache[modelName];  // Return the cached VBO
        }

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

        // Cache the created VBO using the model name
        vboCache[modelName] = vao;

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

    ModelData loadModel(const std::string& modelName) {
        Primitive prim = from3dFileToPrimitive(modelName.c_str());
        if (!prim) return {};

        const auto& pontos = getPoints(prim);
        const auto& indices = getIndices(prim);

        std::vector<float> vertices;
        for (const auto& p : pontos) {
            vertices.push_back(getX(p));
            vertices.push_back(getY(p));
            vertices.push_back(getZ(p));
        }

        GLuint vao, vbo, ebo;
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

        return { vao, ebo, indices.size() };
    }

    void renderGroup(const Group& group) {
        glPushMatrix();

    
        // Render curve for visualization
        if (group.transform.hasCurve) {
            renderCatmullRomCurve(group.transform.controlPoints);
        }
    
        // Handle animation along curve
        if (group.transform.hasCurve) {
            if (!animationStarted) {
                startTime = glutGet(GLUT_ELAPSED_TIME);
                animationStarted = true;
            }
    
            float elapsedTime = (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000.0f;
            float t = group.transform.time > 0 ? fmod(elapsedTime, group.transform.time) / group.transform.time : 0.0f;
    
            float pos[3], deriv[3];
    
            std::vector<std::array<float, 3>> controlPoints;
            for (const auto& point : group.transform.controlPoints) {
                controlPoints.push_back(std::array<float, 3>{point[0], point[1], point[2]});
            }
    
            getGlobalCatmullRomPointWithDeriv(t, controlPoints, pos, deriv);
    
            glTranslatef(pos[0], pos[1], pos[2]);
    
            if (group.transform.alignToCurve) {
                normalize(deriv);
    
                float y[3], z[3];
    
                cross(deriv, yAxis().data(), z); // Xi = deriv
    
                normalize(z);
    
                cross(z, deriv, y); // Xi = deriv

    
                normalize(y);
    
                float rot[16];
                buildRotMatrix(deriv, y, z, rot);
    
                // Aplica a transformação de rotação com glMultMatrixf
                glMultMatrixf(rot);
            }
        }

        // Handle rotation over time
        if (group.transform.rotationTime > 0.0f) {
            if (!animationStarted) {
                startTime = glutGet(GLUT_ELAPSED_TIME);
                animationStarted = true;
            }
            
            float elapsedTime = (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000.0f;
            float rotationFraction = fmod(elapsedTime, group.transform.rotationTime) / group.transform.rotationTime;
            float rotationAngle = 360.0f * rotationFraction;
            
            glRotatef(rotationAngle, group.transform.rotationAxis[0], group.transform.rotationAxis[1], group.transform.rotationAxis[2]);
        }
    
        applyTransform(group.transform);
    
        // Render child groups
        for (const auto* child : getChildren(&group)) {
            glPushMatrix();
            renderGroup(*child);
            glPopMatrix();
        }
    
        // Render models
        for (const auto& modelName : getModels(&group)) {
            // Carrega modelo apenas uma vez
            if (modelCache.find(modelName) == modelCache.end()) {
                ModelData modelData = loadModel(modelName);
                if (modelData.vao != 0) {
                    modelCache[modelName] = modelData;
                } else {
                    std::cerr << "Erro ao carregar modelo: " << modelName << std::endl;
                    continue;
                }
            }
    
            const ModelData& modelData = modelCache[modelName];
            drawPrimitiveVBO(modelData.vao, modelData.ebo, modelData.indexCount);
        }
    
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
        updateFPS();
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