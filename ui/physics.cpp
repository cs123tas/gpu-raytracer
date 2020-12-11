#include "physics.h"
#include "iostream"


Physics::Physics(int fps):
    m_fps(fps*100),
    m_eps(1e-6)
{
//    m_spheres = spheres;
//    m_walls = walls;
//    m_spheres.reserve(3);
//    m_spheres.resize(3);
//    m_walls.reserve(6);
//    m_walls.resize(6);
//    Physics::setupSpheres();
//    Physics::setupWalls();
}

Physics::~Physics()
{

}

void Physics::computeForce(glm::vec3 &force, float mass){
    force[0] = 2.0f*(mass+m_eps)*m_g[0];
    force[1] = 2.0f*(mass+m_eps)*m_g[1];
    force[2] = 2.0f*(mass+m_eps)*m_g[2];
}

void Physics::updateAcceleration(glm::vec3 &acceleration, glm::vec3 &force, float mass){
//    printf("%.4f, %.4f\n", force[1], mass);
    acceleration = force/(mass+m_eps);
//    printf("%.4f\n", force[1]/mass);
}

void Physics::updateVelocity(glm::vec3 &velocity, glm::vec3 &acceleration){
//    printf("v: %.3f\n", acceleration[1]);
    velocity += acceleration * (1.0f/m_fps);
}

void Physics::updatePosition(glm::vec3 &position, glm::vec3 &velocity){
    position += velocity * (1.0f/m_fps);
}

void Physics::collisionDetection(Sphere& sphere, std::vector<Plane>& walls){

}

void Physics::runPhysics(std::vector<Sphere> &spheres, std::vector<Plane> &walls){
    for (int i=0; i<spheres.size();i++){
        computeForce(spheres[i].force, spheres[i].mass);
//        printf("%.3f, %.3f\n", m_spheres[i].force[1], m_spheres[i].mass);
        updateAcceleration(spheres[i].acceleration, spheres[i].force, spheres[i].mass);
//        printf("%.3f\n", m_spheres[i].acceleration[1]);
        updateVelocity(spheres[i].velocity, spheres[i].acceleration);
//        printf("%.3f\n", m_spheres[i].velocity[1]);
        updatePosition(spheres[i].position, spheres[i].velocity);
    }

//    m_spheres[0].force[0] = m_spheres[0].mass*m_g[0];
//    m_spheres[0].force[1] = m_spheres[0].mass*m_g[1];
//    m_spheres[0].force[2] = m_spheres[0].mass*m_g[2];

//    m_spheres[1].force[0] = m_spheres[1].mass*m_g[0];
//    m_spheres[1].force[1] = m_spheres[1].mass*m_g[1];
//    m_spheres[1].force[2] = m_spheres[1].mass*m_g[2];

//    m_spheres[2].force[0] = m_spheres[2].mass*m_g[0];
//    m_spheres[2].force[1] = m_spheres[2].mass*m_g[1];
//    m_spheres[2].force[2] = m_spheres[2].mass*m_g[2];
////    printf("%.3f, %.3f, %.3f\n", m_spheres[0].force[1], m_spheres[1].force[1], m_spheres[2].force[1]);

//    m_spheres[0].acceleration = m_spheres[0].force/(m_spheres[0].mass+m_eps);
//    m_spheres[1].acceleration = m_spheres[1].force/(m_spheres[1].mass+m_eps);
//    m_spheres[2].acceleration = m_spheres[2].force/(m_spheres[2].mass+m_eps);
//    printf("%.3f, %.3f, %.3f\n", m_spheres[0].acceleration[1], m_spheres[1].acceleration[1], m_spheres[2].acceleration[1]);

//    m_spheres[0].velocity = m_spheres[0].acceleration * (1.0f/m_fps);
//    m_spheres[1].velocity = m_spheres[1].acceleration * (1.0f/m_fps);
//    m_spheres[2].velocity = m_spheres[2].acceleration * (1.0f/m_fps);

//    m_spheres[0].position += m_spheres[0].velocity * (1.0f/m_fps);
//    m_spheres[1].position += m_spheres[1].velocity * (1.0f/m_fps);
//    m_spheres[2].position += m_spheres[2].velocity * (1.0f/m_fps);


//    std::vector<Sphere> spheres;
//    spheres.reserve(3);
//    glm::vec3 force({m_spheres[0].mass*m_g[0], m_spheres[0].mass*m_g[1], m_spheres[0].mass*m_g[2]});
//    glm::vec3 acceleration({force/(m_spheres[0].mass+m_eps)});
//    glm::vec3 velocity({m_spheres[0].acceleration});
//    velocity *= (1.0f/m_fps);
//    glm::vec3 position(m_spheres[0].position);
//    position += velocity * (1.0f/m_fps);
//    Sphere sphere1 = {position, velocity, force, acceleration, 8.2f};
//    spheres.push_back(sphere1);

//    force = glm::vec3({m_spheres[1].mass*m_g[0], m_spheres[1].mass*m_g[1], m_spheres[1].mass*m_g[2]});
//    acceleration = glm::vec3({force/(m_spheres[1].mass+m_eps)});
//    velocity = glm::vec3({m_spheres[1].acceleration});
//    velocity *= (1.0f/m_fps);
//    position = glm::vec3(m_spheres[1].position);
//    position += velocity * (1.0f/m_fps);
//    Sphere sphere2 = {position, velocity, force, acceleration, 8.2f};
//    spheres.push_back(sphere2);

//    force = glm::vec3({m_spheres[2].mass*m_g[0], m_spheres[2].mass*m_g[1], m_spheres[2].mass*m_g[2]});
//    acceleration = glm::vec3({force/(m_spheres[2].mass+m_eps)});
//    velocity = glm::vec3({m_spheres[2].acceleration});
//    velocity *= (1.0f/m_fps);
//    position = glm::vec3(m_spheres[2].position);
//    position += velocity * (1.0f/m_fps);
//    Sphere sphere3 = {position, velocity, force, acceleration, 8.2f};
//    spheres.push_back(sphere3);

//    m_spheres = spheres;


}


