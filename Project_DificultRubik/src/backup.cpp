#include "glad.h"

#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

//#include "linmath.h"
#include "deps/linmath.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <iostream>
#include <string>

#include <filesystem>

#include <cmath>

#include <fstream>
#include <sstream>
#include <cerrno>

//#include <windows.h> 
#include <unistd.h>

#include <vector>
#include <queue>
#include <cstdlib>

#include "Solver/random.h"
#include "Solver/solve.h"

using std::string;
using std::vector;
using std::cout;

vector<string> Movimientos;

//VERTEX SHADER
//recibe vectores de size 3 (aPos) y devuelve vectores de size 4 (gl_Position)
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 camMatrix;"
"void main()\n"
"{\n"
"   gl_Position = camMatrix*vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

//FRAGMENT SHADER
//No tiene input, devuelve un vector de size 4 (FragColor)
//Tiene una variable uniform (puede ser modificada mientras el programa corre)
//El output es FragColor que es igual a la variable uniform
const char* fragmentShaderSource = "#version 330 core\n"
"layout(location=0) out vec4 FragColor;\n"
"uniform vec4 u_color;"
"void main()\n"
"{\n"
"   FragColor=u_color;\n"
"}\n\0";

const char* color_vs = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"uniform mat4 camMatrix;"
"void main()\n"
"{\n"
"    FragPos = vec3(vec4(aPos.x, aPos.y, aPos.z, 1.0));\n"
"    Normal =  aNormal;\n"
"    gl_Position = camMatrix * vec4(FragPos, 1.0);\n"
"}\0";

const char* frag_vs = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"uniform vec3 lightPos;"
"uniform vec3 viewPos;"
"uniform vec3 lightColor;"
"uniform vec3 objectColor;"
"void main()\n"
"{\n"
"    float ambientStrength = 0.1;\n"
"    vec3 ambient = ambientStrength * lightColor;\n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 lightDir = normalize(lightPos - FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec3 diffuse = diff * lightColor;\n"
"    float specularStrength = 0.5;\n"
"    vec3 viewDir = normalize(viewPos - FragPos);\n"
"    vec3 reflectDir = reflect(-lightDir, norm);\n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"    vec3 specular = specularStrength * spec * lightColor;\n"
"    vec3 result = (ambient + diffuse + specular) * objectColor;\n"
"    FragColor = vec4(result, 1.0);\n"
"}\0";

int contador = 0;
bool inverted = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

float* multiplicacion(float matriz1[3], float matriz2[4][4], bool vector);

// settings (size de la ventana)
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//para registrar key inputs
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

//para los inputs
int contador2 = 0;
char camadas = '0';
char camadas2 = '0';
bool restart = 0;
bool space = 0;
bool visible1 = 1, visible2 = 0, visible3 = 0, visible4 = 0;
int solverCube = 0;
std::vector<std::string> solutionSolver;
int contadorFinal = 25;
queue<char> randMoves;

float Lx, Ly, Lz;

//////////////SHADER/////////////////////
class shader {
public:
    GLuint ID;
    shader() :ID(0) {};
    shader(const char* vertexShaderSource, const char* fragmentShaderSource) {
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void activateS() {
        glUseProgram(ID);
    }

    void deleteS() {
        glDeleteProgram(ID);
    }

    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};

//////////////CUBITO////////////////////

class cubitoV2 {
public:

    float vertices[24], tempvertices[144], vertices2[144];
    unsigned int contorno[48], allFaces[6][6], allFaces2[36];
    float temp[3];
    float* res;
    int faces, cubitoFaces[3], cubitoFaces2[3];
    unsigned int faceorderX[4], faceorderY[4], faceorderZ[4];
    float colora[4], colorb[4], colorc[4];
    unsigned int VBO, EBO1, EBO2, EBO3, EBO4, EBO5;

    cubitoV2() :VBO(0), EBO1(0), EBO2(0), EBO3(0), EBO4(0), faces(0), res(NULL) {
    }

    cubitoV2(unsigned int VBO, unsigned int EBO1, unsigned int EBO2, unsigned int EBO3) {
        this->VBO = VBO;
        this->EBO1 = EBO1;
        this->EBO2 = EBO2;
        this->EBO5 = EBO3;
        faces = 1;
    }

    cubitoV2(unsigned int VBO, unsigned int EBO1, unsigned int EBO2, unsigned int EBO3, unsigned int EBO4) {
        this->VBO = VBO;
        this->EBO1 = EBO1;
        this->EBO2 = EBO2;
        this->EBO3 = EBO3;
        this->EBO5 = EBO4;
        faces = 2;
    }

    cubitoV2(unsigned int VBO, unsigned int EBO1, unsigned int EBO2, unsigned int EBO3, unsigned int EBO4, unsigned int EBO5) {
        this->VBO = VBO;
        this->EBO1 = EBO1;
        this->EBO2 = EBO2;
        this->EBO3 = EBO3;
        this->EBO4 = EBO4;
        this->EBO5 = EBO5;
        faces = 3;
    }

