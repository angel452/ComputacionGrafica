/*
Nombre: Angel Josue Loayza Huarachi
Curso: Computacion Grafica
*/

// ################################# LIBRERIAS ###################################

//#define GLAD_GL_IMPLEMENTATION
//#include <glad/gl.h>
//#define GLFW_INCLUDE_NONE
#include "glad.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "linmath.h"
#include "deps/linmath.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <iostream>
#include <string>

#include <filesystem>

#include <fstream>
#include <sstream>
#include <cerrno>

//#include <windows.h> 
#include <unistd.h>

#include <vector>
#include <math.h>

#include <cmath>
#include <string.h>

//This is something that stb image defines in its own include guard, so I should be able to test for it
#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

using namespace std;
//namespace fs = std::filesystem;

// ############################# VARIABLES GLOBALES ##############################
// * Ventana
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 900;

// * Primitiva
int currentDrawMode = GL_TRIANGLES;

// * Shaders
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
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

// * Mouse
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float yaw = -90.0f; // --> Iniciar mirando hacia la izquierda
float pitch = 0.0f;
// * FOV: Campo de vision de la camara (Normalmente 45 grados)
float fov = 45.0f;

// * Camara 
// - Sistema de coordenadas para la camara (posicion, hacia donde mira, hacia donde esta arriba )
// - Se usa para crear la matriz de vista - LookAt
glm::vec3 cameraPos = glm::vec3(1.0f, 1.0f, 7.0f); // --> Vector - Posicion de la camara
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // --> Vector - Hacia donde mira la camara
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // --> Hacia donde esta arriba la camara

// * Tiempo
// - Para la velocidad de movimiento de la camara
float deltaTime = 0.0f; // --> Tiempo entre el frame actual y el anterior
float lastFrame = 0.0f; // --> Tiempo del ultimo frame

// ############################## CLASE SHADER ###################################
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

// ############################# CLASE CUBO #####################################
class Cubo {
    public:
    float center[3];
    float* vertexes;
    float* centeredVertexes;
    unsigned int* indexes;
    int sizeVertexes;
    int sizeIndexes;
    int scaleAmount;
    int scaleDirection;

    Cubo(float*, int); // Vertices
    Cubo(float*, int, unsigned int*, int); // Vertices e indices
    void toCenter();
    void scale(float);
    void rotate(float, char);
    void move(float, float, float);
};

// ################################# FUNCIONES ###################################
// --> Configurar la pantalla
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
     (0, 0, width, height);
}

// --> Procesar la entrada del teclado
void processInput(GLFWwindow *window){
    // --> Cambiar primitiva
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        currentDrawMode = GL_POINTS;

    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        currentDrawMode = GL_LINE_STRIP;

    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        currentDrawMode = GL_TRIANGLES;
    
    float cameraSpeed = static_cast<float>(2.5f * deltaTime); // --> Velocidad de movimiento de la camara
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos = cameraPos + cameraSpeed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos = cameraPos - cameraSpeed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos = cameraPos - glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos = cameraPos + glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// --> Procesar la entrada del mouse
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX; // --> Posición en x del mouse
    float yoffset = lastY - ypos; // --> Posición en y del mouse
    lastX = xpos;
    lastY = ypos;

    // --> Sensibilidad del mouse
    float sensitivity = 0.1f;
    xoffset = xoffset * sensitivity;
    yoffset = yoffset * sensitivity;

    // --> Angulo de rotación
    yaw = yaw + xoffset;
    pitch = pitch + yoffset;

    // --> Limitar el movimiento de la cámara
    if(pitch > 89.0f){
        pitch = 89.0f;
    }
    if(pitch < -89.0f){
        pitch = -89.0f;
    }

    // --> Cálculo de la dirección de la cámara
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); // --> Cálculo de la dirección en x
    front.y = sin(glm::radians(pitch)); // --> Cálculo de la dirección en y
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)); // --> Cálculo de la dirección en z
    cameraFront = glm::normalize(front);
}

// --> Procesar el scroll del mouse
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    fov = fov - (float)yoffset;
    if(fov < 1.0f){
        fov = 1.0f;
    }
    if(fov > 45.0f){
        fov = 45.0f;
    }
}

// ################################ FIGURAS #################################
// * Cubo
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

