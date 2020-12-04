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

using namespace CS123;
using namespace GL;


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

    std::unique_ptr<Shader> m_program;
    std::unique_ptr<OpenGLShape> m_quad;


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
