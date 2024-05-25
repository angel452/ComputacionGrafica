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

// ############################ VARIABLES GLOBALES ############################
// --> Vertex y Fragment shader
const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\n\0";

// --> Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// --> Variables para las camadas
char camadas = '0';
char camadas2 = '0';

// ######################## ENCABEZADOS ########################
// --> Funciones
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
float* multiplicacion(float matriz1[3], float matriz2[4][4], bool vector);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
//void set_rubik(Cubo Rubik);

// ############################ CLASE CAMERA ############################
class Camera {
    public:
        // Stores the main vectors of the camera
        glm::vec3 Camara;
        glm::vec3 Right = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        bool firstClick = true;
        float speed = 0.2f;
        float sensitivity = 100.0f;
        float fov = 45.0f;
        float near = 0.01f;
        float far = 300.0f;

        Camera(glm::vec3 position)
        {
            Camara = position;
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
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            {
                Camara += speed * Up;
            }
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            {
                Camara += speed * -Up;
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

// ############################ CLASE SHADER ############################
class ShaderClass{
    public:
        unsigned int ID_ShaderProgram;

        ShaderClass(const char* vertexShader_File, const char* fragmentShader_File){
            // --> Leer los archivos de los shaders
            //string vertexShader_Code = leer_Contenido_Shaders(vertexShader_File);
            //string fragmentShader_Code = leer_Contenido_Shaders(fragmentShader_File);

            // --> Convertir los archivos a char*
            //const char* vertexShader_Source = vertexShader_Code.c_str();
            //const char* fragmentShader_Source = fragmentShader_Code.c_str();

            int success;
            char infoLog[512];

            // --> Configuración de shaders (vertexShader)
            unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader);
            // * Verificar errores de compilación del vertex shader
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success){
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            }

            // --> Configuración de shaders (fragmentShader)
            unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
            glCompileShader(fragmentShader);
            // * Verificar errores de compilación del fragment shader
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success){
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }

            // --> Configuración del shader program
            ID_ShaderProgram = glCreateProgram();
            glAttachShader(ID_ShaderProgram, vertexShader);
            glAttachShader(ID_ShaderProgram, fragmentShader);
            glLinkProgram(ID_ShaderProgram);
            // * Verificar errores de compilación del shader program
            glGetProgramiv(ID_ShaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(ID_ShaderProgram, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n";
            }

            // --> Eliminar los shaders
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        }

        void Activar_ShaderProgram(){
            glUseProgram(ID_ShaderProgram);
        }

        void Eliminar_ShaderProgram(){
            glDeleteProgram(ID_ShaderProgram);
        }

        // utility uniform functions
        void setBool(const std::string &name, bool value) const
        {         
            glUniform1i(glGetUniformLocation(ID_ShaderProgram, name.c_str()), (int)value); 
        }
        
        void setInt(const std::string &name, int value) const
        { 
            glUniform1i(glGetUniformLocation(ID_ShaderProgram, name.c_str()), value); 
        }
        
        void setFloat(const std::string &name, float value) const
        { 
            glUniform1f(glGetUniformLocation(ID_ShaderProgram, name.c_str()), value); 
        }

        void setVec2(const std::string &name, const glm::vec2 &value) const
        { 
            glUniform2fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, &value[0]); 
        }
        
        void setVec2(const std::string &name, float x, float y) const
        { 
            glUniform2f(glGetUniformLocation(ID_ShaderProgram, name.c_str()), x, y); 
        }
        
