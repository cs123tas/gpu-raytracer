#include "view.h"

#include "viewformat.h"
#include <QApplication>
#include <QKeyEvent>
#include <iostream>

#include "lib/ResourceLoader.h"
#include "lib/Sphere.h"
#include "gl/textures/Texture2D.h"

using namespace CS123::GL;

// Physics TODOs
// Loop over all objs
//  1- Compute Dynamics
//        1- For each shape compute the force (e.g. gravity) acting on it
//        2- Compute its updated velocity from the acceleration produced by force
//        3- Update position based on the computed velocity
//        4- Compute rotations using the computed angular velocities, moment of interia, torque
//  2- Compute Collision
// 3- Move OBJs with keys

// Represent objects with: center of mass, mass, , velocity vector, position vector,
// Represent the world with global things like gravity, wall boundaries, friction

View::View(QWidget *parent) : QGLWidget(ViewFormat(), parent),
    m_fbo(nullptr),
    m_time(),
    m_timer(),
    m_captureMouse(false),
    m_height(std::min(1000, height())),
    m_width(std::min(1000, width())),
    m_angleX(-0.5f),
    m_angleY(0.5f),
    m_zoom(4.f),
    m_leftSpeed(0.1f),
    m_centerSpeed(0.02f),
    m_rightSpeed(0.01),
    m_sleepTime(100),
    m_depth(1),
    m_increment(0),
    m_fps(60.0f),
    m_friction(0.05),
    m_physics(m_fps)
{
    // Rigid Physics
    m_g = glm::vec3({0.0f, 0.0f, -9.81f});
    m_dt = 1.0f/m_fps;

    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor
    if (m_captureMouse) {
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The update loop is implemented using a timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    m_timer.start(1000.0f / m_fps);
    m_tick = 1.0;


    // Rigid Physics
    m_spheres.reserve(3);
    m_spheres.resize(3);
    View::setupSpheres();
    m_walls.reserve(6);
    m_walls.resize(6);
    View::setupWalls();
}

View::~View()
{
}

void View::initializeGL() {
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    //initialize glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Something is very wrong, glew initialization failed." << std::endl;
    }
    std::cout << "Using GLEW " <<  glewGetString( GLEW_VERSION ) << std::endl;

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
//    m_time.start();
//    m_timer.start(1000 / 60);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Loading in our ray tracer!
    if (false){ // TODO: comp shader
        std::string rayTracerSource = ResourceLoader::loadResourceFileToString(":/shaders/rayTracer.comp");
        m_rayTracerCompProgram = std::make_unique<Shader>(rayTracerSource);
    }

    // Full screen quad
    std::string quadVertexSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.vert");
    std::string quadFragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.frag");
    m_textureProgram = std::make_unique<Shader>(quadVertexSource, quadFragmentSource);

    // Loading in our ray tracer!
    std::string rayTracerFragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/rayTracer.frag");
    m_rayTracerFragProgram = std::make_unique<Shader>(quadVertexSource, rayTracerFragmentSource);

    std::vector<GLfloat> quadData{
        -1.f, 1.f, 0.0,
         0.f, 0.f,
        -1.f, -1.f, 0.0,
         0.f, 1.f,
         1.f, 1.f, 0.0,
         1.f, 0.f,
         1.f, -1.f, 0.0,
         1.f, 1.f
    };

    m_quad = std::make_unique<OpenGLShape>();
    m_quad->setVertexData(&quadData[0], quadData.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, 4);
    m_quad->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->setAttribute(ShaderAttrib::TEXCOORD0, 2, 3*sizeof(GLfloat), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->buildVAO();

    // Print the max FBO dimension.
    bool isBenchMarkingFBO = false;
    if (isBenchMarkingFBO) {
        GLint maxRenderBufferSize;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &maxRenderBufferSize);
        std::cout << "Max FBO size: " << maxRenderBufferSize << std::endl;
    }

    // TODO: I don't know why the FBO class isn't working, here's raw ogl instead
    glGenTextures(1, &m_renderOut);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, std::min(1000, m_width), std::min(m_height, 1000), 0, GL_RGBA, GL_FLOAT, 0);

