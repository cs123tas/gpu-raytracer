#ifndef PHYSICS_H
#define PHYSICS_H

#include "vector"
#include "memory"
#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions

// Rigid Physics
struct Sphere{
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 force;
    glm::vec3 acceleration;
    float mass;
};

struct Plane{
    glm::vec3 position;
    glm::vec3 normal;
};

class Physics{

public:
    Physics(int fps);
    virtual ~Physics();

    // Rigid physics
    void computeForce(glm::vec3& forceVec, float mass);
    void updateAcceleration(glm::vec3& acceleration, glm::vec3& force, float mass);
    void updateVelocity(glm::vec3 &velocity, glm::vec3 &acceleration);
    void updatePosition(glm::vec3 &position, glm::vec3 &velocity);

    void collisionDetection(Sphere& sphere, std::vector<Plane>& walls);
    void runPhysics(std::vector<Sphere>& spheres);

    glm::vec3 m_g;
    int m_fps;
    float m_eps;
};

#endif // PHYSICS_H
