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
    m_fbo(nullptr),
    m_time(),
    m_timer(),
    m_captureMouse(false),
    m_height(height()),
    m_width(width()),
    m_angleX(-0.5f),
    m_angleY(0.5f),
    m_zoom(4.f)
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

// TODO: not sure if useful for final product, part of benchmarking
static void checkAvailableWorkers() {
    /*
     * This checks how many workers are
     * available for the task, changes between machines
     * Dan: max global (total) work group counts x:65535 y:65535 z:65535
     * max local (in one shader) work group sizes x:1024 y:1024 z:64
     */
    int numWorkGroups[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &numWorkGroups[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &numWorkGroups[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &numWorkGroups[2]);

    printf("max global (total) work group counts x:%i y:%i z:%i\n",
      numWorkGroups[0], numWorkGroups[1], numWorkGroups[2]);

    int sizeWorkGroups[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &sizeWorkGroups[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &sizeWorkGroups[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &sizeWorkGroups[2]);

    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
      sizeWorkGroups[0], sizeWorkGroups[1], sizeWorkGroups[2]);
}

void View::rebuildMatrices() {
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
             glm::rotate(m_angleY, glm::vec3(1,0,0)) *
             glm::rotate(m_angleX, glm::vec3(0,1,0));

    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    update();
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
    std::string rayTracerSource = ResourceLoader::loadResourceFileToString(":/shaders/rayTracer.comp");
    m_rayTracerProgram = std::make_unique<Shader>(rayTracerSource);

    bool isBenchMarkingWorkers = true;
    if (isBenchMarkingWorkers) {
        checkAvailableWorkers(); //see above
    }

    // Full screen quad
    std::string quadVertexSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.vert");
    std::string quadFragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.frag");
    m_textureProgram = std::make_unique<Shader>(quadVertexSource, quadFragmentSource);

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

    // TODO: abstract this in the texure class, remove instance m_renderOut
    glGenTextures(1, &m_renderOut);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_renderOut);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_height, m_width, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, m_renderOut, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void View::paintGL() {

    { // ray tracer program block
        m_rayTracerProgram->bind();
        glm::mat4 M_film2World = glm::inverse(m_view); // TODO: scale matrix
        glm::vec4 eye = M_film2World*glm::vec4(0.f, 0.f, 0.f, 1.f);

        m_rayTracerProgram->setUniform("M_film2World", M_film2World);
        m_rayTracerProgram->setUniform("eye", eye);
        m_rayTracerProgram->setUniform("height", 512);
        m_rayTracerProgram->setUniform("width", 512);

        // abstract out? unique to compute shader which is why I think not
        glDispatchCompute(static_cast<GLuint>(512), static_cast<GLuint>(512), 1); // 512 by 512 pixels
    }

     glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // lock writing until ready to read

    {  // traditional rendering block
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_textureProgram->bind();
        // TODO: Implement the demo rendering here

        glActiveTexture(GL_TEXTURE0); // TODO: is this abstracted?
        glBindTexture(GL_TEXTURE_2D, m_renderOut);
        m_quad->draw();
    }

     m_textureProgram->unbind();
     m_rayTracerProgram->unbind();
}


void View::resizeGL(int w, int h) {
    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    w = static_cast<int>(w / ratio);
    h = static_cast<int>(h / ratio);

    w = 512;
    h = 512;
    glViewport(0, 0, w, h); // TODO: restore, keep it here for compute concerns

    m_fbo = std::make_unique<FBO>(1, FBO::DEPTH_STENCIL_ATTACHMENT::NONE, w, h, TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE);
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

void View::tick() {
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = m_time.restart() * 0.001f;

    // TODO: Implement the demo update here

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
