//
//  Particles.cpp
//  CSE167-Final
//
//  Created by Freddy Sauceda on 12/6/16.
//  Copyright © 2016 Alfredo Sauceda. All rights reserved.
//

#include "ParticleGen.h"

ParticleGen::ParticleGen(int amount, GLuint texture, GLuint shader)
: amount(amount), texture(texture), shaderProgram(shader) {
    
    e1 = std::default_random_engine(r());
    uniform_real = std::uniform_real_distribution<float>(-100.0f,100.0f);

    GLfloat particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    //Fill mess buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    
    //Set mesh attrib
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    // Create this->amount default particle instances
    for (GLuint i = 0; i < this->amount; ++i){
        this->particles.push_back(Particle());
    }
}
//Store first unused particle
GLuint lastUsedParticle = 0;
// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
GLuint ParticleGen::firstUnusedParticle(){
    
    // First search from last used particle, this will usually return almost instantly
    for (GLuint i = lastUsedParticle; i < this->amount; ++i){
        if (this->particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // Otherwise, do a linear search
    for (GLuint i = 0; i < lastUsedParticle; ++i){
        if (this->particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}
// Respawns particle
void ParticleGen::respawnParticle(Particle &particle){
//    srand((unsigned int)time(NULL));
    
    float randomX = uniform_real(e1);
    float randomZ = uniform_real(e1);
    
//    GLfloat randomX = ((rand() % 200) - 100);
//    GLfloat randomZ = ((rand() & 200) - 100);
    GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
    particle.Position = glm::vec3(randomX, CLOUD_HEIGHT, randomZ);//Randomize positions on the xy-plane, z should be same
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f); //randomize the color
    particle.Life = 2.0f;
    particle.Velocity = glm::vec3(0.0f,-100.0f,0.0f); //Velocity should be straight down always (try 1/10th of grav)
}

void ParticleGen::draw(glm::mat4 C){
    // Use additive blending to give it a 'glow' effect
    toWorld = C;
    
    glm::mat4 modelview = Window::V * toWorld;
    glUseProgram(shaderProgram);

    //Get projection and modelview locations
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    uModel = glGetUniformLocation(shaderProgram, "model");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &this->toWorld[0][0]);
    
    //Reset particles drawn
    Window::parts_drawn = 0;
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (Particle particle : this->particles)
    {
        if (particle.Life > 0.0f)
        {
            //Increment particles drawn
            Window::parts_drawn++;

            glUniform3f(glGetUniformLocation(shaderProgram, "partPos"), particle.Position.x, particle.Position.y, particle.Position.z);
            glUniform4f(glGetUniformLocation(shaderProgram, "color"), particle.Color.x,particle.Color.y,particle.Color.z,particle.Color.w);
            
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    // Don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void ParticleGen::update(){}

void ParticleGen::update(GLfloat dt, GLuint newParticles)
{
    // Add new particles
    for (GLuint i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle]);
    }
    // Update all particles
    for (GLuint i = 0; i < this->amount; ++i)
    {
        Particle &p = this->particles[i];
        p.Life -= dt; // reduce life
        if (p.Life > 0.0f)
        {	// particle is alive, thus update
            p.Position += p.Velocity * dt;
            p.Color.a -= dt * 2.5;
        }
    }
}
