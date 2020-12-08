/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
#include <Settings.h>

CamtransCamera::CamtransCamera():
    m_aspectRatio(1.f),
    m_near(1.f),
    m_far(30.f),
    m_thetaH(60.f),

    m_eye(glm::vec4(2.f, 2.f, 2.f, 1.f)),
    m_up(glm::vec4(0.f, 1.f, 0.f, 0.f)),

    m_w(glm::normalize(glm::vec4(2.f, 2.f, 2.f, 0.f))),
    m_v(glm::normalize(m_up - glm::dot(m_up.xyz(), m_w.xyz())*m_w)),
    m_u(glm::vec4(glm::cross(m_v.xyz(), m_w.xyz()), 0.f))
{
    // @TODO: [CAMTRANS] Fill this in...;
    updateViewMatrix();
    updateProjectionMatrix();
}

void CamtransCamera::setAspectRatio(float a)
{
    // @TODO: [CAMTRANS] Fill this in...
    m_aspectRatio = a;
    updateProjectionMatrix();
}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
//    return glm::mat4x4();
    return m_perspectiveTransformation*m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
//    return glm::mat4x4();
    return m_rotationMatrix*m_translationMatrix;
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
//    return glm::mat4x4();
    return m_scaleMatrix;
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
//    return glm::mat4x4();
    return m_perspectiveTransformation;
}

glm::vec4 CamtransCamera::getPosition() const {
    // @TODO: [CAMTRANS] Fill this in...
//    return glm::vec4();
    return m_eye;
}

glm::vec4 CamtransCamera::getLook() const {
    // @TODO: [CAMTRANS] Fill this in...
//    return glm::vec4();
    return -m_w;
}

glm::vec4 CamtransCamera::getUp() const {
    // @TODO: [CAMTRANS] Fill this in...
//    return glm::vec4();
    return m_up;
}

float CamtransCamera::getAspectRatio() const {
    // @TODO: [CAMTRANS] Fill this in...
//    return 0;
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const {
    // @TODO: [CAMTRANS] Fill this in...
//    return 0;
    return m_thetaH;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    // @TODO: [CAMTRANS] Fill this in...
    m_eye = eye;
    m_up = up;

    m_w = glm::normalize(-look);
    m_v = glm::normalize(m_up - glm::dot(m_up.xyz(), m_w.xyz())*m_w);
    m_u = (glm::vec4(glm::cross(m_v.xyz(), m_w.xyz()), 0.f));

    updateViewMatrix();
    updateProjectionMatrix();
}

void CamtransCamera::setHeightAngle(float h) {
    // @TODO: [CAMTRANS] Fill this in...
    m_thetaH = h;

//    float width = (1.f/m_far)/tan(glm::radians(m_thetaH/2.f))/m_aspectRatio;
    float width  = m_far*glm::tan(glm::radians(m_thetaH/2.f))*m_aspectRatio;
    m_thetaW = 2.f*glm::atan(width/(m_far));

    updateProjectionMatrix();
}

void CamtransCamera::translate(const glm::vec4 &v) {
    // @TODO: [CAMTRANS] Fill this in...
    m_eye = m_eye + v;
    updateViewMatrix();
}

void CamtransCamera::rotateU(float degrees) {
    // @TODO: [CAMTRANS] Fill this in...
    float theta = glm::radians(degrees);
    glm::vec4 v0 = m_v;
    glm::vec4 w0 = m_w;

// u = u0; v = w0 ∗ sin(θ) + v0 ∗ cos(θ); w = w0 ∗ cos(θ) − v0 ∗ sin(θ)
    m_v = w0*glm::sin(theta) + v0*glm::cos(theta);
    m_w = w0*glm::cos(theta) - v0*glm::sin(theta);

    updateViewMatrix();

}

void CamtransCamera::rotateV(float degrees) {
    // @TODO: [CAMTRANS] Fill this in...
    float theta = glm::radians(degrees);
    glm::vec4 u0 = m_u;
    glm::vec4 w0 = m_w;

    //u = u0 ∗ cos(θ) − w0 ∗ sin(θ); v = v0; w = u0 ∗ sin(θ) + w0 ∗ cos(θ)
    m_u = u0*glm::cos(theta) - w0*glm::sin(theta);
    m_w = u0*glm::sin(theta) + w0*glm::cos(theta);
    updateViewMatrix();
}

void CamtransCamera::rotateW(float degrees) {
    // @TODO: [CAMTRANS] Fill this in...
    float theta = glm::radians(degrees);
    glm::vec4 u0 = m_u;
    glm::vec4 v0 = m_v;

//  u = u0 ∗ cos(θ) − v0 ∗ sin(θ); v = u0 ∗ sin(θ) + v0 ∗ cos(θ); w = w0
    m_u = u0*glm::cos(-theta) - v0*glm::sin(-theta);
    m_v = u0*glm::sin(-theta) + v0*glm::cos(-theta);
    updateViewMatrix();
}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    // @TODO: [CAMTRANS] Fill this in...
    m_near = nearPlane;
    m_far = farPlane;
    updateProjectionMatrix();
}

glm::vec4 CamtransCamera::getU() {
    return m_u;
}

glm::vec4 CamtransCamera::getV() {
    return m_v;
}

glm::vec4 CamtransCamera::getW() {
    return m_w;
}


void CamtransCamera::updateProjectionMatrix() {
    updateScaleMatrix();
    updatePerspectiveMatrix();
}

void CamtransCamera::updatePerspectiveMatrix(){
    float c = -m_near/m_far;
    m_perspectiveTransformation = glm::transpose(glm::mat4({
                                                            1.f, 0.f, 0.f, 0.f,
                                                            0.f, 1.f, 0.f, 0.f,
                                                            0.f, 0.f, -1.f/(c + 1.f), c/(c + 1.f),
                                                            0.f, 0.f, -1.f, 0.f
                                                            }));
}

void CamtransCamera::updateScaleMatrix() {
//    float h = 2.f*(tan(glm::radians(m_thetaH/2.f)) * m_far);
    float h = 1.f/(m_far * tan(glm::radians(m_thetaH/2.f)));
    float w = h/m_aspectRatio;
    m_scaleMatrix = glm::transpose(glm::mat4({
                                              w, 0.f, 0.f, 0.f,
                                              0.f, h, 0.f, 0.f,
                                              0.f, 0.f, 1.f/m_far, 0.f,
                                              0.f, 0.f, 0.f, 1.f
                                            }));

}

void CamtransCamera::updateViewMatrix() {
    updateTranslationMatrix();
    updateRotationMatrix();
}

void CamtransCamera::updateRotationMatrix(){
    m_rotationMatrix = glm::transpose(glm::mat4({
                                                    m_u.x, m_u.y, m_u.z, 0.f,
                                                    m_v.x, m_v.y, m_v.z, 0.f,
                                                    m_w.x, m_w.y, m_w.z, 0.f,
                                                    0.f, 0.f, 0.f, 1.f
                                                }));

}

void CamtransCamera::updateTranslationMatrix(){
    m_translationMatrix = glm::transpose(glm::mat4({
                                                       1.f, 0.f, 0.f, -m_eye.x,
                                                       0.f, 1.f, 0.f, -m_eye.y,
                                                       0.f, 0.f, 1.f, -m_eye.z,
                                                       0.f, 0.f, 0.f, 1.f
                                                   }));
}
