#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>

#include "figure.h"

using namespace std;

float cubeVertex[] = {
    /*
    Amarillo: 1.0f, 1.0f, 0.0f
    Verde: 0.0f, 1.0f, 0.0f
    Azul: 0.0f, 0.2f, 0.6f
    Rojo: 1.0f, 0.0f, 0.0f
    AmarilloClaro: 0.5f, 0.5f, 1.0f
    Naranja: 1.0f, 0.5f, 0.0f
    */

    // * Cara color Azul
    -0.5f, -0.5f, -0.5f,  0.0f, 0.2f, 6.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.2f, 6.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.2f, 6.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.2f, 6.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.2f, 6.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.2f, 6.0f,

    // * Cara color verde
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

    // * Cara color Naranja
    -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.0f,

    // * Cara color rojo
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

    // * Cara color amarillo
    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,

    // * Cara color blanco
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f
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
    std::cout << "Destructor called!" << std::endl;
    delete [] vertexes;
    delete [] centeredVertexes;
    //delete [] indexes;
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
    //transformVerticesCenter(translationMatrix);

    //cout << "AFTER" << endl;
    //printVertexes();
}

void 
Figure::scale(float scaleFactor) {
    float scalingMatrix[16] = {
        scaleFactor, 0.0f, 0.0f, 0.0f,
        0.0f, scaleFactor, 0.0f, 0.0f,
        0.0f, 0.0f, scaleFactor, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    //transformVertices(scalingMatrix);
    transformVerticesCenter(scalingMatrix);
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
        default:
            exit(-1);
            break;
    }
    //transformVertices(rotationMatrix);
    transformVerticesCenter(rotationMatrix);
}

void 
Figure::rotate_inversa(float angle, char type){ // el bool identifica que quiero aplicar una inversa 
    /*
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
    */

    rotate(-angle, type);
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

    //updateTransMatrix();
    calculateCenter();

    //cout << "BEFORE:" << verticesCentrales.x << ", " << verticesCentrales.y << ", " << verticesCentrales.z << endl;
    //calculateCenter();
    //cout << "AFTER:" << verticesCentrales.x << ", " << verticesCentrales.y << ", " << verticesCentrales.z << endl;
    //cout << endl;
}

void 
Figure::transformVerticesCenter(const float* matrix) {
    // 1. Calculo el centro coordenadas de la figura (Cx, Cy, Cz)
    //      * Para ello sumo todos los vertices y los divido entre la cantidad de vertices
    //      * Dicha distancia es lo que se tiene que trasladar la figura para llegar al centro

    float Cx, Cy, Cz;
    for(int i = 0; i < sizeVertexes; i += 6){
        Cx += vertexes[i];
        Cy += vertexes[i + 1];
        Cz += vertexes[i + 2];
    }
    Cx /= 36;
    Cy /= 36;
    Cz /= 36;

    // 2. Traslado la figura al origen (restando el centro a cada vertice)
    for(int i = 0; i < sizeVertexes; i += 6){
        vertexes[i] -= Cx;
        vertexes[i + 1] -= Cy;
        vertexes[i + 2] -= Cz;
    }

    // 3. Aplico la transformacion (multiplicando los vertices por la matriz)
    for(int i = 0; i < sizeVertexes; i += 6){
        float originalVertex[4] = {vertexes[i], vertexes[i + 1], vertexes[i + 2], 1.0f};

        float transformedVertex[4];
        matrixVectorMult(matrix, originalVertex, transformedVertex);

        vertexes[i] = transformedVertex[0];
        vertexes[i + 1] = transformedVertex[1];
        vertexes[i + 2] = transformedVertex[2];
    }

    // 4. Traslado la figura al centro original (sumando el centro a cada vertice)
    for(int i = 0; i < sizeVertexes; i += 6){
        vertexes[i] += Cx;
        vertexes[i + 1] += Cy;
        vertexes[i + 2] += Cz;
    }

    /*
    for (int i = 0; i < sizeVertexes; i += 6) {
        float originalVertex[4] = { centeredVertexes[i], 
                                    centeredVertexes[i + 1], 
                                    centeredVertexes[i + 2], 1.0f};

        float transformedVertex[4];
        matrixVectorMult(matrix, originalVertex, transformedVertex);

        centeredVertexes[i] = transformedVertex[0];
        centeredVertexes[i + 1] = transformedVertex[1];
        centeredVertexes[i + 2] = transformedVertex[2];

        vertexes[i] = transformedVertex[0] + verticesCentrales[0];
        vertexes[i + 1] = transformedVertex[1] + verticesCentrales[1];
        vertexes[i + 2] = transformedVertex[2] + verticesCentrales[2];
    }
    */

    calculateCenter();
}
