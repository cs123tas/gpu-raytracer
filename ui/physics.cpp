#include "physics.h"


Physics::Physics(int fps):
    m_fps(fps*1000000)
{
    m_g = glm::vec3(0.0f);
}

Physics::~Physics()
{

}

void Physics::computeForce(glm::vec3& forceVec, float mass){
    forceVec[0] = mass*m_g[0];
    forceVec[1] = mass*m_g[1];
    forceVec[2] = mass*m_g[2];
}

void Physics::updateAcceleration(glm::vec3& acceleration, glm::vec3& force, float mass){
    acceleration = force/mass;
}

void Physics::updateVelocity(glm::vec3 &velocity, glm::vec3 &acceleration){
//    velocity += acceleration * (1.0f/m_fps);
    velocity = acceleration * (1.0f/m_fps);
}

void Physics::updatePosition(glm::vec3 &position, glm::vec3 &velocity){
    position += velocity * (1.0f/m_fps);
}

void Physics::collisionDetection(Sphere& sphere, std::vector<Plane>& walls){

}

void Physics::runPhysics(std::vector<Sphere>& spheres){
    for (int i=0; i<spheres.size();i++){
        Physics::computeForce(spheres[i].force, spheres[i].mass);
        Physics::updateAcceleration(spheres[i].acceleration, spheres[i].force, spheres[i].mass);
        Physics::updateVelocity(spheres[i].velocity, spheres[i].acceleration);
        Physics::updatePosition(spheres[i].position, spheres[i].velocity);
    }
}
