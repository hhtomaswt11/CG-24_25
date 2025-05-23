    #include "../../include/Engine/engine.h"
    #define PI M_PI

    XMLDataFormat* xmlData = nullptr;
    std::list<Primitive> primitives;
    
    std::map<std::string, ModelData> modelCache;
    std::map<std::string, GLuint> textureCache;

    // definicoes da camera FPS
    float camX = 0.0f, camY = 1.75f, camZ = 5.0f;
    float yaw = -90.0f;   // rotação em torno do eixo Y
    float pitch = 0.0f;   // rotação em torno do eixo X

    float dirX, dirY, dirZ;
    float speed = 0.2f;

    int lastMouseX = -1, lastMouseY = -1;
    bool firstMouse = true;

    // atualiza a direção da câmera com base em yaw/pitch
    void updateCameraDirection() {
        float yawRad = yaw * (M_PI / 180.0f);
        float pitchRad = pitch * (M_PI / 180.0f);

        dirX = cos(pitchRad) * cos(yawRad);
        dirY = sin(pitchRad);
        dirZ = cos(pitchRad) * sin(yawRad);

        // normalizacao 
        float len = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
        dirX /= len; dirY /= len; dirZ /= len;
    }

    void setupLights() {
        float amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
        
        const auto& lights = getLights(xmlData);
        int lightIdx = GL_LIGHT0;
        
        for (const auto& light : lights) {
            glEnable(lightIdx);
    
            GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
            GLfloat lightPos[4] = {
                light.position[0], 
                light.position[1], 
                light.position[2],
                light.position[3] // 0.0 = directional, 1.0 = point or spot
            };
    
            glLightfv(lightIdx, GL_POSITION, lightPos);
            glLightfv(lightIdx, GL_DIFFUSE, lightColor);
            glLightfv(lightIdx, GL_SPECULAR, lightColor);
    
            if (light.type == "directional") {
                glLightf(lightIdx, GL_SPOT_CUTOFF, 180.0f); 
            }
            else if (light.type == "spot") {
                GLfloat spotDir[3] = {
                    light.direction[0],
                    light.direction[1],
                    light.direction[2]
                };
                glLightfv(lightIdx, GL_SPOT_DIRECTION, spotDir);
                glLightf(lightIdx, GL_SPOT_CUTOFF, light.cutoff);
            }
            else {
                glLightf(lightIdx, GL_SPOT_CUTOFF, 180.0f); 
            }
    
            lightIdx++;
            if (lightIdx > GL_LIGHT7) break;
        }
    }
    


    void applyMaterial(const Color& color) {
        GLfloat matAmbient[] = {color.ambient[0], color.ambient[1], color.ambient[2], 1.0f};
        GLfloat matDiffuse[] = {color.diffuse[0], color.diffuse[1], color.diffuse[2], 1.0f};
        GLfloat matSpecular[] = {color.specular[0], color.specular[1], color.specular[2], 1.0f};
        GLfloat matEmissive[] = {color.emissive[0], color.emissive[1], color.emissive[2], 1.0f};

        
        glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
        glMaterialfv(GL_FRONT, GL_EMISSION, matEmissive);
        glMaterialf(GL_FRONT, GL_SHININESS, color.shininess);
    }


    GLuint loadTexture(const std::string& filename) {
        static bool ilInitialized = false;
        if (!ilInitialized) {
            ilInit();
            iluInit();
            ilEnable(IL_ORIGIN_SET);
            ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
            ilInitialized = true;
        }

        ILuint imageID;
        ilGenImages(1, &imageID);
        ilBindImage(imageID);

        // load da imagem
        std::string fullPath = "../test_files_phase_4/" + filename; 
        if (!ilLoadImage(fullPath.c_str())) {
            std::cerr << "Error loading texture: " << fullPath
                      << " - " << iluErrorString(ilGetError()) << std::endl;
            ilDeleteImages(1, &imageID);
            return 0;
        }

        // converter a imaagem para rgba 
        if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
            std::cerr << "Error converting texture: " << fullPath 
                      << " - " << iluErrorString(ilGetError()) << std::endl;
            ilDeleteImages(1, &imageID);
            return 0;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // quando minifica
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // quando amplia

        // upload the texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                    ilGetInteger(IL_IMAGE_WIDTH),
                    ilGetInteger(IL_IMAGE_HEIGHT),
                    0, GL_RGBA, GL_UNSIGNED_BYTE,
                    ilGetData());
        glGenerateMipmap(GL_TEXTURE_2D);

        ilDeleteImages(1, &imageID);

        return textureID;
    }

    void normalize(float* vec) {
        float length = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
        if (length > 0.0001f) { 
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
    
            char title[128];  
            sprintf(title, "CG @UMINHO - Fase 4 - Grupo 36 - FPS: %.2f", fps);
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
    float lookAtx, lookAty, lookAtz;
    float upx, upy, upz;
    float fov, nearPlane, farPlane;

    // Window
    float windowWidth, windowHeight;

    // Eixos x, y, z
    bool showAxes = true;

    // Rendering mode
    int mode = GL_FILL;

    // White
    float colorR = 1.0f, colorG = 1.0f, colorB = 1.0f;

    // angulos da camera
    float Alpha, Beta, radius;

    // constantes para o movimento da camera
    const float ANGLE_INCREMENT = PI / 75;
    const float ZOOM_INCREMENT = 0.5f;

    int startTime = 0; // tempo de inicio da animacao
    bool animationStarted = false; 




    void drawAxes(){
        float axisLength = 500.0f;

        // estado de luz atual 
        GLboolean lightingEnabled;
        glGetBooleanv(GL_LIGHTING, &lightingEnabled);
        
        glDisable(GL_LIGHTING);

        glBegin(GL_LINES);

        // x vermelho 
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-axisLength, 0.0f, 0.0f);
        glVertex3f(axisLength, 0.0f, 0.0f);

        // y verde 
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -axisLength, 0.0f);
        glVertex3f(0.0f, axisLength, 0.0f);

        // z azul 
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -axisLength);
        glVertex3f(0.0f, 0.0f, axisLength);
        glEnd();

        // restore do estado da luz 
        if (lightingEnabled) {
            glEnable(GL_LIGHTING);
        }
    }

    // Funcionalidade para ver a posição da camera no terminal 
    void showCameraPosition() {
        std::cout << "Camera Position: (" << camX << "," << camY << "," << camZ << ")" << std::endl;
        std::cout << "<position x=\"" << camX << "\" y=\"" << camY << "\" z=\"" << camZ << "\" />" << std::endl;
        std::cout << std::endl;
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


    void drawPrimitiveVBO(GLuint vao, GLuint ebo, size_t indexCount) {
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void applyTransform(const Transform& transform) {
        // transformações estáticas em ordem
        for (const TransformStep& step : transform.orderedSteps) {
            switch (step.type) {
                case TransformType::TRANSLATE:
                    glTranslatef(step.translate[0], step.translate[1], step.translate[2]);
                    break;
                case TransformType::ROTATE:
                    glRotatef(step.rotate[0], step.rotate[1], step.rotate[2], step.rotate[3]);
                    break;
                case TransformType::SCALE:
                    glScalef(step.scale[0], step.scale[1], step.scale[2]);
                    break;
            }
        }
    }

    void renderCatmullRomCurve(const std::vector<std::array<float, 3>>& controlPoints) {
        float pos[3];

        glBegin(GL_LINE_STRIP);
        glColor3f(1.0, 1.0, 1.0); 

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
        const auto& texCoords = getTexCoords(prim);
        const auto& normals = getNormals(prim);
    
        std::vector<float> vertices;
        std::vector<float> textureCoords;
        std::vector<float> normalVectors;
    
        for (size_t i = 0; i < pontos.size(); ++i) {
            const auto& p = pontos[i];
            const auto& texCoord = texCoords[i];
            const auto& normal = normals[i];
    
            // add vertex coordinates (X, Y, Z)
            vertices.push_back(getX(p));
            vertices.push_back(getY(p));
            vertices.push_back(getZ(p));
    
            // add texture coordinates (U, V)
            textureCoords.push_back(texCoord.u);
            textureCoords.push_back(texCoord.v);
    
            // add normals (NX, NY, NZ)
            normalVectors.push_back(getX(normal));
            normalVectors.push_back(getY(normal));
            normalVectors.push_back(getZ(normal));
        }
    
        GLuint vao, ebo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &ebo);
    
        glBindVertexArray(vao);
    
        // VBO para os vertices 
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, 0);
    
        // VBO para texture coordinates
        GLuint texCoordVbo;
        glGenBuffers(1, &texCoordVbo);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordVbo);
        glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(float), textureCoords.data(), GL_STATIC_DRAW);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
    
        // VBO para normais
        GLuint normalVbo;
        glGenBuffers(1, &normalVbo);
        glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
        glBufferData(GL_ARRAY_BUFFER, normalVectors.size() * sizeof(float), normalVectors.data(), GL_STATIC_DRAW);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, 0);
    
        // EBO para indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    
        glBindVertexArray(0);
    
        return ModelData{vao, ebo, indices.size()};
    }
    
    

    void renderGroup(const Group& group) {
        glPushMatrix();

        if (group.transform.hasCurve) {
            renderCatmullRomCurve(group.transform.controlPoints);
        }

        // animacao ao longo da curva 
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

                cross(deriv, yAxis().data(), z);
                normalize(z);
                cross(z, deriv, y);
                normalize(y);

                float rot[16];
                buildRotMatrix(deriv, y, z, rot);
                glMultMatrixf(rot);
            }
        }

        // rotacao time based 
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

            for (const auto& model : getGroupModels(&group)) {
                if (modelCache.find(model.file) == modelCache.end()) {
                    ModelData modelData = loadModel(model.file);
                    if (modelData.vao != 0) {
                        modelCache[model.file] = modelData;
                    }
                }

                applyMaterial(model.color);

                if (!model.texture.empty()) {
                    if (textureCache.find(model.texture) == textureCache.end()) {
                        textureCache[model.texture] = loadTexture(model.texture);
                    }
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, textureCache[model.texture]);
                } else {
                    glDisable(GL_TEXTURE_2D);
                }

                if (modelCache.count(model.file)) {
                    drawPrimitiveVBO(modelCache[model.file].vao, 
                                   modelCache[model.file].ebo,
                                   modelCache[model.file].indexCount);
                }
            }

        for (const auto* child : getChildren(&group)) {
            renderGroup(*child);
        }

            glPopMatrix();
    }
     

    void renderScene() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(
            camX, camY, camZ,
            camX + dirX, camY + dirY, camZ + dirZ,
            0.0f, 1.0f, 0.0f
        );
        
        setupLights();

        if (showAxes) {
            glDisable(GL_LIGHTING);
            drawAxes();
            glEnable(GL_LIGHTING);
        }

        glPolygonMode(GL_FRONT, mode);
        renderGroup(*getRootGroup(xmlData));

        updateFPS();
        glutSwapBuffers();
    }

    void normalize2(float& x, float& y, float& z) {
        float len = sqrt(x * x + y * y + z * z);
        if (len != 0.0f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }
    
    void crossProduct(float ax, float ay, float az,
                      float bx, float by, float bz,
                      float& rx, float& ry, float& rz) {
                rx = ay * bz - az * by;
                ry = az * bx - ax * bz;
                rz = ax * by - ay * bx;
    }
    

    void processKeys(unsigned char key, int, int) {
        float rightX, rightY, rightZ;
        crossProduct(dirX, dirY, dirZ, 0.0f, 1.0f, 0.0f, rightX, rightY, rightZ);
        normalize2(rightX, rightY, rightZ);
    
        switch (tolower(key)) {
                case 'w': 
                camX += dirX * speed;
                camY += dirY * speed;
                camZ += dirZ * speed;
                break;
            case 's':  
                camX -= dirX * speed;
                camY -= dirY * speed;
                camZ -= dirZ * speed;
                break;
            case 'a': 
                camX -= rightX * speed;
                camZ -= rightZ * speed;
                break;
            case 'd':
                camX += rightX * speed;
                camZ += rightZ * speed;
                break;
            case 'c':
                showCameraPosition(); 
                break;
            case 'x':
                showAxes = !showAxes;
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
        }
        glutPostRedisplay();
    }
    

    void processSpecialKeys(int key, int, int) {
        // calcula o vetor lateral com base na direção atual
        float rightX, rightY, rightZ;
        crossProduct(dirX, dirY, dirZ, 0.0f, 1.0f, 0.0f, rightX, rightY, rightZ);
        normalize2(rightX, rightY, rightZ);
    
        switch (key) {
            case GLUT_KEY_UP:
                camX += dirX * speed;
                camY += dirY * speed;
                camZ += dirZ * speed;
                break;
            case GLUT_KEY_DOWN:
                camX -= dirX * speed;
                camY -= dirY * speed;
                camZ -= dirZ * speed;
                break;
            case GLUT_KEY_LEFT:
                camX -= rightX * speed;
                camY -= rightY * speed;
                camZ -= rightZ * speed;
                break;
            case GLUT_KEY_RIGHT:
                camX += rightX * speed;
                camY += rightY * speed;
                camZ += rightZ * speed;
                break;
        }
    
        glutPostRedisplay();
    }
    

    bool mousePressed = false;
    void mouseMotion(int x, int y) {
        if (!mousePressed) return; // só move se o botão estiver pressionado
    
        if (firstMouse) {
            lastMouseX = x;
            lastMouseY = y;
            firstMouse = false;
            return;
        }
    
        int deltaX = x - lastMouseX;
        int deltaY = lastMouseY - y; // cima é positivo
    
        lastMouseX = x;
        lastMouseY = y;
    
        float sensitivity = 0.1f;
        yaw += deltaX * sensitivity;
        pitch += deltaY * sensitivity;
    
        // limites para o pitch (para evitar virar de cabeça pra baixo)
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    
        updateCameraDirection();
        glutPostRedisplay();
    }

    void mouseButton(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON) {
            if (state == GLUT_DOWN) {
                mousePressed = true;
                lastMouseX = x;
                lastMouseY = y;
                firstMouse = true;
            } else if (state == GLUT_UP) {
                mousePressed = false;
            }
        }
    }
    
    void idleFunc() {
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
            // calcula a direção (vetor entre lookAt e posição da câmera)
            float dirVecX = lookAtx - camX;
            float dirVecY = lookAty - camY;
            float dirVecZ = lookAtz - camZ;

            // normaliza
            float length = sqrt(dirVecX * dirVecX + dirVecY * dirVecY + dirVecZ * dirVecZ);
            dirVecX /= length;
            dirVecY /= length;
            dirVecZ /= length;

            // calcula pitch e yaw a partir do vetor direção
            pitch = asin(dirVecY) * 180.0f / M_PI;
            yaw   = atan2(dirVecZ, dirVecX) * 180.0f / M_PI;

            // atualiza a direção usada na câmera FPS
            updateCameraDirection();

        }
    }

    int main(int argc, char* argv[]) {
        if (argc < 2) {
            helper_engine();
            return -1;
        }

        xmlData = xmlToXMLDataFormat(argv[1]);
        if (!xmlData) {
            std::cerr << "Erro no parsing do ficheiro XML." << std::endl;
            return -1;
        }

        initializeCameraAndWindow(xmlData);

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
        int width = 1920, height = 1080;
        int posX = (width - windowWidth) / 2, posY = (height - windowHeight) / 2;
        glutInitWindowPosition(posX, posY);
        glutInitWindowSize(windowWidth, windowHeight);
        glutCreateWindow("CG @UMINHO - Fase 4 - Grupo 36");

        glewInit();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHTING);
        glEnable(GL_RESCALE_NORMAL);

        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glutDisplayFunc(renderScene);
        glutReshapeFunc(changeSize);
        glutKeyboardFunc(processKeys);
        glutSpecialFunc(processSpecialKeys);
        glutMouseFunc(mouseButton);
        glutMotionFunc(mouseMotion);
        glutIdleFunc(idleFunc); 


        glutMainLoop();

        deleteXMLDataFormat(xmlData);

        return 0;
    }