        void setVec3(const std::string &name, const glm::vec3 &value) const
        { 
            glUniform3fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, &value[0]); 
        }
        
        void setVec3(const std::string &name, float x, float y, float z) const
        { 
            glUniform3f(glGetUniformLocation(ID_ShaderProgram, name.c_str()), x, y, z); 
        }
        
        void setVec4(const std::string &name, const glm::vec4 &value) const
        { 
            glUniform4fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, &value[0]); 
        }
        
        void setVec4(const std::string &name, float x, float y, float z, float w) const
        { 
            glUniform4f(glGetUniformLocation(ID_ShaderProgram, name.c_str()), x, y, z, w); 
        }
        
        void setMat2(const std::string &name, const glm::mat2 &mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        
        void setMat3(const std::string &name, const glm::mat3 &mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        
        void setMat4(const std::string &name, const glm::mat4 &mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
};

// ############################ CLASE CUBITO ############################
class Cubito {
private:
    int nVertices;
    float vertices[54];
    glm::vec3 position;
    glm::vec2 rotacion;
    int faces;

public:
    //float vertices2[144];
    float nuevosPuntos[3];
    float* res;

    int cubitoFaces[3], cubitoFaces2[3];
    unsigned int VBO;

    Cubito(){
        this->VBO = 0;
    }

    // --> Constructor para 1 cara
    Cubito(unsigned int VBO, int nCaras) {
        this->VBO = VBO;
        faces = nCaras;
        cout << "Creando cubito con " << faces << " caras\n";
        nVertices = (6 * 3) * faces; // (nVerticesCuadrado * CoordenadasXYZ) * nCaras
    }

    void createCubito(float verticesFaces[], glm::vec3 positions, glm::vec2 rotaciones){
        // -->  Filtramos por el numero de caras
        if(faces == 1){
            
            cout << endl << "Guardando vertices de 1 cara\n";

            // * Guardamos los vertices
            for(int i = 0; i < nVertices; i++){
                vertices[i] = verticesFaces[i];
            }
            // * Guardamos su posicion
            position = positions;
            // * Guardamos su rotacion
            rotacion = rotaciones;
        }
        else if (faces == 2){
            
            cout << "Guardando vertices de 2 caras\n";
        }
        else if (faces == 3){
                
            cout << "Guardando vertices de 3 caras\n";

        }
    }

    void actualizarPuntos(int size, float matriz[4][4], float vertices[], bool vector) {
        for (int i = 0; i < size; i += 3) {
            nuevosPuntos[0] = vertices[i];
            nuevosPuntos[1] = vertices[i + 1];
            nuevosPuntos[2] = vertices[i + 2];

            res = multiplicacion(nuevosPuntos, matriz, vector);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }

    void printVertices(){
        cout << "Vertices: " << endl;
        for (int i = 0; i < nVertices; i++) {
            cout << vertices[i] << " ";
        }
        cout << endl;
    }

    void drawCubie(ShaderClass shaderProgram){
        //printVertices();
        //cout << endl;

        glm::mat4 model = glm::mat4(1.0f);
        shaderProgram.setMat4("model", model);

        glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(float), vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, nVertices);
    }

    int returnNVertices() {
        return nVertices;
    }

    glm::vec3 returnPosition() {
        return position;
    }

    glm::vec2 returnRotacion() {
        return rotacion;
    }

    float* returnVertices() {
        return vertices;
    }

    void rotacionX(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_x[4][4] = { {1,0,0,0},{0,cos(degree),-sin(degree),0},{0,sin(degree),cos(degree),0},{0,0,0,1} };
        actualizarPuntos(nVertices, rot_x, vertices, 0);
    }

    void rotacion_y(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_y[4][4] = { {cos(degree),0,sin(degree),0},{0,1,0,0},{-sin(degree),0,cos(degree),0},{0,0,0,1} };
        actualizarPuntos(nVertices, rot_y, vertices, 0);
    }

    void rotacion_z(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_z[4][4] = { {cos(degree),-sin(degree),0,0},{sin(degree),cos(degree),0,0},{0,0,1,0},{0,0,0,1} };
        actualizarPuntos(nVertices, rot_z, vertices, 0);
    }

    void rot_xINV(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_x[4][4] = { {1,0,0,0},{0,cos(degree),sin(degree),0},{0,-sin(degree),cos(degree),0},{0,0,0,1} };
        actualizarPuntos(nVertices, rot_x, vertices, 0);
    }

    void rot_yINV(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_y[4][4] = { {cos(degree),0,-sin(degree),0},{0,1,0,0},{sin(degree),0,cos(degree),0},{0,0,0,1} };
        actualizarPuntos(nVertices, rot_y, vertices, 0);
    }

    void rot_zINV(float angle) {
        float degree = angle * (3.141592653589793238463 / 180);
        float rot_z[4][4] = { {cos(degree),sin(degree),0,0},{-sin(degree),cos(degree),0,0},{0,0,1,0},{0,0,0,1} };
        actualizarPuntos(nVertices, rot_z, vertices, 0);
    }

    void translation(float x, float y, float z) {
        float trans[4][4] = {   { 1, 0, 0, x},
                                { 0, 1, 0, y},
                                { 0, 0, 1, z},
                                { 0, 0, 0, 1}};
        actualizarPuntos(nVertices, trans, vertices, 1);
    }

    void translationINV(float x, float y, float z) {
        float trans[4][4] = { {1,0,0,-x},{0,1,0,-y},{0,0,1,-z},{0,0,0,1} };
        actualizarPuntos(nVertices, trans, vertices, 1);
    }

    void scale(float x, float y, float z) {
        float scal[4][4] = { {x,0,0,0},{0,y,0,0},{0,0,z,0},{0,0,0,1} };
        actualizarPuntos(nVertices, scal, vertices, 0);
    }

    void scaleINV(float x, float y, float z) {
        float scal[4][4] = { {1 / x,0,0,0},{0,1 / y,0,0},{0,0,1 / z,0},{0,0,0,1} };
        actualizarPuntos(nVertices, scal, vertices, 0);
    }

    void bindVBO() {
        if(faces == 1){
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(float), vertices, GL_DYNAMIC_DRAW);
        }
        else if (faces == 2){

        }
        else if (faces == 3){

        }
    }
};

// ############################ CLASE CUBO ############################
class Cubo {
    public:
        /*Logica
        - Guardamos en cubitos los 26 cubitos del cubo
        - El orden sera:
            - 6 cubitos con 1 cara
            - 12 cubitos con 2 caras
            - 8 cubitos con 3 caras
        */
        Cubito cubitos[26];

        Cubo(unsigned int VBO[26]) {
            // --> 3 tipos de cubitos: 1 cara, 2 caras, 3 caras
            //* 1 cara (6 cubitos)
            for( int indiceCubitos = 0; indiceCubitos < 6; indiceCubitos++){
                // - NCARAS = 1
                cubitos[indiceCubitos] = Cubito(VBO[indiceCubitos], 1);
            }

            //* 2 caras (12 cubitos)
            for( int indiceCubitos = 6; indiceCubitos < 18; indiceCubitos++) {
                // - NCARAS = 2
                cubitos[indiceCubitos] = Cubito(VBO[indiceCubitos], 2);
            }

            //* 3 caras (8 cubitos)
            for( int indiceCubitos = 18; indiceCubitos < 26; indiceCubitos++) {
                // - NCARAS = 3
                cubitos[indiceCubitos] = Cubito(VBO[indiceCubitos], 3);
            }

            // --> Les pasamos los vertices a cada cubito
            // * 1 cara (6 cubitos)
            // - Graficamos 1 cara
            float example_oneFace[] = {
                -0.5f, -0.5f, -0.5f, // 0
                 0.5f, -0.5f, -0.5f, // 1
                 0.5f,  0.5f, -0.5f, // 2
                 0.5f,  0.5f, -0.5f, // 3
                -0.5f,  0.5f, -0.5f, // 4
                -0.5f, -0.5f, -0.5f  // 5
            };

            // - Creamos (a partir de arreglo), las otras 3 caras
            glm::vec3 positions[] = {
                glm::vec3(  1.2f,  1.2f,  0.0f), // Top
                glm::vec3(  1.2f,  0.0f,  1.2f), // Mid - Front
                glm::vec3(  1.2f,  0.0f,  0.0f), // Mid - Right
                glm::vec3( -1.2f,  0.0f,  0.0f), // Mid - Left
                glm::vec3(  1.2f,  0.0f, -1.2f), // Mid - Back
                glm::vec3(  1.2f, -1.2f,  0.0f), // Bottom
            };

            glm::vec2 rotaciones[] = { // 1 -> x, 2 -> y, 3 -> z
                glm::vec2( 1.0f ,  90.0f), // Top           (x)
                glm::vec2( 2.0f , 90.0f), // Mid - Front   (z)
                glm::vec2( 2.0f , 90.0f), // Mid - Right   (z)
                glm::vec2( 2.0f , 90.0f), // Mid - Left    (z)
                glm::vec2( 2.0f , 90.0f), // Mid - Back    (z)
                glm::vec2( 1.0f ,  90.0f), // Bottom        (x)
            };
            
            // - Le pasamos al cubito los vertices, las posiciones y las rotaciones
            for( int indiceCubitos = 0; indiceCubitos < 6; indiceCubitos++) {
                cubitos[indiceCubitos].createCubito(example_oneFace, positions[indiceCubitos], rotaciones[indiceCubitos]);
            }

            // * 2 caras (12 cubitos)

            // * 3 caras (8 cubitos)

            // --> Seteamos los vertices de cada cubito con las posiciones
            //* 1 cara (6 cubitos)
            for( int indiceCubitos = 0; indiceCubitos < 6; indiceCubitos++){
                cout << endl << "Set cube " << indiceCubitos << endl << endl;

                // --> Traslada los cubitos a su posicion
                float trasX = cubitos[indiceCubitos].returnPosition().x;
                float trasY = cubitos[indiceCubitos].returnPosition().y;
                float trasZ = cubitos[indiceCubitos].returnPosition().z;

                cubitos[indiceCubitos].translation(trasX, trasY, trasZ);

                // --> Rota los cubitos a su posicion
                float eje = cubitos[indiceCubitos].returnRotacion().x;
                float grados = cubitos[indiceCubitos].returnRotacion().y;
                if(eje == 1.0f){
                    cout << "Rotando en X " << grados << " grados\n";
                    cubitos[indiceCubitos].rotacionX(grados);
                }
                else if(eje == 2.0f){
                    cout << "Rotando en Y " << grados << " grados\n";
                    cubitos[indiceCubitos].rotacion_y(grados);
                }
                else if(eje == 3.0f){
                    cout << "Rotando en Z " << grados << " grados\n";
                    cubitos[indiceCubitos].rotacion_z(grados);
                }

                // --> Bindeamos el VBO
                cubitos[indiceCubitos].bindVBO();
            }

            //* 2 caras (12 cubitos)
            for (int indiceCubitos = 6; indiceCubitos < 18; indiceCubitos++) {
            
            }

            //* 3 caras (8 cubitos)
            for (int indiceCubitos = 18; indiceCubitos < 26; indiceCubitos++) {
            
            }

            float vertices_cubo[] = {
                // Posiciones         // Colores
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
        }

        void moveCamada(unsigned int direccion, char extra) {
            float movement = 2;

            if (direccion < 6) {
                for (int i = 0; i < 26; i++) {
                    if (direccion == 0) {
                        cubitos[i].rotacion_z(movement);
                    }
                    else if (direccion == 1) {
                        cubitos[i].rotacion_z(movement);
                    }
                    else if (direccion == 2) {
                        cubitos[i].rotacionX(movement);
                    }
                    else if (direccion == 3) {
                        cubitos[i].rotacionX(movement);
                    }
                    else if (direccion == 4) {
                        cubitos[i].rotacion_y(movement); 
                    }
                    else if (direccion == 5) {
                        cubitos[i].rotacion_y(movement);
                    }
                }
            }
        }
};

// ############################ FUNCIONES ############################
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //si presionas a muestra la estrella
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '1';
        }
    }
    //si presiones b muestra la flecha
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '2';
        }
    }
    //si presiones d muestra el circulo
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '3';
        }
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '4';
        }
    }
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '5';
        }
    }
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
    {
        if (camadas == '0') {
            camadas = '6';
        }
    }
}