// * Posiciones de todos los cubos en el espacio
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 1.02f,  0.0f,  0.0f),
    glm::vec3( 2.04f,  0.0f,  0.0f),

    glm::vec3( 0.0f,  1.02f,  0.0f),
    glm::vec3( 1.02f,  1.02f,  0.0f),
    glm::vec3( 2.04f,  1.02f,  0.0f),

    glm::vec3( 0.0f,  2.04f,  0.0f),
    glm::vec3( 1.02f,  2.04f,  0.0f),
    glm::vec3( 2.04f,  2.04f,  0.0f),

    glm::vec3( 0.0f,  0.0f,  1.02f),
    glm::vec3( 1.02f,  0.0f,  1.02f),
    glm::vec3( 2.04f,  0.0f,  1.02f),

    glm::vec3( 0.0f,  1.02f,  1.02f),
    glm::vec3( 1.02f,  1.02f,  1.02f),
    glm::vec3( 2.04f,  1.02f,  1.02f),

    glm::vec3( 0.0f,  2.04f,  1.02f),
    glm::vec3( 1.02f,  2.04f,  1.02f),
    glm::vec3( 2.04f,  2.04f,  1.02f),

    glm::vec3( 0.0f,  0.0f,  2.04f),
    glm::vec3( 1.02f,  0.0f,  2.04f),
    glm::vec3( 2.04f,  0.0f,  2.04f),

    glm::vec3( 0.0f,  1.02f,  2.04f),
    glm::vec3( 1.02f,  1.02f,  2.04f),
    glm::vec3( 2.04f,  1.02f,  2.04f),

    glm::vec3( 0.0f,  2.04f,  2.04f),
    glm::vec3( 1.02f,  2.04f,  2.04f),
    glm::vec3( 2.04f,  2.04f,  2.04f),
};

// ########################## FUNCION MAIN ###################################
int main()
{
    // --> Configuración de GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // --> Creación de la ventana
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // --> Capturar el cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    glEnable(GL_DEPTH_TEST);

    // --> Configuracion de los shaders
    ShaderClass shaderProgram(vertexShaderSource, fragmentShaderSource);

    // --> Creamos las figuras
    // *CUBO
    Cubo cubeRubick(vertices_cubo, sizeof(vertices_cubo) / sizeof(float));

    unsigned int VBO_Cubo, VAO_Cubo;
    glGenVertexArrays(1, &VAO_Cubo);
    glGenBuffers(1, &VBO_Cubo);

    glBindVertexArray(VAO_Cubo);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cubo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cubo), vertices_cubo, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, cubeRubick.sizeVertexes * sizeof(float), cubeRubick.vertexes, GL_STATIC_DRAW);

    // Posiciones de los vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Coordenadas de textura
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Tell OpenGL for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    shaderProgram.Activar_ShaderProgram();

    // --> MAIN LOGIC
    glPointSize(4.0f);
    glLineWidth(3.0f);

    while (!glfwWindowShouldClose(window)){
        // --> Logica del tiempo por frame
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --> Inputs de teclado
        processInput(window);

        // --> Renderizado
        // * Color de fondo: Plomo Oscuro
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // * Limpiamos el buffer del color y el buffer de profundidad antes de volver a renderizar
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activar_ShaderProgram();

        // * MATRIZ DE PROYECCION
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                                // Perspective( * Campo de vision, 
                                //              * Relacion de aspecto (ancho / altura), 
                                //              * Plano mas cercano, 
                                //              * Plano mas lejano )

        shaderProgram.setMat4("projection", projection);

        // * MATRIZ DE VISTA
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
                        // LookAt( * Posicion de la camara (cameraPos), 
                        //         * Hacia donde mira la camara (cameraFront), 
                        //         * Hacia donde esta arriba la camara (cameraUp) 
                        //        )

        shaderProgram.setMat4("view", view);

        // render boxes
        glBindVertexArray(VAO_Cubo);

        int cantidadCubos = sizeof(cubePositions) / sizeof(cubePositions[0]);
        for(unsigned int i = 0; i < cantidadCubos; i++)
        {
            // * MATRIZ DE MODELO
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
                    
            shaderProgram.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --> Eliminación de recursos
    glDeleteVertexArrays(1, &VAO_Cubo);
    glDeleteBuffers(1, &VBO_Cubo);

    // --> Terminar proceso
    glfwTerminate();
    return 0;
}

Cubo::Cubo(float* initVertexes, int szVertexes):
        sizeVertexes(szVertexes), scaleAmount(0), scaleDirection(1)
{
    vertexes = new float[szVertexes];
    centeredVertexes = new float[szVertexes];
    for (int i = 0; i < szVertexes; i++) {
        vertexes[i] = initVertexes[i];
        centeredVertexes[i] = initVertexes[i];
    };

    center[0] = 0;
    center[1] = 0.0f;
    center[2] = 0.0f;
}

Cubo::Cubo(float* initVertexes, int szVertexes, unsigned int* initIndexes, int szIndexes):
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

    center[0] = 0;
    center[1] = 0.0f;
    center[2] = 0.0f;
}