    void set_border(unsigned int& k, unsigned int points[4]) {
        unsigned int corner[2] = { 0,3 };
        for (int i = 0; i < 2; i++) {
            for (int j = 1; j < 3; j++) {
                contorno[k++] = points[corner[i]];
                contorno[k++] = points[j];
            }
        }
        std::cout << std::endl;
    }

    void set_faces(unsigned int* face, unsigned int points[4]) {
        unsigned int corner[2] = { 0,3 };
        unsigned int k = 0;
        for (int i = 0; i < 2; i++) {
            face[k++] = points[corner[i]];
            for (int j = 1; j < 3; j++) {
                face[k++] = points[j];
            }
        }
    }

    void create(float x, float y, float z) {
        float aux[2] = { -0.1f, 0.1f };
        int count = 0;
        float normas[6][3] = {  {0.0f,0.0f,1.0f}, 
                                {0.0f,0.0f,-1.0f},
                                {0.0f,1.0f,0.0f} ,
                                {0.0,-1.0f,0.0f} ,
                                {1.0f,0.0f,0.0f},
                                {-1.0f,0.0f,0.0f} };
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    vertices[count++] = z - aux[k];
                    vertices[count++] = y - aux[j];
                    vertices[count++] = x - aux[i];
                }
            }
        }

        count = 0;
        int caras_ind[6][4] = { {0,1,2,3},{4,5,6,7},{1,0,5,4},{3,2,7,6},{0,2,4,6},{1,3,5,7} };
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 4; j++) {
                vertices2[count++] = vertices[caras_ind[i][j] * 3];
                vertices2[count++] = vertices[caras_ind[i][j] * 3 + 1];
                vertices2[count++] = vertices[caras_ind[i][j] * 3 + 2];
                vertices2[count++] = normas[i][0];
                vertices2[count++] = normas[i][1];
                vertices2[count++] = normas[i][2];
            }


        }

        unsigned int k = 0;
        unsigned int borders[6][4] = { { 0,1,2,3 } ,
            { 1,0,5,4 },
            { 1,3,5,7 },
            { 0,2,4,6 },
            { 3,2,7,6 },
            { 4,5,6,7 } };

        //front
        set_border(k, borders[0]);
        //up
        set_border(k, borders[1]);
        //left
        set_border(k, borders[2]);
        //right
        set_border(k, borders[3]);
        //down
        set_border(k, borders[4]);
        //back
        set_border(k, borders[5]);

        unsigned int faces[6][4] = { { 1,0,3,2 },
            {5,1,4,0},
            { 0,2,4,6 },
            { 4,5,6,7 },
            { 2,3,6,7 },
            { 3,1,7,5 } };

        //FaceNear [0]
        set_faces(allFaces[0], faces[0]);
        //FaceFar [1]
        set_faces(allFaces[1], faces[3]);
        //FaceRight [2]
        set_faces(allFaces[2], faces[2]);
        //FaceLeft [3]
        set_faces(allFaces[3], faces[5]);
        //FaceUp [4]
        set_faces(allFaces[4], faces[1]);
        //FaceDown [5]
        set_faces(allFaces[5], faces[4]);

        k = 0;
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                allFaces2[k++] = allFaces[i][j];
            }
        }

        faceorderX[0] = 0;
        faceorderX[1] = 5;
        faceorderX[2] = 1;
        faceorderX[3] = 4;

        faceorderY[0] = 0;
        faceorderY[1] = 2;
        faceorderY[2] = 1;
        faceorderY[3] = 3;

        faceorderZ[0] = 4;
        faceorderZ[1] = 3;
        faceorderZ[2] = 5;
        faceorderZ[3] = 2;
    }

    int pos(unsigned int dir, unsigned int faces[4]) {
        for (int i = 0; i < 4; i++) {
            if (dir == faces[i]) {
                return i;
            }
        }
        return -1;
    }

    int* check_faces(unsigned int a) {
        int* temp = new int[2];
        for (int i = 0; i < faces; i++) {
            if (cubitoFaces2[i] == a) {
                temp[0] = 1;
                temp[1] = i;
                return temp;
            }
        }
        temp[0] = 0;
        temp[1] = -1;
        return temp;
    }

    void updatefaceX(int k, bool inverted) {
        for (int i = 0; i < 3; i++) {
            if (cubitoFaces2[i] == k) {
                continue;
            }
            if (!inverted) {
                cubitoFaces2[i] = faceorderX[(pos(cubitoFaces2[i], faceorderX) + 1) % 4];
            }
            else {
                cubitoFaces2[i] = faceorderX[(pos(cubitoFaces2[i], faceorderX) + 4 - 1) % 4];
            }
        }
    }
    void updatefaceY(int k, bool inverted) {
        for (int i = 0; i < 3; i++) {
            if (cubitoFaces2[i] == k) {
                continue;
            }
            if (!inverted) {
                cubitoFaces2[i] = faceorderY[(pos(cubitoFaces2[i], faceorderY) + 1) % 4];
            }
            else {
                cubitoFaces2[i] = faceorderY[(pos(cubitoFaces2[i], faceorderY) + 4 - 1) % 4];
            }
        }
    }
    void updatefaceZ(int k, bool inverted) {
        for (int i = 0; i < faces; i++) {
            if (cubitoFaces2[i] == k) {
                continue;
            }
            if (!inverted) {
                cubitoFaces2[i] = faceorderZ[(pos(cubitoFaces2[i], faceorderZ) + 1) % 4];
            }
            else {
                cubitoFaces2[i] = faceorderZ[(pos(cubitoFaces2[i], faceorderZ) + 4 - 1) % 4];
            }
        }
    }

    void setTemp() {
        for (int i = 0; i < 144; i++) {
            tempvertices[i] = vertices2[i];
        }
    }

    void Reset() {
        for (int i = 0; i < 144; i++) {
            vertices2[i] = tempvertices[i];
        }
        for (int i = 0; i < faces; i++) {
            cubitoFaces2[i] = cubitoFaces[i];
        }
    }

    void choosefaceOne(unsigned int face, float RGB[3]) {
        cubitoFaces[0] = face;
        cubitoFaces2[0] = face;
        colora[0] = RGB[0];
        colora[1] = RGB[1];
        colora[2] = RGB[2];
    }

    void choosefaceTwo(unsigned int face, float RGB[3]) {
        cubitoFaces[1] = face;
        cubitoFaces2[1] = face;
        colorb[0] = RGB[0];
        colorb[1] = RGB[1];
        colorb[2] = RGB[2];
    }

    void choosefaceThree(unsigned int face, float RGB[3]) {
        cubitoFaces[2] = face;
        cubitoFaces2[2] = face;
        colorc[0] = RGB[0];
        colorc[1] = RGB[1];
        colorc[2] = RGB[2];
    }

    void changing_vectors(int size, float matriz[4][4], float vertices[24], bool vector) {
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices2[i];
            temp[1] = vertices2[i + 1];
            temp[2] = vertices2[i + 2];
            res = multiplicacion(temp, matriz, vector);
            vertices2[i] = res[0];
            vertices2[i + 1] = res[1];
            vertices2[i + 2] = res[2];
        }
    }

    void rot_x(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_x[4][4] = { {1,0,0,0},{0,cos(degree),-sin(degree),0},{0,sin(degree),cos(degree),0},{0,0,0,1} };
        changing_vectors(144, rot_x, vertices2, 0);
    }

    void rot_y(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_y[4][4] = { {cos(degree),0,sin(degree),0},{0,1,0,0},{-sin(degree),0,cos(degree),0},{0,0,0,1} };
        changing_vectors(144, rot_y, vertices2, 0);
    }

    void rot_z(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_z[4][4] = { {cos(degree),-sin(degree),0,0},{sin(degree),cos(degree),0,0},{0,0,1,0},{0,0,0,1} };
        changing_vectors(144, rot_z, vertices2, 0);
    }

    void rot_xINV(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_x[4][4] = { {1,0,0,0},{0,cos(degree),sin(degree),0},{0,-sin(degree),cos(degree),0},{0,0,0,1} };
        changing_vectors(144, rot_x, vertices2, 0);
    }

    void rot_yINV(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_y[4][4] = { {cos(degree),0,-sin(degree),0},{0,1,0,0},{sin(degree),0,cos(degree),0},{0,0,0,1} };
        changing_vectors(144, rot_y, vertices2, 0);
    }

    void rot_zINV(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_z[4][4] = { {cos(degree),sin(degree),0,0},{-sin(degree),cos(degree),0,0},{0,0,1,0},{0,0,0,1} };
        changing_vectors(144, rot_z, vertices2, 0);
    }

    void translation(float x, float y, float z) {
        float trans[4][4] = { {1,0,0,x},{0,1,0,y},{0,0,1,z},{0,0,0,1} };
        changing_vectors(144, trans, vertices2, 1);
    }

    void translationINV(float x, float y, float z) {
        float trans[4][4] = { {1,0,0,-x},{0,1,0,-y},{0,0,1,-z},{0,0,0,1} };
        changing_vectors(144, trans, vertices2, 1);
    }

    void scale(float x, float y, float z) {
        float scal[4][4] = { {x,0,0,0},{0,y,0,0},{0,0,z,0},{0,0,0,1} };
        changing_vectors(144, scal, vertices2, 0);
    }

    void scaleINV(float x, float y, float z) {
        float scal[4][4] = { {1 / x,0,0,0},{0,1 / y,0,0},{0,0,1 / z,0},{0,0,0,1} };
        changing_vectors(144, scal, vertices2, 0);
    }

    void bindVBO() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 144 * sizeof(float), vertices2, GL_DYNAMIC_DRAW);
    }

    void set_contorno() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 48 * sizeof(int), contorno, GL_DYNAMIC_DRAW);
    }

    void set_allfaces() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO5);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(int), allFaces2, GL_DYNAMIC_DRAW);
    }

    void set_faceOne() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), allFaces[cubitoFaces[0]], GL_DYNAMIC_DRAW);
    }

    void set_faceTwo() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), allFaces[cubitoFaces[1]], GL_DYNAMIC_DRAW);
    }

    void set_faceThree() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(int), allFaces[cubitoFaces[2]], GL_DYNAMIC_DRAW);
    }

    void print() {
        for (int i = 0; i < 23; i += 3) {
            std::cout << "x: " << vertices[i] << " y: " << vertices[i + 1] << " z: " << vertices[i + 2] << std::endl;
        }
    }
};


