#include "figure.h"
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>

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
/*
Figure::Figure(float* initVertexes, int szVertexes):
        sizeVertexes(szVertexes), scaleAmount(0), scaleDirection(1)
{
    vertexes = new float[szVertexes];
    centeredVertexes = new float[szVertexes];
    for (int i = 0; i < szVertexes; i++) {
        vertexes[i] = initVertexes[i];
        centeredVertexes[i] = initVertexes[i];
    };

}

/*
Figure::Figure(float* initVertexes, int szVertexes, unsigned int* initIndexes, int szIndexes):
        sizeVertexes(szVertexes), sizeIndexes(szIndexes), scaleAmount(0), scaleDirection(1)
{
    vertexes = new float[szVertexes];
    centeredVertexes = new float[szVertexes];
    indexes = new unsigned int[szIndexes];
    for (int i = 0; i < szVertexes; i++) {
        vertexes[i] = initVertexes[i];
        centeredVertexes[i] = initVertexes[i];
    };
    for (int i = 0; i < szIndexes; i++) {
        indexes[i] = initIndexes[i];
    };
}
*/

Figure::~Figure()
{
    std::cout << "Destructor called!" << std::endl;
    delete [] vertexes;
    delete [] centeredVertexes;
    delete [] indexes;
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
        float originalVertex[4] = {centeredVertexes[i], centeredVertexes[i + 1], centeredVertexes[i + 2], 1.0f};

        float transformedVertex[4];
        matrixVectorMult(matrix, originalVertex, transformedVertex);

        centeredVertexes[i] = transformedVertex[0];
        centeredVertexes[i + 1] = transformedVertex[1];
        centeredVertexes[i + 2] = transformedVertex[2];

        vertexes[i] = transformedVertex[0] + center[0];
        vertexes[i + 1] = transformedVertex[1] + center[1];
        vertexes[i + 2] = transformedVertex[2] + center[2];
    }

    updateTransMatrix();

    //cout << "BEFORE:" << verticesCentrales.x << ", " << verticesCentrales.y << ", " << verticesCentrales.z << endl;
    //calculateCenter();
    //cout << "AFTER:" << verticesCentrales.x << ", " << verticesCentrales.y << ", " << verticesCentrales.z << endl;
    //cout << endl;
}

void 
Figure::scale(float scaleFactor) {
    float scalingMatrix[16] = {
        scaleFactor, 0.0f, 0.0f, 0.0f,
        0.0f, scaleFactor, 0.0f, 0.0f,
        0.0f, 0.0f, scaleFactor, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    transformVertices(scalingMatrix);
}

void 
Figure::rotate(float angle, char type) {
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

    float *rotationMatrix = new float[16];
    switch (type) {
        case 'x':
            rotationMatrix = rotationMatrixX;
            break;
        case 'y':
            rotationMatrix = rotationMatrixY;
            break;
        case 'z':
            rotationMatrix = rotationMatrixZ;
            break;
    }
    transformVertices(rotationMatrix);
}

void 
Figure::rotate_inversa(float angle, char type){ // el bool identifica que quiero aplicar una inversa 
    float rotationMatrixX[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cos(angle), sin(angle), 0.0f,
        0.0f, -sin(angle), cos(angle), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float rotationMatrixY[16] = {
        cos(angle), 0.0f, -sin(angle), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sin(angle), 0.0f, cos(angle), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    float rotationMatrixZ[16] = {
        cos(angle), sin(angle), 0.0f, 0.0f,
        -sin(angle), cos(angle), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };

    float *rotationMatrix = new float[16];
    switch (type) {
        case 'x':
            rotationMatrix = rotationMatrixX;
            break;
        case 'y':
            rotationMatrix = rotationMatrixY;
            break;
        case 'z':
            rotationMatrix = rotationMatrixZ;
            break;
    }
    transformVertices(rotationMatrix);
}

void 
Figure::move(float xoff, float yoff, float zoff) {
    float translationMatrix[16] = {
        1.0f, 0.0f, 0.0f, xoff,
        0.0f, 1.0f, 0.0f, yoff,
        0.0f, 0.0f, 1.0f, zoff,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    //cout << "BEFORE" << endl;
    //printVertexes();

    transformVertices(translationMatrix);

    //cout << "AFTER" << endl;
    //printVertexes();
}

void
Figure::updateTransMatrix()
{
    transformationMatrix = glm::mat4(1.0f);
    transformationMatrix = glm::translate(transformationMatrix, center);
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
    
    // * Calculo el centro de la figura usando el promedio de los vertices
    for (int i = 0; i < sizeVertexes; i += 6){
        //cout << "Sumando " << cubeVertex[i] << ", " << cubeVertex[i + 1] << ", " << cubeVertex[i + 2] << endl;
        x += vertexes[i];
        y += vertexes[i + 1];
        z += vertexes[i + 2];
    }
    x /= 36;
    y /= 36;
    z /= 36;

    verticesCentrales = glm::vec3(x, y, z);

    //cout << "Centro: " << x << ", " << y << ", " << z << endl;
}