void Cubo::toCenter()
{
    float mCenter[3];
    float aux[3];
    for (int i = 0; i < sizeVertexes; i += 3) {
        mCenter[0] += vertexes[i];
        mCenter[1] += vertexes[i + 1];
        mCenter[2] += vertexes[i + 2];
    }

    mCenter[0] /= (float)sizeVertexes / 3;
    mCenter[1] /= (float)sizeVertexes / 3;
    mCenter[2] /= (float)sizeVertexes / 3;

    aux[0] = center[0] - mCenter[0];
    aux[1] = center[1] - mCenter[1];
    aux[2] = center[2] - mCenter[2];

    for (int i = 0; i < sizeVertexes; i += 3) {
        vertexes[i] = vertexes[i] - aux[0];
        vertexes[i + 1] = vertexes[i + 1] - aux[1];
        vertexes[i + 2] = vertexes[i + 2] - aux[2];
    }
}

void Cubo::scale(float scaleFactor)
{
    float scalingMatrix[16] = {
        scaleFactor, 0.0f, 0.0f, 0.0f,
        0.0f, scaleFactor, 0.0f, 0.0f,
        0.0f, 0.0f, scaleFactor, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    float* originalVertexes = new float[sizeVertexes];
    memcpy(originalVertexes, centeredVertexes, sizeVertexes * sizeof(float));

    for (int i = 0; i < sizeVertexes; i += 3) {
        float originalVertex[4] = {originalVertexes[i], originalVertexes[i + 1], originalVertexes[i + 2], 1.0f};
            
        float scaledVertex[4];
        for (int j = 0; j < 4; j++) {
            scaledVertex[j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                scaledVertex[j] += scalingMatrix[j * 4 + k] * originalVertex[k];
            }
        }
        centeredVertexes[i] = scaledVertex[0];
        centeredVertexes[i + 1] = scaledVertex[1];
        centeredVertexes[i + 2] = scaledVertex[2];

        vertexes[i] = scaledVertex[0] + center[0];
        vertexes[i + 1] = scaledVertex[1] + center[1];
        vertexes[i + 2] = scaledVertex[2] + center[2];
    }

    delete[] originalVertexes;
}

void Cubo::rotate(float angle, char type) 
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

    float* originalVertexes = new float[sizeVertexes];
    memcpy(originalVertexes, centeredVertexes, sizeVertexes * sizeof(float));

    for (int i = 0; i < sizeVertexes; i += 3) {
        float originalVertex[4] = {originalVertexes[i], originalVertexes[i + 1], originalVertexes[i + 2], 1.0f};
            
        float rotatedVertex[4];
        for (int j = 0; j < 4; j++) {
            rotatedVertex[j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                rotatedVertex[j] += rotationMatrix[j * 4 + k] * originalVertex[k];
            }
        }
        centeredVertexes[i] = rotatedVertex[0];
        centeredVertexes[i + 1] = rotatedVertex[1];
        centeredVertexes[i + 2] = rotatedVertex[2];

        vertexes[i] = rotatedVertex[0] + center[0];
        vertexes[i + 1] = rotatedVertex[1] + center[1];
        vertexes[i + 2] = rotatedVertex[2] + center[2];
    }

    
    delete[] originalVertexes;
}

void Cubo::move(float xoff, float yoff, float zoff) {
    float translationMatrix[16] = {
        1.0f, 0.0f, 0.0f, xoff,
        0.0f, 1.0f, 0.0f, yoff,
        0.0f, 0.0f, 1.0f, zoff,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    center[0] += xoff;
    center[1] += yoff;
    center[2] += zoff;

    float* originalVertexes = new float[sizeVertexes];
    memcpy(originalVertexes, centeredVertexes, sizeVertexes * sizeof(float));

    for (int i = 0; i < sizeVertexes; i += 3) {
        float originalVertex[4] = {originalVertexes[i], originalVertexes[i + 1], originalVertexes[i + 2], 1.0f};
            
        float movedVertex[4];
        for (int j = 0; j < 4; j++) {
            movedVertex[j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                movedVertex[j] += translationMatrix[j * 4 + k] * originalVertex[k];
            }
        }

        vertexes[i] = movedVertex[0];
        vertexes[i + 1] = movedVertex[1];
        vertexes[i + 2] = movedVertex[2];
    }

    
    delete[] originalVertexes;
}