/////////////CUBO//////////////////

char moves2[6] = { 'F','B','R','L','U','D' };

class Cubo {
public:
    cubitoV2 cubitos[26];

    Cubo(unsigned int VBO[26], unsigned int EBO[106]) {
        int i1 = 0, i2 = 0;
        //una cara
        for (; i1 < 6; i1++, i2 += 3) {
            cubitos[i1] = cubitoV2(VBO[i1], EBO[i2], EBO[i2 + 1], EBO[i2 + 2]);
        }
        //dos caras
        for (; i1 < 18; i1++, i2 += 4) {
            cubitos[i1] = cubitoV2(VBO[i1], EBO[i2], EBO[i2 + 1], EBO[i2 + 2], EBO[i2 + 3]);
        }
        //tres caras
        for (; i1 < 26; i1++, i2 += 5) {
            cubitos[i1] = cubitoV2(VBO[i1], EBO[i2], EBO[i2 + 1], EBO[i2 + 2], EBO[i2 + 3], EBO[i2 + 4]);
        }

        //posiciones de los colores en el array
        //blue[0] | orange[1] | purple[2] | red[3] | green[4] | yellow[5]
        float colors[6][3] = { { 0.0f,1.0f,0.0f },
            { 1.0f,0.5f,0.0f },
            { 1.0f,1.0f,0.0f },
            { 1.0f,0.0f,0.0f },
            { 0.0f,0.0f,1.0f },
            { 0.0f,1.0f,1.0f }
        };

        //una cara
        unsigned int k = 0;
        float lengths[4] = { 0.2,-0.2, 0.2, -0.2 };
        float lengths2[4] = { 0.2,0.2,-0.2,-0.2 };
        float coords[3] = { 0.0, 0.0, 0.0 };
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                coords[i] = lengths[j];
                cubitos[k].create(coords[0], coords[2], coords[1]);
                cubitos[k].choosefaceOne(k, colors[k]);
                k++;
            }
            coords[i] = 0.0;
        }

        //dos caras
        int directions[3][4] = { 
            { 0,1,4,5 } ,
           { 2,3,4,5 },
            { 2,3,0,1 } };
        int temp;
        for (int i = 0; i < 3; i++) {
            for (int j = 0, j1 = (i + 1) % 3; j < 4; j++) {
                coords[i] = lengths[j];
                coords[j1] = lengths2[j];
                if (i == 1) {
                    coords[i] = lengths2[j];
                    coords[j1] = lengths[j];
                }
                temp = j >= 2 ? 3 : 2;
                cubitos[k].create(coords[0], coords[1], coords[2]);
                cubitos[k].choosefaceOne(directions[i][j % 2], colors[directions[i][j % 2]]);
                cubitos[k++].choosefaceTwo(directions[i][temp], colors[directions[i][temp]]);
                coords[i] = 0.0; coords[j1] = 0.0;
            }
        }

        //tres caras
        unsigned int dir1[2] = { 2,3 };
        unsigned int dir2[2] = { 4,5 };
        unsigned int dir3[2] = { 0,1 };
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k1 = 0; k1 < 2; k1++) {
                    coords[0] = lengths[k1];
                    coords[1] = lengths[j];
                    coords[2] = lengths[i];
                    cubitos[k].create(coords[0], coords[1], coords[2]);
                    cubitos[k].choosefaceOne(dir1[i], colors[dir1[i]]);
                    cubitos[k].choosefaceTwo(dir2[j], colors[dir2[j]]);
                    cubitos[k++].choosefaceThree(dir3[k1], colors[dir3[k1]]);
                }
            }
        }
    }

    char translation_move(char A) {
        char camSol;
        switch (A)
        {
        case 'U':
            camSol = '5';
            break;
        case 'L':
            camSol = '4';
            break;
        case 'F':
            camSol = '1';
            break;
        case 'D':
            camSol = '6';
            break;
        case 'R':
            camSol = '3';
            break;
        case 'B':
            camSol = '2';
            break;
        }
        return camSol;
    }

    void moveCamada(unsigned int dir, char extra) {
        float movement = 3.6;
        unsigned int vel = 24;
        int* check = new int[2];

        if (dir < 6) {
            for (int i = 0; i < 26; i++) {
                check = cubitos[i].check_faces(dir);
                if (check[0] == 1) {
                    if (dir == 0) {
                        if (inverted) {
                            cubitos[i].rot_z(movement);
                        }
                        else {
                            cubitos[i].rot_zINV(movement);
                        }
                        if (contador == vel || contador == (vel * 2) + 1) {
                            cubitos[i].updatefaceZ(dir, !inverted);
                        }
                    }
                    else if (dir == 1) {
                        if (!inverted) {
                            cubitos[i].rot_z(movement);
                        }
                        else {
                            cubitos[i].rot_zINV(movement);
                        }
                        if (contador == vel || contador == (vel * 2) + 1) {
                            cubitos[i].updatefaceZ(dir, inverted);
                        }
                    }
                    else if (dir == 2) {
                        if (inverted) {
                            cubitos[i].rot_x(movement);
                        }
                        else {
                            cubitos[i].rot_xINV(movement);
                        }
                        if (contador == vel || contador == (vel * 2) + 1) {
                            cubitos[i].updatefaceX(dir, !inverted);
                        }
                    }
                    else if (dir == 3) {
                        if (!inverted) {
                            cubitos[i].rot_x(movement);
                        }
                        else {
                            cubitos[i].rot_xINV(movement);
                        }
                        if (contador == vel || contador == (vel * 2) + 1) {
                            cubitos[i].updatefaceX(dir, inverted);
                        }
                    }
                    else if (dir == 4) {
                        if (inverted) {
                            cubitos[i].rot_y(movement);
                        }
                        else {
                            cubitos[i].rot_yINV(movement);
                        }
                        if (contador == vel || contador == (vel * 2) + 1) {
                            cubitos[i].updatefaceY(dir, !inverted);
                        }
                    }
                    else if (dir == 5) {
                        if (!inverted) {
                            cubitos[i].rot_y(movement);
                        }
                        else {
                            cubitos[i].rot_yINV(movement);
                        }
                        if (contador == vel || contador == (vel * 2) + 1) {
                            cubitos[i].updatefaceY(dir, inverted);
                        }
                    }
                }
            }
        }
    }
};

