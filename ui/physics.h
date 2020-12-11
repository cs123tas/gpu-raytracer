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
    float radius;
};

struct Plane{
    glm::vec3 position;
    glm::vec3 normal;
};

class Physics{

public:
    Physics(int fps, glm::vec3 g);
    virtual ~Physics();

    // Rigid physics
    void computeForce(glm::vec3 &force, float mass);
    void updateAcceleration(glm::vec3 &acceleration, glm::vec3 &force, float mass);
    void updateVelocity(glm::vec3 &velocity, glm::vec3 &acceleration);
    void updatePosition(glm::vec3 &position, glm::vec3 &velocity);

    glm::vec3 collisionDetection(Sphere& sphere, std::vector<Plane> walls);
    void runPhysics(std::vector<Sphere> &spheres, std::vector<Plane> &walls);
    Sphere runPhysics(Sphere sphere, std::vector<Plane> walls);

    void setupSpheres();
    void setupWalls();

    float sum(glm::vec3 vec);

    glm::vec3 m_g;
    int m_fps;
    float m_eps;
//    std::vector<Sphere> m_spheres;
//    std::vector<Plane> m_walls;

};

#endif // PHYSICS_H