//    m_fbo = std::make_unique<FBO>(1,
//                                  FBO::DEPTH_STENCIL_ATTACHMENT::NONE,
//                                  m_width,
//                                  m_height,
//                                  TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
//                                  TextureParameters::FILTER_METHOD::LINEAR,
//                                  GL_FLOAT
//                                  );
}

void View::paintGL() {
    if (true) { // TODO: comp shaders
        paintWithFragmentShaders();
    } else {
        paintWithComputeShaders();
    }
}

// Figure out fbo problem, important for performance
void View::paintWithFragmentShaders() {
    //m_fbo->bind();


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float time = m_increment++ / (float) m_fps;

    m_rayTracerFragProgram->bind();
    m_rayTracerFragProgram->setUniform("time", static_cast<float>(time));
    m_rayTracerFragProgram->setUniform("dimensions", glm::vec2(m_width, m_height));
    m_rayTracerFragProgram->setUniform("depth", m_depth);
    m_rayTracerFragProgram->setUniform("rightSpeed", m_rightSpeed);
    m_rayTracerFragProgram->setUniform("leftSpeed", m_leftSpeed);
    m_rayTracerFragProgram->setUniform("centerSpeed", m_centerSpeed);

    // Rigid physics
    // TODOs: pass fps, gravity, acceleration, velocity
    m_physics.m_g = m_g;
    m_physics.m_fps = m_fps;
    m_physics.runPhysics(m_spheres);
    printf("new z component of left sphere's position: %.4f\n", m_spheres[0].position[2]);
    m_rayTracerFragProgram->setUniform("pos1", m_spheres[0].position);
    m_rayTracerFragProgram->setUniform("pos2", m_spheres[1].position);
    m_rayTracerFragProgram->setUniform("pos3", m_spheres[2].position);


    glActiveTexture(GL_TEXTURE0); // TODO: restore after figuring out the fbo issues
    glBindTexture(GL_TEXTURE_2D, m_renderOut);
    //m_fbo->getColorAttachment(0).bind();
    m_quad->draw();
    m_rayTracerFragProgram->unbind();
    // m_fbo->unbind();
    glBindImageTexture(0, m_renderOut, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

// TODO: it would be nice to optimize this based on a user's particular hardware
static void checkAvailableWorkers() {
    /*
     * This checks how many workers are
     * available for the task, changes between machines
     * Dan: max global (total) work group counts x:65535 y:65535 z:65535
     * max local (in one shader) work group sizes x:1024 y:1024 z:64
     */
    std::vector<int> numWorkGroups = std::vector<int>(3);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &numWorkGroups[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &numWorkGroups[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &numWorkGroups[2]);

    std::cout << "I have " << numWorkGroups[0] << " by " << numWorkGroups[1] << " by " << numWorkGroups[2] << " workers" << std::endl;

    std::vector<int> sizeWorkGroups = std::vector<int>(3);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &sizeWorkGroups[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &sizeWorkGroups[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &sizeWorkGroups[2]);

    std::cout << ", each with a max workload of " << sizeWorkGroups[0] << " by " << sizeWorkGroups[1] << " by " << sizeWorkGroups[2] << std::endl;
}

void View::paintWithComputeShaders(){
    bool isBenchMarkingWorkers = false;
    if (isBenchMarkingWorkers) {
        checkAvailableWorkers(); //see above
    }

    // TODO: compute shaders;
    { // ray tracer program block
        m_rayTracerCompProgram->bind();
        m_rayTracerCompProgram->setUniform("time", static_cast<float>(m_time.msec()/1000.f));

        m_rayTracerCompProgram->setUniform("dimensions", glm::vec2(m_width, m_height));


        glDispatchCompute(static_cast<GLuint>(std::min(m_width, 1000)),
                          static_cast<GLuint>(std::min(m_height, 1000)),
                          1); // canvas-sized number of jobs, each operating at pixel level
        m_rayTracerCompProgram->unbind();
    }

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // lock writing until ready to read

    { // traditional rendering block
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glActiveTexture(GL_TEXTURE0); // TODO: restore after figuring out the fbo issues
        glBindTexture(GL_TEXTURE_2D, m_renderOut);
        //m_fbo->getColorAttachment(0).bind();
        m_quad->draw();
        // m_fbo->unbind();
        glBindImageTexture(0, m_renderOut, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    }
}

// TODO: change if we want camera effects
void View::rebuildMatrices() {
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
             glm::rotate(m_angleY, glm::vec3(1,0,0)) *
             glm::rotate(m_angleX, glm::vec3(0,1,0));

    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    m_scale = glm::mat4({
                           m_width, 0.f, 0.f, 0.f,
                             0.f, m_height, 0.f, 0.f,
                            0.f, 0.f, 100.f, 0.f,
                            0.f, 0.f, 0.f, 1.f
                        });
    update();
}


void View::resizeGL(int w, int h) {
    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    w = static_cast<int>(w / ratio);
    h = static_cast<int>(h / ratio);

    m_width = std::min(w, 1000);
    m_height = std::min(h, 1000);
    glViewport(0, 0, m_width, m_height);

    m_fbo = std::make_unique<FBO>(1,
                                  FBO::DEPTH_STENCIL_ATTACHMENT::NONE,
                                  m_width,
                                  m_height,
                                  TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
                                  TextureParameters::FILTER_METHOD::NEAREST,
                                  GL_FLOAT
                                  );
    rebuildMatrices();
}

void View::mousePressEvent(QMouseEvent *event) {

}

void View::mouseMoveEvent(QMouseEvent *event) {
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    if(m_captureMouse) {
        int deltaX = event->x() - width() / 2;
        int deltaY = event->y() - height() / 2;
        if (!deltaX && !deltaY) return;
        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

        // TODO: Handle mouse movements here

    }
}

void View::mouseReleaseEvent(QMouseEvent *event) {

}

void View::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here
}

void View::keyReleaseEvent(QKeyEvent *event) {

}

// TODO: I don't know why time isn't being sent to fragment
void View::tick() {
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = m_time.restart() * 0.001f;

    // TODO: Implement the demo update here

    // Flag this view for repainting (Qt will call paintGL() soon after)
//    Sleep(m_sleepTime); // TODO: remove for non-Windows
    update();
}

void View::setupSpheres(){
    m_spheres[0].mass = 0.001f;
    m_spheres[1].mass = 1.0f;
    m_spheres[2].mass = 0.6f;

    m_spheres[0].force = glm::vec3({0.0f, 0.0f, 0.0f});
    m_spheres[1].force = glm::vec3({0.0f, 0.0f, 0.0f});
    m_spheres[2].force = glm::vec3({0.0f, 0.0f, 0.0f});

    m_spheres[0].position = glm::vec3({-0.5f, 0.5f, 0.0f});
    m_spheres[1].position = glm::vec3({0.2f, 0.01f, -0.25f});
    m_spheres[2].position = glm::vec3({-0.5f, 0.5f, 0.f});

    m_spheres[0].velocity = glm::vec3({0.0f, 0.0f, 0.0f});
    m_spheres[1].velocity = glm::vec3({0.0f, 0.0f, 0.0f});
    m_spheres[2].velocity = glm::vec3({0.0f, 0.0f, 0.0f});

    m_spheres[0].acceleration = glm::vec3({0.0f, 0.0f, 0.0f});
    m_spheres[1].acceleration = glm::vec3({0.0f, 0.0f, 0.0f});
    m_spheres[2].acceleration = glm::vec3({0.0f, 0.0f, 0.0f});
}

void View::setupWalls(){
    m_walls[0].position = glm::vec3({0.f, 0.f, -2.f}); // back wall
    m_walls[1].position = glm::vec3({0.f, 0.f, 0.1f}); // front wall
    m_walls[2].position = glm::vec3({-1.5f, 0.f, 0.f}); // left wall
    m_walls[3].position = glm::vec3({1.5f, 0.f, 0.f}); // right wall
    m_walls[4].position = glm::vec3({0.f, 1.5f, 0.f}); // top wall
    m_walls[5].position = glm::vec3({0.f, -1.5f, 0.f}); // bottom wall

    m_walls[0].normal = glm::vec3({0.f, 0.f, 1.f});
    m_walls[0].normal = glm::vec3({0.f, 0.f, -1.f});
    m_walls[0].normal = glm::vec3({1.f, 0.f, 0.f});
    m_walls[0].normal = glm::vec3({-1.f, 0.f, 1.f});
    m_walls[0].normal = glm::vec3({0.f, -1.f, 1.f});
    m_walls[0].normal = glm::vec3({0.f, 1.f, 1.f});
}