/////////////CAMARA/////////////

class Camera
{
public:
    // Stores the main vectors of the camera
    glm::vec3 Camara;
    glm::vec3 Right = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    bool firstClick = true;
    float speed = 0.2f;
    float sensitivity = 100.0f;
    float fov = 90.0f;
    float near = 0.01f;
    float far = 300.0f;

    //Todo esto se usa para que la camara se mueva sola.
    int Repetitions = 0;
    float speedTraverse = 0.5f;
    float endingPoint = 11.202f;
    int totalRepetitions = 2;
    bool timeToSolve = 0;

    //para desarmar y resolver solo una vez
    bool cube1Rand = 1, cube1Solve = 1;
    bool cube2Rand = 1, cube2Solve = 1;
    bool cube3Rand = 1, cube3Solve = 1;

    Camera(glm::vec3 position)
    {
        Camara = position;
    }

    void Matrix(shader& Shader, const char* uniform)
    {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        view = glm::lookAt(Camara, Camara + Right, Up);
        projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / SCR_HEIGHT, near, far);
        
        glUniformMatrix4fv(glGetUniformLocation(Shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
    }

    void Inputs(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            Camara += speed * Right;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            Camara += speed * -glm::normalize(glm::cross(Right, Up));
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            Camara += speed * -Right;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            Camara += speed * glm::normalize(glm::cross(Right, Up));
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            Camara += speed * Up;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            Camara += speed * -Up;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            cout << Camara.x << " " << Camara.y << " " << Camara.z << endl;
            if (camadas == '0') {
                if (Repetitions >= totalRepetitions) {
                    speedTraverse = -0.5f;
                    endingPoint = 10.0f;
                    //aplicamos el solver
                    timeToSolve = 1;
                }
                if (Camara.z > 22.2f) {
                    //solo el cubo 2 y 3 no son visibles, cuando rota el cubo 1
                    visible2 = 0;
                    visible3 = 0;
                    visible4 = 0;
                    //solo podemos mover el cubo 1;
                    solverCube = 0;
                    Camara += speedTraverse * Right;
                    if (Camara.z < 73.01f && Camara.z > 72.00f && timeToSolve) {
                        solutionSolver = get_solution(to_cube_not(Movimientos));
                        Movimientos = vector<string>();
                    }
                }
                if (Camara.z < 22.2f && Camara.z > 13.56f) {
                    //al entrar al cubo 1, el cubo 2 es visible pero no el 3
                    visible2 = 1;
                    visible3 = 0;
                    visible4 = 0;
                    //solo podemos mover o resolver el cubo 2
                    solverCube = 1;
                    Camara += speedTraverse / 10 * Right;
                    if (Camara.z < 21.5f && Camara.z > 21.4f && timeToSolve) {
                        solutionSolver = get_solution(to_cube_not(Movimientos));
                        Movimientos = vector<string>();
                    }
                }
                if (Camara.z < 13.56f && Camara.z > 11.6081f) {
                    //al entrar al cubo 2 el cubo 3 es visible
                    visible3 = 1;
                    visible4 = 0;
                    //solo podemos mover el cubo 3
                    solverCube = 2;
                    Camara += speedTraverse / 10 / 5 * Right;
                    if (Camara.z < 13.36f && Camara.z > 13.32 && timeToSolve) {
                        solutionSolver = get_solution(to_cube_not(Movimientos));
                        Movimientos = vector<string>();
                    }
                }
                if (Camara.z < 11.6081f && Camara.z > 11.2f) {
                    //al entrar al cubo 3 el cubo 4 es visible
                    visible4 = 1;
                    //solo podemos mover el cubo 4
                    solverCube = 3;
                    Camara += speedTraverse / 10 / 5 / 5 * Right;
                    if (Camara.z < 11.582f && Camara.z > 11.578 && timeToSolve) {
                        solutionSolver = get_solution(to_cube_not(Movimientos));
                        Movimientos = vector<string>();
                    }
                }
                if (Repetitions <= totalRepetitions) {
                    if (Camara.z <= endingPoint) {
                        Camara.x = -20.0f;
                        Camara.y = 20.0f;
                        if ((Repetitions + 1) < totalRepetitions) {
                            Camara.z = 22.1;
                        }
                        Repetitions++;
                    }
                }
                else if (Repetitions <= totalRepetitions * 2) {
                    if (Camara.z >= endingPoint) {
                        Camara.x = -20.0f;
                        Camara.y = 20.0f;
                        Camara.z = 11.202;
                        Repetitions++;
                    }
                }
            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            if (firstClick)
            {
                glfwSetCursorPos(window, (SCR_WIDTH / 2), (SCR_HEIGHT / 2));
                firstClick = false;
            }

            double mouseX;
            double mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            float rotX = sensitivity * (float)(mouseY - (SCR_HEIGHT / 2)) / SCR_HEIGHT;
            float rotY = sensitivity * (float)(mouseX - (SCR_WIDTH / 2)) / SCR_WIDTH;

            glm::vec3 newOrientation = glm::rotate(Right, glm::radians(-rotX), glm::normalize(glm::cross(Right, Up)));

            if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
            {
                Right = newOrientation;
            }

            Right = glm::rotate(Right, glm::radians(-rotY), Up);

            glfwSetCursorPos(window, (SCR_WIDTH / 2), (SCR_HEIGHT / 2));
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstClick = true;
        }
    }
};

void create_figure(shader& shaderInst1, cubitoV2& temp, bool contorno, float cx, float cy, float cz);

void set_rubik(Cubo Rubik, shader& inst) {
    int i = 0;
    for (; i < 6; i++) {
        Rubik.cubitos[i].set_faceOne();
    }
    for (; i < 18; i++) {
        Rubik.cubitos[i].set_faceOne();
        Rubik.cubitos[i].set_faceTwo();
    }
    for (; i < 26; i++) {
        Rubik.cubitos[i].set_faceOne();
        Rubik.cubitos[i].set_faceTwo();
        Rubik.cubitos[i].set_faceThree();
    }

    for (int i = 0; i < 26; i++) {
        Rubik.cubitos[i].bindVBO();
        Rubik.cubitos[i].set_contorno();
        Rubik.cubitos[i].set_allfaces();
        //Rubik.moveCamada(0);
        create_figure(inst, Rubik.cubitos[i], 1, 0.0, 0.0, 0.0);
    }
}

void random(int n) {
    char possibleMoves[6] = { '1','2','3','4','5','6' };
    std::string movesToletters[6] = { "F'","B'","R'","L'","U'","D'" };
    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++) {
        int numRand = rand();
        randMoves.push(possibleMoves[numRand % 6]);
        Movimientos.push_back(movesToletters[numRand % 6]);
        std::cout << possibleMoves[numRand % 6] << std::endl;
        std::cout << movesToletters[numRand % 6] << std::endl;
    }
}

