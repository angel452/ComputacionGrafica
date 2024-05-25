#ifndef DRAYS_H
#define DRAYS_H

#include <stdio.h>
#include <glm/glm.hpp>
#include "shaderClass.h"

class deathRay {
    public:
    unsigned int VAO, VBO;
    float vertexes[6];
    float step;
    glm::vec3 camPos, camDir;

    deathRay(glm::vec3, glm::vec3);
    void updateStep();
    void renderInit();
    void render(ShaderClass shp);
    glm::vec3 getCenter();
};

#endif