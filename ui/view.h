#ifndef VIEW_H
#define VIEW_H

#include "GL/glew.h"
#include <qgl.h>
#include <QTime>
#include <QTimer>

#include "memory"

#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr

#include "../gl/shaders/Shader.h"
#include "../lib/OpenGLShape.h"
#include "../gl/datatype/FBO.h"

#include "physics.h"

using namespace CS123::GL;


class View : public QGLWidget {
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    QTime m_time;
    QTimer m_timer;

    bool m_captureMouse;
    int m_height;
    int m_width;

    GLuint m_renderOut; // TODO: remove

    std::unique_ptr<Shader> m_rayTracerCompProgram; // gpgpu
    std::unique_ptr<Shader> m_textureProgram; // fullscreen quad
    std::unique_ptr<Shader> m_rayTracerFragProgram; // glsl raytracer
    std::unique_ptr<Shader> m_motionBlurProgram; // post-processing effect

    std::unique_ptr<OpenGLShape> m_quad;

    std::unique_ptr<FBO> m_motionBlurFBO;

    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::mat4 m_scale;

    float m_angleX;
    float m_angleY;
    float m_zoom;

    void paintWithComputeShaders();
    void paintWithFragmentShaders();

    void rebuildMatrices();

    float m_leftSpeed;
    float m_centerSpeed;
    float m_rightSpeed;
    int m_sleepTime;
    int m_depth;


    // Rigid Physics
    void setupSpheres();
    void setupWalls();
    std::vector<Sphere> m_spheres;
    std::vector<Plane> m_walls;
    Physics m_physics;
    int m_increment;
    float m_fps, m_friction, m_dt;
    glm::vec3 m_g;
    double m_tick;

    // Inheritted from QWidget
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void tick();
};

#endif // VIEW_H