inline void cubeMoves(Cubo* rubik1, Cubo* rubik2, Cubo* rubik3, Cubo* rubik4, int camada, char camada2) {
    switch (solverCube)
    {
    case 0:
        rubik1->moveCamada(camada, camada2);
        break;
    case 1:
        rubik2->moveCamada(camada, camada2);
        break;
    case 2:
        rubik3->moveCamada(camada, camada2);
        break;
    case 3:
        rubik4->moveCamada(camada, camada2);
        break;
    }
}

int main()
{
    // glfw: initialize and configure (da la version y tipo de perfil en este caso CORE)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned int VAO, VBO[26], EBO[106];
    glGenVertexArrays(1, &VAO);
    glGenBuffers(26, VBO);
    glGenBuffers(106, EBO);

    glBindVertexArray(VAO);

    Cubo Rubik(VBO, EBO);
    for (int i = 0; i < 26; i++) {
        Rubik.cubitos[i].setTemp();
    }

    shader shaderInst1(color_vs, frag_vs);

    int location = glGetUniformLocation(shaderInst1.ID, "objectColor");
    if (location == -1) {
        printf("Error al conseguir uniform");
        exit(1);
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int VAO2, VBO2[26], EBO2[106];
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(26, VBO2);
    glGenBuffers(106, EBO2);

    glBindVertexArray(VAO2);

    Cubo Rubik2(VBO2, EBO2);
    for (int i = 0; i < 26; i++) {
        Rubik2.cubitos[i].setTemp();
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int VAO3, VBO3[26], EBO3[106];
    glGenVertexArrays(1, &VAO3);
    glGenBuffers(26, VBO3);
    glGenBuffers(106, EBO3);

    glBindVertexArray(VAO3);

    Cubo Rubik3(VBO3, EBO3);
    for (int i = 0; i < 26; i++) {
        Rubik3.cubitos[i].setTemp();
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int VAO4, VBO4[26], EBO4[106];
    glGenVertexArrays(1, &VAO4);
    glGenBuffers(26, VBO4);
    glGenBuffers(106, EBO4);

    glBindVertexArray(VAO4);

    Cubo Rubik4(VBO4, EBO4);
    for (int i = 0; i < 26; i++) {
        Rubik4.cubitos[i].setTemp();
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glLineWidth((GLfloat)5);
    glPointSize((GLfloat)5);

    Camera camera1(glm::vec3(-20.0f, 20.0f, 60.0f));

    glfwSetKeyCallback(window, keyCallback);

    int i;

    for (int i = 0; i < 26; i++) {
        Rubik.cubitos[i].scale(75.0, 75.0, 75.0);
        Rubik2.cubitos[i].scale(15.0, 15.0, 15.0);
        Rubik2.cubitos[i].translation(-16.0, 16.0, 9.0);
        Rubik3.cubitos[i].scale(3.0, 3.0, 3.0);
        Rubik3.cubitos[i].translation(-19.2, 19.2, 10.7);
        Rubik4.cubitos[i].scale(0.7, 0.7, 0.7);
        Rubik4.cubitos[i].translation(-19.81, 19.81, 10.98);
    }


    Lx = 40.2f;
    Ly = 20.0f;
    Lz = 45.0f;
    glm::vec3 lightPos(Lx, Ly, Lz);


    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        camera1.Inputs(window);

        glm::vec3 lightPos(camera1.Camara);
        std::cout << "Lx: " << Lx << " Ly " << Ly << " Lz " << Lz << std::endl;
        shaderInst1.activateS();
        shaderInst1.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shaderInst1.setVec3("lightPos", lightPos);
        shaderInst1.setVec3("viewPos", camera1.Camara);
        camera1.Matrix(shaderInst1, "camMatrix");

        glBindVertexArray(VAO);
        if (restart) {
            for (int i = 0; i < 26; i++) {
                Rubik.cubitos[i].Reset();
            }
            Movimientos = vector<string>();
            restart = 0;
        }

        //SETTING CUBES
        if (visible1) {
            set_rubik(Rubik, shaderInst1);
        }
        glBindVertexArray(VAO2);
        if (visible2) {
            set_rubik(Rubik2, shaderInst1);
        }
        glBindVertexArray(VAO3);
        if (visible3) {
            set_rubik(Rubik3, shaderInst1);
        }
        glBindVertexArray(VAO4);
        if (visible4) {
            set_rubik(Rubik4, shaderInst1);
        }
        glBindVertexArray(VAO);

        if (contador2 != solutionSolver.size() && camadas == '0') {
            inverted = 0;
            contadorFinal = 25;
            camadas2 = '0';
            char camSol = Rubik.translation_move(solutionSolver[contador2][0]);
            camadas = camSol;
            if (solutionSolver[contador2].size() > 1) {
                camadas2 = solutionSolver[contador2][1];
            }
            if (camadas2 == '2') {
                contadorFinal = 50;
            }
            if (camadas2 == '\'') {
                inverted = 1;
            }
        }
        else if (contador2 == solutionSolver.size()) {
            contador2 = 0;
            contadorFinal = 25;
            if (camadas2 == '\'') {
                inverted = 0;
            }
            camadas2 = '0';
            solutionSolver.clear();
        }

        if (!randMoves.empty() && camadas == '0') {
            camadas = randMoves.front();
        }

        //Devuelve los cubos al centros para luego aplicar alguna rotacion
        if (camadas != '0')
            for (int i = 0; i < 26; i++) {
                Rubik2.cubitos[i].translationINV(-16.0, 16.0, 9.0);
                Rubik2.cubitos[i].scaleINV(15.0, 15.0, 15.0);
                Rubik3.cubitos[i].translationINV(-19.2, 19.2, 10.7);
                Rubik3.cubitos[i].scaleINV(3.0, 3.0, 3.0);
                Rubik4.cubitos[i].translationINV(-19.81, 19.81, 10.98);
                Rubik4.cubitos[i].scaleINV(0.7, 0.7, 0.7);
            }

        switch (camadas)
        {
        case '1':
            cubeMoves(&Rubik, &Rubik2, &Rubik3, &Rubik4, 0, camadas2);
            break;
        case '2':
            cubeMoves(&Rubik, &Rubik2, &Rubik3, &Rubik4, 1, camadas2);
            break;
        case '3':
            cubeMoves(&Rubik, &Rubik2, &Rubik3, &Rubik4, 2, camadas2);
            break;
        case '4':
            cubeMoves(&Rubik, &Rubik2, &Rubik3, &Rubik4, 3, camadas2);
            break;
        case '5':
            cubeMoves(&Rubik, &Rubik2, &Rubik3, &Rubik4, 4, camadas2);
            break;
        case '6':
            cubeMoves(&Rubik, &Rubik2, &Rubik3, &Rubik4, 5, camadas2);
            break;
        }

        if (camadas != '0') {
            contador++;
            //regresa los cubos a sus posiciones originales
            for (int i = 0; i < 26; i++) {
                Rubik2.cubitos[i].scale(15.0, 15.0, 15.0);
                Rubik2.cubitos[i].translation(-16.0, 16.0, 9.0);
                Rubik3.cubitos[i].scale(3.0, 3.0, 3.0);
                Rubik3.cubitos[i].translation(-19.2, 19.2, 10.7);
                Rubik4.cubitos[i].scale(0.7, 0.7, 0.7);
                Rubik4.cubitos[i].translation(-19.81, 19.81, 10.98);
            }
        }

        if (contador == contadorFinal) {
            camadas = '0';
            contador = 0;
            if (solutionSolver.size() != 0) {
                contador2++;
            }
            if (!randMoves.empty()) {
                randMoves.pop();
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, VBO);
    glDeleteBuffers(2, EBO);
    shaderInst1.deleteS();

    glfwTerminate();
    return 0;
}

//Dibuja la figura que queramos
void create_figure(shader& shaderInst1, cubitoV2& temporal, bool contorno, float cx, float cy, float cz) {
    glBindBuffer(GL_ARRAY_BUFFER, temporal.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //glUniform4f(location, temporal.colora[0], temporal.colora[1], temporal.colora[2], 1.0f);
    shaderInst1.setVec3("objectColor", temporal.colora[0], temporal.colora[1], temporal.colora[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temporal.EBO1);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    if (temporal.faces > 1) {
        //glUniform4f(location, temporal.colorb[0], temporal.colorb[1], temporal.colorb[2], 1.0f);
        shaderInst1.setVec3("objectColor", temporal.colorb[0], temporal.colorb[1], temporal.colorb[2]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temporal.EBO3);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    if (temporal.faces > 2) {
        //glUniform4f(location, temporal.colorc[0], temporal.colorc[1], temporal.colorc[2], 1.0f);
        shaderInst1.setVec3("objectColor", temporal.colorc[0], temporal.colorc[1], temporal.colorc[2]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temporal.EBO4);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    if (contorno) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temporal.EBO2);
        //glUniform4f(location, cx, cy, cz, 1.0f);
        shaderInst1.setVec3("objectColor", cx, cy, cz);
        glDrawElements(GL_LINES, 48, GL_UNSIGNED_INT, 0);
    }
    //glUniform4f(location, 0.0f, 0.0f, 0.0f, 1.0f);
    shaderInst1.setVec3("objectColor", 0.0f, 0.0f, 0.0f);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temporal.EBO5);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f);
    shaderInst1.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
    //glDrawElements(GL_POINTS, 24 * 2, GL_UNSIGNED_INT, 0);
}

//Key input
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //si presionas a muestra la estrella
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '1';
            Movimientos.push_back("F");
        }
    }
    //si presiones b muestra la flecha
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '2';
            Movimientos.push_back("B");
        }
    }
    //si presiones d muestra el circulo
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '3';
            Movimientos.push_back("R");
        }
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '4';
            Movimientos.push_back("L");
        }
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '5';
            Movimientos.push_back("U");
        }
    }
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '6';
            Movimientos.push_back("D");
        }
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        if (camadas == '0') {
            inverted = !inverted;
        }
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        if (camadas == '0') {
            solutionSolver = get_solution(to_cube_not(Movimientos));
            std::cout << "SOLUTION------" << std::endl;
            for (int i = 0; i < solutionSolver.size(); i++) {
                std::cout << solutionSolver[i] << std::endl;
            }
            std::cout << "END SOLUTION---------" << std::endl;
            Movimientos = vector<string>();
        }
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        if (camadas == '0') {
            restart = 1;
        }
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        for (int i = 0; i < Movimientos.size(); i++) {
            std::cout << "Movimiento: " << i << " Es " << Movimientos[i] << std::endl;
        }
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        if (camadas == '0') {
            random(10);
        }
    }
    if (key == GLFW_KEY_I) {
        Ly += 0.8;
    }
    if (key == GLFW_KEY_K) {
        Ly -= 0.8;
    }
    if (key == GLFW_KEY_J) {
        Lx -= 0.8;
    }
    if (key == GLFW_KEY_L) {
        Lx += 0.8;
    }
    if (key == GLFW_KEY_U) {
        Lz += 0.8;
    }
    if (key == GLFW_KEY_O) {
        Lz -= 0.8;
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

float* multiplicacion(float matriz1[3], float matriz2[4][4], bool vector) {
    float temp[4][1];
    float new_matriz1[4][1];
    for (int i = 0; i < 3; i++) {
        new_matriz1[i][0] = matriz1[i];
        for (int j = 0; j < 1; j++) {
            temp[i][j] = 0;
        }
    }
    !vector ? new_matriz1[3][0] = 0 : new_matriz1[3][0] = 1;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1; j++) {
            for (int k = 0; k < 4; k++) {
                temp[i][j] += matriz2[i][k] * new_matriz1[k][j];
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        matriz1[i] = temp[i][0];
    }

    return matriz1;
}