int main()
{
    // --> Configuracion del GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // --> Creacion del la ventana
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ---------------------------------------
    //gladLoadGL(glfwGetProcAddress);
        // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // ---------------------------------------

    // --> Configuracion del shader
    ShaderClass shaderProgram(vertexShaderSource, fragmentShaderSource);

    // --> Creamos las figuras
    unsigned int VAO, VBO[26];
    glGenVertexArrays(1, &VAO);
    glGenBuffers(26, VBO);

    glBindVertexArray(VAO);

    Cubo Rubik(VBO); // Dentro configuramos el VBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    int location = glGetUniformLocation(shaderProgram.ID_ShaderProgram, "ourColor");
    if (location == -1) {
        printf("Error al conseguir uniform");
        exit(1);
    }
    glUniform4f(location, 1.0f, 1.0f, 10.0f, 1.0f);

    // -------------------------------------------------------------------------------------------
    shaderProgram.Activar_ShaderProgram();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.01f, 100.0f);
    shaderProgram.setMat4("projection", projection);

    glLineWidth((GLfloat)5);
    glPointSize((GLfloat)5);

    Camera camera1(glm::vec3(0.0f, 0.0f, -10.0f));

    glfwSetKeyCallback(window, keyCallback);

    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        camera1.Inputs(window);

        // ------------------------------------------------------------------------------------------
        shaderProgram.Activar_ShaderProgram();

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        view = glm::lookAt(camera1.Camara, glm::vec3(0.0f, 0.0f, 0.0f), camera1.Up);
        shaderProgram.setMat4("view", view);

        glBindVertexArray(VAO);

        for(int i = 0; i < 26; i++){
            Rubik.cubitos[i].drawCubie(shaderProgram);
        }

        //glm::mat4 model = glm::mat4(1.0f);
        //shaderProgram.setMat4("model", model);

        //glDrawArrays(GL_TRIANGLES, 0, 90);
        //drawCubies(Rubik, shaderProgram, VAO);


        //SETTING CUBES
        //set_rubik(Rubik);

        switch (camadas)
        {
        case '1':
            //Rubik.moveCamada(0,camadas2);
            break;
        case '2':
            //Rubik.moveCamada(1,camadas2);
            break;
        case '3':
            //Rubik.moveCamada(2,camadas2);
            break;
        case '4':
            //Rubik.moveCamada(3,camadas2);
            break;
        case '5':
            //Rubik.moveCamada(4,camadas2);
            break;
        case '6':
            //Rubik.moveCamada(5,camadas2);
            break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, VBO);
    shaderProgram.Eliminar_ShaderProgram();

    glfwTerminate();
    return 0;
}