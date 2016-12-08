//
//  Particles.hpp
//  CSE167-Final
//
//  Created by Freddy Sauceda on 12/6/16.
//  Copyright © 2016 Alfredo Sauceda. All rights reserved.
//

#ifndef Particles_hpp
#define Particles_hpp

#include <stdio.h>
#include "Geode.h"
#include "Window.h"
#include <random>
#include <time.h>

#define CLOUD_HEIGHT 50.0f

// Represents a single particle and its state
struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    GLfloat Life;
    
    Particle() : Position(glm::vec3(0.0f,CLOUD_HEIGHT, 0.0f)), Velocity(0.0f), Color(1.0f), Life(0.0f) {    }
};

class ParticleGen : public Geode{
    
public:
    ParticleGen(int amount, GLuint texture, GLuint shader);
    void draw(glm::mat4 C);
    void update();
    void update(GLfloat dt, GLuint newParticles);

private:
    int amount;
    GLuint texture, shaderProgram;
    std::vector<Particle> particles;
    
    GLuint uModel, uProjection, uModelview;
    
    // Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    GLuint firstUnusedParticle();
    // Respawns particle
    void respawnParticle(Particle &particle);
    
};

#endif /* Particles_hpp */
