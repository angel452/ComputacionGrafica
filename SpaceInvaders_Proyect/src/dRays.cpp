#include "dRays.h"

deathRay::deathRay(glm::vec3 mCamPos, glm::vec3 mCamFront):
    camPos(mCamPos), camDir(glm::normalize(mCamFront)), step(0.0f)
{
    glm::vec3 tempInitial = camPos;
    glm::vec3 tempFinal = camPos + 0.1f * - camDir;

    vertexes[0] = tempInitial[0];
    vertexes[1] = tempInitial[1];
    vertexes[2] = tempInitial[2];
    vertexes[3] = tempFinal[0];
    vertexes[4] = tempFinal[1];
    vertexes[5] = tempFinal[2];
    
    renderInit();
}

void
deathRay::updateStep()
{
    step = step + 0.1f;

    glm::vec3 direction(vertexes[3] - vertexes[0], vertexes[4] - vertexes[1], vertexes[5] - vertexes[2]);

    direction = glm::normalize(direction);

    glm::vec3 startPoint(vertexes[0] - step * direction.x, 
                         vertexes[1] - step * direction.y, 
                         vertexes[2] - step * direction.z);

    glm::vec3 endPoint(vertexes[3] - step * direction.x, 
                       vertexes[4] - step * direction.y, 
                       vertexes[5] - step * direction.z);

    vertexes[0] = startPoint.x;
    vertexes[1] = startPoint.y;
    vertexes[2] = startPoint.z;
    vertexes[3] = endPoint.x;
    vertexes[4] = endPoint.y;
    vertexes[5] = endPoint.z;

    //printf("step: %f, coord1: %f, %f, %f; coord2:  %f, %f, %f\n", step, vertexes[0], vertexes[1], vertexes[2], vertexes[3], vertexes[4], vertexes[5]);

}

glm::vec3
deathRay::getCenter()
{
    return glm::vec3((vertexes[0] + vertexes[3]) / 2, (vertexes[1] + vertexes[4]) / 2, (vertexes[2] + vertexes[5]) / 2);
}

void
deathRay::renderInit()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertexes, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
deathRay::render(ShaderClass shp)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, getCenter());
    shp.setMat4("model", model);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertexes, GL_STATIC_DRAW);
    glDrawArrays(GL_LINES, 0, 12);
}