//void Physics::setupSpheres(){
////    Sphere sphere1 = {glm::vec3({0.5f, -0.5f, 3.0f}), glm::vec3({0.0f, 0.0f, 0.0f}), glm::vec3({0.0f, 0.0f, 0.0f}), glm::vec3({0.0f, 0.0f, 0.0f}), 8.2f};
////    Sphere sphere2 = {glm::vec3({-0.2f, -0.01f, 5.0f}), glm::vec3({0.0f, 0.0f, 0.0f}), glm::vec3({0.0f, 0.0f, 0.0f}), glm::vec3({0.0f, 0.0f, 0.0f}), 2.0f};
////    Sphere sphere3 = {glm::vec3({0.0f, 0.25f, 0.0f}), glm::vec3({0.0f, 0.0f, 0.0f}), glm::vec3({0.0f, 0.0f, 0.0f}), glm::vec3({0.0f, 0.0f, 0.0f}), 8.2f};
////    m_spheres.push_back(sphere1);
////    m_spheres.push_back(sphere2);
////    m_spheres.push_back(sphere3);

//    m_spheres[0].mass = 8.2f;
//    m_spheres[1].mass = 2.0f;
//    m_spheres[2].mass = 0.6f;

//    m_spheres[0].force = glm::vec3({0.0f, 0.0f, 0.0f});
//    m_spheres[1].force = glm::vec3({0.0f, 0.0f, 0.0f});
//    m_spheres[2].force = glm::vec3({0.0f, 0.0f, 0.0f});

//    m_spheres[0].position = glm::vec3({0.5f, -0.5f, 3.0f});
//    m_spheres[1].position = glm::vec3({-0.2f, -0.01f, 5.0f});
//    m_spheres[2].position = glm::vec3({0.0f, 0.25f, 0.0f});

//    m_spheres[0].velocity = glm::vec3({0.0f, 0.0f, 0.0f});
//    m_spheres[1].velocity = glm::vec3({0.0f, 0.0f, 0.0f});
//    m_spheres[2].velocity = glm::vec3({0.0f, 0.0f, 0.0f});

//    m_spheres[0].acceleration = glm::vec3({0.0f, 0.0f, 0.0f});
//    m_spheres[1].acceleration = glm::vec3({0.0f, 0.0f, 0.0f});
//    m_spheres[2].acceleration = glm::vec3({0.0f, 0.0f, 0.0f});
//}

//void Physics::setupWalls(){
//    m_walls[0].position = glm::vec3({0.f, 0.f, -2.f}); // back wall
//    m_walls[1].position = glm::vec3({0.f, 0.f, 0.1f}); // front wall
//    m_walls[2].position = glm::vec3({-1.5f, 0.f, 0.f}); // left wall
//    m_walls[3].position = glm::vec3({1.5f, 0.f, 0.f}); // right wall
//    m_walls[4].position = glm::vec3({0.f, 1.5f, 0.f}); // top wall
//    m_walls[5].position = glm::vec3({0.f, -1.5f, 0.f}); // bottom wall

//    m_walls[0].normal = glm::vec3({0.f, 0.f, 1.f});
//    m_walls[0].normal = glm::vec3({0.f, 0.f, -1.f});
//    m_walls[0].normal = glm::vec3({1.f, 0.f, 0.f});
//    m_walls[0].normal = glm::vec3({-1.f, 0.f, 1.f});
//    m_walls[0].normal = glm::vec3({0.f, -1.f, 1.f});
//    m_walls[0].normal = glm::vec3({0.f, 1.f, 1.f});
//}

