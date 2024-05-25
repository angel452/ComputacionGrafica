#include <iostream>
#include <math.h>
#include <vector>
using namespace std;

float cubeVertex[] = {
    // * Cara color rojo
    -0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f,
    1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
    1.0f,  0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
};

class Figure {
public:
    int sizeVertexes;
    float *vertexes;
    float *centeredVertexes; // No se para que se usa
    vector<float> verticesCentrales;

    Figure();
    // Figure(float, float, float);
    // Figure(float*, int);
    // Figure(float*, int, unsigned int*, int);

    void rotate(float, char);
    void matrixVectorMult(const float *, const float *, float *);
    void transformVerticesCenter(const float *);
    void calculateCenter();
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


void Figure::matrixVectorMult(const float *matrix, const float *in,
                              float *out) {
    for (int i = 0; i < 4; i++) {
        out[i] = 0.0f;
        for (int j = 0; j < 4; j++) {
        out[i] += matrix[i * 4 + j] * in[j];
        }
    }
}

void Figure::calculateCenter() {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    verticesCentrales.push_back(0.0f);
    verticesCentrales.push_back(0.0f);
    verticesCentrales.push_back(0.0f);

    // * Calculo el centro de la figura usando el promedio de los vertices
    for (int i = 0; i < sizeVertexes; i += 6) {
        // cout << "Sumando " << cubeVertex[i] << ", " << cubeVertex[i + 1] << ", "
        // << cubeVertex[i + 2] << endl;
        x += vertexes[i];
        y += vertexes[i + 1];
        z += vertexes[i + 2];
    }
    x /= 36;
    y /= 36;
    z /= 36;

    verticesCentrales[0] = x;
    verticesCentrales[1] = y;
    verticesCentrales[2] = z;

    // cout << "Centro: " << x << ", " << y << ", " << z << endl;
}

void Figure::transformVerticesCenter(const float *matrix) {
    for (int i = 0; i < sizeVertexes; i += 6) {
        float originalVertex[4] = { centeredVertexes[i], 
                                    centeredVertexes[i + 1],
                                    centeredVertexes[i + 2], 1.0f};

        cout << endl;
        cout << "Vertice original: " << endl;
        for (int i = 0; i < 4; i++) {
            cout << originalVertex[i] << ", ";
        }

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

void Figure::rotate(float angle, char type) {
    float rotationMatrixX[16] = {
        1.0f, 0.0f,       0.0f,       0.0f, 
        0.0f, cos(angle), -sin(angle), 0.0f,
        0.0f, sin(angle), cos(angle), 0.0f, 
        0.0f, 0.0f,       0.0f,        1.0f};
    
    float rotationMatrixY[16] = {
        cos(angle),  0.0f, sin(angle), 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f,
        -sin(angle), 0.0f, cos(angle), 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f};
    
    float rotationMatrixZ[16] = {
        cos(angle), -sin(angle), 0.0f, 0.0f, 
        sin(angle), cos(angle), 0.0f, 0.0f,
        0.0f,       0.0f,        1.0f, 0.0f, 
        0.0f,       0.0f,       0.0f, 1.0f};

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
    // transformVertices(rotationMatrix);

    cout << endl;
    cout << "Rotando " << type << " " << angle << " radianes" << endl;
    cout << "Matriz de rotacion: " << endl;
    for (int i = 0; i < 16; i++) {
        cout << rotationMatrix[i] << ", ";
        if ((i + 1) % 4 == 0) {
            cout << endl;
        }
    }
    cout << endl;
    transformVerticesCenter(rotationMatrix);
}

int main() {
    Figure test;

    // --> Variables inicializadas en el constructor
    cout << "Size: " << test.sizeVertexes << endl;  
    cout << "Vertexes: " << endl;
    for (int i = 0; i < test.sizeVertexes; i++) {
        cout << test.vertexes[i] << ", ";
    }
    cout << endl;
    cout << "Centered Vertexes: " << endl;
    for (int i = 0; i < test.sizeVertexes; i++) {
        cout << test.centeredVertexes[i] << ", ";
    }
    cout << endl;

    test.rotate(M_PI / 2, 'x');
}