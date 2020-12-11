/*!

 Settings.h
 CS123 Support Code

 @author  Evan Wallace (edwallac)
 @date    9/1/2010

 This file contains various settings and enumerations that you will need to
 use in the various assignments. The settings are bound to the GUI via static
 data bindings.

**/

#include "Settings.h"
#include <QFile>
#include <QSettings>

Settings settings;


/**
 * Loads the application settings, or, if no saved settings are available, loads default values for
 * the settings. You can change the defaults here.
 */
void Settings::loadSettingsOrDefaults() {
    // Set the default values below
    QSettings s("CS123", "CS123");


    shapeParameter1 = s.value("shapeParameter1", 15).toDouble();
    shapeParameter2 = s.value("shapeParameter2", 15).toDouble();
    shapeParameter3 = s.value("shapeParameter3", 15).toDouble();

    // Camtrans
    useOrbitCamera = s.value("useOrbitCamera", true).toBool();
    cameraFov = s.value("cameraFov", 55).toDouble();
    cameraNear = s.value("cameraNear", 0.1).toDouble();
    cameraFar = s.value("cameraFar", 50).toDouble();


    currentTab = s.value("currentTab", view).toBool();

    // These are for computing deltas and the values don't matter, so start all dials in the up
    // position
    cameraPosX = 0;
    cameraPosY = 0;
    cameraPosZ = 0;
    cameraRotU = 0;
    cameraRotV = 0;
    cameraRotN = 0;

}

void Settings::saveSettings() {
    QSettings s("CS123", "CS123");

    s.setValue("shapeParameter1", shapeParameter1);
    s.setValue("shapeParameter2", shapeParameter2);
    s.setValue("shapeParameter3", shapeParameter3);


    s.setValue("currentTab", currentTab);
}

int Settings::getSceneMode() {
    return 0;
}

//int Settings::getCameraMode() {
//    if (this->useOrbitCamera)
//        return CAMERAMODE_ORBIT;
//    else
//        return CAMERAMODE_CAMTRANS;
//}
