/*
Nombre: Angel Josue Loayza Huarachi
Nombre: Frank Roger Salas Ticona
Curso: Computacion Grafica
*/

#include "dRays.h"
#include "cubes.h"
#include <GLFW/glfw3.h>

#define cantRubick 100

const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPosition;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 fragColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPosition, 1.0f);\n"
    "   fragColor = aColor;\n"
    "}\n";

const char *fragmentShaderSource = "#version 330 core\n"
    "in vec3 fragColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(fragColor, 1.0f);\n"
    "}\n";

const char *vertexShaderSource1 = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource1 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
    "}\n\0";
// * Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// * Mouse
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float yaw = -90.0f; // --> Iniciar mirando hacia la izquierda
float pitch = 0.0f;
float fov = 45.0f;

// * Camara
glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 13.0f); // --> Posicion de la camara
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // --> Hacia donde mira la camara
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // --> Hacia donde esta arriba la camara

// * Rubicks
bool enemiesMoves = false;
std::vector<deathRay> dRays;
std::vector<cube> mcube(cantRubick);
bool aliveCubes[cantRubick];

int 
main()
{
    //mcube.reserve(cantRubick);
    for (int i = 0; i < cantRubick; i++) {
    //    mcube.emplace_back();
        aliveCubes[i] = true;
    }

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef _APPLE_
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Homework", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    /*
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    */
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // ---------------------------------------

    glEnable(GL_DEPTH_TEST);

    //unsigned int shaderProgram = createVertexShader(vertexShaderSource, fragmentShaderSource);
    ShaderClass shaderProgram(vertexShaderSource, fragmentShaderSource);
    ShaderClass lineProgram(vertexShaderSource1, fragmentShaderSource1);

    // FIGURE
    for (int i = 0; i < mcube.size(); i++) {
        mcube[i].renderInit();
        for (int j = 0; j < mcube[i].cantidadCubos; j++) {
            for (int k = 0; k < mcube[i].cubes[j].sizeVertexes; k++) {
                mcube[i].originalRubickVertexes[j][k] = mcube[i].cubes[j].vertexes[k];
            }
        }
    }

    shaderProgram.Activar_ShaderProgram();


    for (int i = 0; i < mcube.size(); i++) {
        mcube[i].infoCentros();
    }

    for (int i = 0; i < mcube.size(); i++) {
        printf("Rubick %d: \n", i);
        for (int j = 0; j < mcube[i].cantidadCubos; j++) {
            printf("Cube %d: \n", j);
            mcube[i].cubes[j].printVertexes();
        }
    }

    float mira1Vertex[] = {
       -0.1f, 0.0f, 0.0f,
        0.1f, 0.0f, 0.0f,
    };
    float mira2Vertex[] = {
        0.0f, -0.1f, 0.0f,
        0.0f, 0.1f, 0.0f,
    };
    unsigned int VAOmira1, VBOmira1;
    unsigned int VAOmira2, VBOmira2;
        
    glGenVertexArrays(1, &VAOmira1);
    glGenBuffers(1, &VBOmira1);

    glBindVertexArray(VAOmira1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOmira1);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), mira1Vertex, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &VAOmira2);
    glGenBuffers(1, &VBOmira2);

    glBindVertexArray(VAOmira2);
    glBindBuffer(GL_ARRAY_BUFFER, VBOmira2);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), mira1Vertex, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window)) { 
        processInput(window);

        lineProgram.Activar_ShaderProgram();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(3);
        glPointSize(3);
        glBindVertexArray(VAOmira1);
        glBindBuffer(GL_ARRAY_BUFFER, VBOmira1);
        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), mira1Vertex, GL_STATIC_DRAW);
        glDrawArrays(GL_LINES, 0, 12); 

        glBindVertexArray(VAOmira2);
        glBindBuffer(GL_ARRAY_BUFFER, VBOmira2);
        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), mira2Vertex, GL_STATIC_DRAW);
        glDrawArrays(GL_LINES, 0, 12); 

        glLineWidth(10);
        glPointSize(10);
        shaderProgram.Activar_ShaderProgram();

        if (enemiesMoves == true) {
            for (int i = 0; i < cantRubick; i++) {
                if (aliveCubes[i] == true)
                    mcube[i].randMove();
            }
        }
        
        for (int i = 0; i < dRays.size(); i++) {
            dRays[i].updateStep();
        }

        for (int j = 0; j < cantRubick; j++) {
            if (aliveCubes[j] == true) {
                for (int i = 0; i < dRays.size(); i++) {
                    glm::vec3 tcenter = dRays[i].getCenter();
                    printf("tcenter: %f, %f, %f\n", tcenter[0], tcenter[1], tcenter[2]);
                    printf("cube %d center: %f, %f, %f\n", j, mcube[j].cubes[13].verticesCentrales[0], mcube[j].cubes[13].verticesCentrales[1], mcube[j].cubes[13].verticesCentrales[2]);
                    if (mcube[j].isColliding(tcenter.x, tcenter.y, tcenter.z)) {
                        printf("COLISION DETECTED!\n");
                        aliveCubes[j] = false;
                    }
                }
            }
        }


        // --> Matriz de transformacion
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaderProgram.setMat4("projection", projection);

        // --> View/cam Transformation
        //glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp); // --> Camara en el origen
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shaderProgram.setMat4("view", view);

        ///
        for (int i = 0; i < cantRubick; i++) {
            if (aliveCubes[i] == true)
                mcube[i].render(shaderProgram);
        }

        for (int i = 0; i < dRays.size(); i++) {
            dRays[i].render(lineProgram);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // EXIT
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // MOVIMIENTO DE CAMARA
    float cameraSpeed = 0.2f; // --> Velocidad de movimiento de la cámara
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos = cameraPos + cameraSpeed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos = cameraPos - cameraSpeed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos = cameraPos - glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos = cameraPos + glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos = cameraPos + cameraSpeed * cameraUp;
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos = cameraPos - cameraSpeed * cameraUp;
    if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        enemiesMoves = enemiesMoves ? false : true;

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (dRays.size() < 100) {
            printf("Death Ray Spawned!\n");
            dRays.push_back(deathRay(cameraPos, cameraFront));
        }
    }


    // MOVIMIENTO DE CAMADA
    if(mcube[0].isMoving == false){
        if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // * Derecha - Izquierda       (L )
            mcube[0].camada = 1;
        if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // * Derecha - Izquierda
            mcube[0].camada = 2;
        if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // * Derecha - Izquierda       (R')
            mcube[0].camada = 3;
        if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) // * Arriba - Abajo            (D )
            mcube[0].camada = 4;
        if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) // * Arriba - Abajo
            mcube[0].camada = 5;
        if(glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) // * Arriba - Abajo            (U')
            mcube[0].camada = 6;
        if(glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) // * Cerca - Lejos             (B )
            mcube[0].camada = 7;
        if(glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) // * Cerca - Lejos
            mcube[0].camada = 8;
        if(glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) // * Cerca - Lejos             (F')
            mcube[0].camada = 9;
    }


    // Solver
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        mcube[0].getSolucionActive = true;
    }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse){
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
    if (pitch > 89.0f){
        pitch = 89.0f;
    }
    if (pitch < -89.0f){
        pitch = -89.0f;
    }

    // --> Cálculo de la dirección de la cámara
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); // --> Cálculo de la dirección en x
    front.y = sin(glm::radians(pitch)); // --> Cálculo de la dirección en y
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)); // --> Cálculo de la dirección en z
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    fov = fov - (float)yoffset;
    if (fov < 1.0f){
        fov = 1.0f;
    }
    if (fov > 45.0f){
        fov = 45.0f;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
