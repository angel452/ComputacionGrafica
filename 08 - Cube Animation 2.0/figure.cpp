#include <cmath>
#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>

#include "figure.h"

using namespace std;

float cubeVertex[] = {
    // * Cara color rojo
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

    // * Cara color verde
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

    // * Cara color azul
    -0.5f,  0.5f,  0.5f,  0.0f, 0.2f, 0.6f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.2f, 0.6f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.2f, 0.6f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.2f, 0.6f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.2f, 0.6f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.2f, 0.6f,

    // * Cara color amarillo
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

    // * Cara color amarilloClaro???
    -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 1.0f,

    // * Cara color naraja
    -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f
};

Figure::Figure():
        sizeVertexes(sizeof(cubeVertex) / sizeof(float))
{
    centeredVertexes = new float[sizeVertexes];
    vertexes = new float[sizeVertexes];

    for (int i = 0; i < sizeVertexes; i++) {
        vertexes[i] = cubeVertex[i];
        centeredVertexes[i] = cubeVertex[i];
    }
}

Figure::~Figure()
{
    // std::cout << "Destructor called!" << std::endl;
    delete [] vertexes;
    delete [] centeredVertexes;
}

void
Figure::move(float xoff, float yoff, float zoff) 
{
    float translationMatrix[16] = {
        1.0f, 0.0f, 0.0f, xoff,
        0.0f, 1.0f, 0.0f, yoff,
        0.0f, 0.0f, 1.0f, zoff,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    transformVertices(translationMatrix);
}

void
Figure::scale(float scaleFactor)
{
    float translationMatrix[16] = {
        scaleFactor, 0.0f, 0.0f, 1.0f,
        0.0f, scaleFactor, 0.0f, 1.0f,
        0.0f, 0.0f, scaleFactor, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    transformVerticesCenter(translationMatrix);
}

void
Figure::rotate(float angle, char axis)
{
    float rotationMatrixX[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cos(angle), -sin(angle), 0.0f,
        0.0f, sin(angle), cos(angle), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float rotationMatrixY[16] = {
        cos(angle), 0.0f, sin(angle), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sin(angle), 0.0f, cos(angle), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float rotationMatrixZ[16] = {
        cos(angle), -sin(angle), 0.0f, 0.0f,
        sin(angle), cos(angle), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    float *translationMatrix = new float[16];
    switch (axis) {
        case 'x':
            translationMatrix = rotationMatrixX;
            break;
        case 'y':
            translationMatrix = rotationMatrixY;
            break;
        case 'z':
            translationMatrix = rotationMatrixZ;
            break;
        default:
            exit(-1);
            break;
    }

    transformVerticesCenter(translationMatrix);
}

void
Figure::rotate_inversa(float angle, char axis)
{
    rotate(-angle, axis);
}

void
Figure::printVertexes()
{
    for (int i = 0; i < sizeVertexes; i += 6) {
        printf("x: %f, y: %f, z: %f\n", vertexes[i], vertexes[i + 1], vertexes[i + 2]);
    }
}

void
Figure::calculateCenter(){
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    verticesCentrales = glm::vec3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < sizeVertexes; i += 6){
        x += vertexes[i];
        y += vertexes[i + 1];
        z += vertexes[i + 2];
    }
    x /= 36;
    y /= 36;
    z /= 36;

    verticesCentrales = glm::vec3(x, y, z);
}

void 
Figure::matrixVectorMult(const float* matrix, const float* in, float* out) {
    for (int i = 0; i < 4; i++) {
        out[i] = 0.0f;
        for (int j = 0; j < 4; j++) {
            out[i] += matrix[i * 4 + j] * in[j];
        }
    }
}

void 
Figure::transformVertices(const float* matrix) {
    for (int i = 0; i < sizeVertexes; i += 6) {
        float originalVertex[4] = {vertexes[i], vertexes[i + 1], vertexes[i + 2], 1.0f};

        float transformedVertex[4];
        matrixVectorMult(matrix, originalVertex, transformedVertex);

        vertexes[i] = transformedVertex[0];
        vertexes[i + 1] = transformedVertex[1];
        vertexes[i + 2] = transformedVertex[2];
    }

    calculateCenter();
}

void 
Figure::transformVerticesCenter(const float* matrix) {
    for (int i = 0; i < sizeVertexes; i += 6) {
        float originalVertex[4] = {centeredVertexes[i], centeredVertexes[i + 1], centeredVertexes[i + 2], 1.0f};

        float transformedVertex[4];
        matrixVectorMult(matrix, originalVertex, transformedVertex);

        centeredVertexes[i] = transformedVertex[0];
        centeredVertexes[i + 1] = transformedVertex[1];
        centeredVertexes[i + 2] = transformedVertex[2];

        vertexes[i] = transformedVertex[0] + verticesCentrales[0];
        vertexes[i + 1] = transformedVertex[1] + verticesCentrales[1];
        vertexes[i + 2] = transformedVertex[2] + verticesCentrales[2];
    }

    calculateCenter();
}
