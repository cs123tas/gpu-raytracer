#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <memory>
#include <QButtonGroup>
#include <QMainWindow>
#include "Settings.h"

class View;

namespace Ui {
    class MainWindow;
}

class DataBinding;

/**
 * @class MainWindow
 *
 * The main graphical user interface class (GUI class) for our application.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

    // Overridden from QWidget. Handles the window resize event.
    virtual void changeEvent(QEvent *e);

    // Overridden from QWidget. Handles the window close event.
    virtual void closeEvent(QCloseEvent *e);

private:

    // Sets up the data bindings between the UI and app settings
    void dataBind();

    // [C++ Note] private members start with m_
    QList<DataBinding*> m_bindings;
    QList<QButtonGroup*> m_buttonGroups;
    View *m_view;

    // Auto-generated by Qt. DO NOT RENAME!
    Ui::MainWindow *ui;

public slots:

    // Sets the current tab on the UI to the 2D canvas view.
    void activateCanvas2D();

    // Sets the current tab on the UI to the 3D canvas view.
    void activateCanvas3D();

    // These methods are update different aspects of the 3D camera, and delegate to the Canvas3D.
    void setCameraAxisX();
    void setCameraAxisY();
    void setCameraAxisZ();
    void updateCameraTranslation();
    void updateCameraRotationN();
    void updateCameraRotationV();
    void updateCameraRotationU();
    void resetUpVector();
    void resetSliders();
    void updateCameraClip();
    void updateCameraHeightAngle();
    void setCameraAxonometric();
};


#endif // MAINWINDOW_H
