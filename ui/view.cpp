#include "view.h"

#include "viewformat.h"
#include <QApplication>
#include <QKeyEvent>
#include <iostream>

#include "lib/ResourceLoader.h"
#include "lib/Sphere.h"
#include "gl/textures/Texture2D.h"

using namespace CS123::GL;

View::View(QWidget *parent) : QGLWidget(ViewFormat(), parent),
    m_motionBlurFBO(nullptr),
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
    m_sleepTime(50),
    m_depth(2)
{
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
    m_time.start();
    m_timer.start(1000 / 60);

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

    // Loading in post-processing shaders
    std::string motionBlurFragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/motionBlur.frag");
    m_motionBlurProgram = std::make_unique<Shader>(quadVertexSource, motionBlurFragmentSource);

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

    m_postProcessingQuad = std::make_unique<OpenGLShape>();
    m_postProcessingQuad->setVertexData(&quadData[0], quadData.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, 4);
    m_postProcessingQuad->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_postProcessingQuad->setAttribute(ShaderAttrib::TEXCOORD0, 2, 3*sizeof(GLfloat), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_postProcessingQuad->buildVAO();

    // Print the max FBO dimension.
    bool isBenchMarkingFBO = false;
    if (isBenchMarkingFBO) {
        GLint maxRenderBufferSize;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &maxRenderBufferSize);
        std::cout << "Max FBO size: " << maxRenderBufferSize << std::endl;
    }


    m_motionBlurFBO = std::make_unique<FBO>(1,
                                  FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY,
                                  std::min(1000, m_width),
                                  std::min(1000, m_height),
                                  TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE
                                  );
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
    //m_motionBlurFBO->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_rayTracerFragProgram->bind();

    glm::mat4 M_film2World = glm::inverse(m_scale*m_view);
    m_rayTracerFragProgram->setUniform("M_film2World", M_film2World);

    m_rayTracerFragProgram->setUniform("time", static_cast<float>(m_time.msec()/1000.f));
    m_rayTracerFragProgram->setUniform("dimensions", glm::vec2(m_width, m_height));
    m_rayTracerFragProgram->setUniform("depth", m_depth);
    m_rayTracerFragProgram->setUniform("rightSpeed", m_rightSpeed);
    m_rayTracerFragProgram->setUniform("leftSpeed", m_leftSpeed);
    m_rayTracerFragProgram->setUniform("centerSpeed", m_centerSpeed);
    //m_motionBlurFBO->getColorAttachment(0).bind();
    m_quad->draw();


    //m_motionBlurFBO->getColorAttachment(0).bind();
    //m_motionBlurFBO->unbind();

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //m_motionBlurProgram->bind();
    //m_postProcessingQuad->draw();
    //m_motionBlurFBO->unbind();

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //m_motionBlurProgram->unbind();
}


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
    m_view = glm::translate(glm::vec3(-2.f, -2.f, -m_zoom)) *
             glm::rotate(m_angleY, glm::vec3(1,0,0)) *
             glm::rotate(m_angleX, glm::vec3(0,1,0));

    m_projection = glm::perspective(0.8f, static_cast<float>(m_width/m_height), 0.1f, 100.f);
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

    m_motionBlurFBO = std::make_unique<FBO>(1,
                                  FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY,
                                  m_width,
                                  m_height,
                                  TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE
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
//    update();
}
