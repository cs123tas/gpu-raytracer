#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Databinding.h"
#include "view.h"
#include <QGLFormat>
#include <QFileDialog>
#include <QMessageBox>
#include "QSettings"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Make sure the settings are loaded before the UI
    settings.loadSettingsOrDefaults();


    QGLFormat qglFormat;
    qglFormat.setVersion(4, 3);
    qglFormat.setProfile(QGLFormat::CoreProfile);
    qglFormat.setSampleBuffers(true);
    ui->setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(ui->view);
    m_view = new View(this);
    gridLayout->addWidget(m_view, 0, 1);

    // Restore the UI settings
    QSettings qtSettings("CS123", "CS123");
    restoreGeometry(qtSettings.value("geometry").toByteArray());
    restoreState(qtSettings.value("windowState").toByteArray());

    // Resize the window because the window is huge since all docks were visible.
    resize(1000, 600);

//    ui->setupUi(this);

    QList<QAction*> actions;
}

MainWindow::~MainWindow()
{
    foreach (DataBinding *b, m_bindings)
        delete b;
//    foreach (QButtonGroup *bg, m_buttonGroups)
//        delete bg;
    delete ui;
}

void MainWindow::dataBind() {
    // Brush dock
#define BIND(b) { \
    DataBinding *_b = (b); \
    m_bindings.push_back(_b); \
    assert(connect(_b, SIGNAL(dataChanged()), this, SLOT(settingsChanged()))); \
}
//    QButtonGroup *shapesButtonGroup = new QButtonGroup;
//    m_buttonGroups.push_back(shapesButtonGroup);


    // Shapes dock
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->shapeParameterSlider1, ui->shapeParameterTextbox1, settings.shapeParameter1, -100.f, 100.f))
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->shapeParameterSlider2, ui->shapeParameterTextbox2, settings.shapeParameter2, -100.f, 100.f))
    BIND(FloatBinding::bindSliderAndTextbox(
        ui->shapeParameterSlider3, ui->shapeParameterTextbox3, settings.shapeParameter3, -100.f, 100.f))

//    BIND(ChoiceBinding::bindTabs(ui->centralWidget, settings.currentTab))

#undef BIND
    // make sure the aspect ratio updates when m_canvas3D changes size
//    connect(m_view, SIGNAL(aspectRatioChanged()), this, SLOT(updateAspectRatio()));
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e); // allow the superclass to handle this for the most part...

    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Save the settings before we quit
    settings.saveSettings();
    QSettings qtSettings("CS123", "CS123");
    qtSettings.setValue("geometry", saveGeometry());
    qtSettings.setValue("windowState", saveState());

    // Stop any raytracing, otherwise the thread will hang around until done
//    ui->canvas2D->cancelRender();

    QMainWindow::closeEvent(event);
}
