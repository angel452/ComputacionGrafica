/*
Nombre: Angel Josue Loayza Huarachi
Nombre: Frank Roger Salas Ticona
Curso: Computacion Grafica
*/

#include "figure.h"
#include "shaderClass.h"

#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --> Librerias Solver
#include "../Headers/TestingFunctions.h"

using namespace std;

// ----------------- VARAIBLES ---------------------------------
#define screens 27 
auto gl_draw_number = GL_TRIANGLES;
const float PI = 3.14159265359;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// * Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

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

// * Cubo
float speed = (PI / 2) / 10;
float acumAngle = 0.0f;
Figure *cubes;
int camada = 0;

// * Solver
vector<string> scramble;
bool getSolucionActive = false;
bool isSolving = false;
bool isSolving2 = false;
vector<string> movimientos_to_solve;
int number_movments = 0;
int actual_movment = 0;

// * Movimiento de camada
bool isMoving = false;

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

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
unsigned int VAO[screens], VBO[screens], EBO[screens];

void printScramble(){
    cout << "Scramble (" << actual_movment << "): ";
    for(int i = 0; i < scramble.size(); i++){
        cout << scramble[i] << " ";
    }
    cout << endl;
}

// * Rotaciones de camada
void rotateCamada_L(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre -1.1 y -0.9, entonces es la camada atras
        if( cubes[i].verticesCentrales[0] > -1.1f && cubes[i].verticesCentrales[0] < -0.9f){
            cubes[i].rotate(speed, 'x');
        }
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            //cout << "BEFORE (Cube " << i << "): " << cubes[i].verticesCentrales[0] << ", " << cubes[i].verticesCentrales[1] << ", " << cubes[i].verticesCentrales[2] << endl; 
            cubes[i].calculateCenter();
            // cout << "AFTER (Cube " << i << "): " << cubes[i].verticesCentrales[0] << ", " << cubes[i].verticesCentrales[1] << ", " << cubes[i].verticesCentrales[2] << endl;
            //cout << endl;
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("L");
        printScramble();
    }
}
void rotateCamada_LI(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre -1.1 y -0.9, entonces es la camada atras
        if( cubes[i].verticesCentrales[0] > -1.1f && cubes[i].verticesCentrales[0] < -0.9f){
            cubes[i].rotate_inversa(speed, 'x');
        }
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("L'");
        printScramble();
    }
}
void rotateCamada_R(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre 0.9 y 1.1, entonces es la camada frente
        if(cubes[i].verticesCentrales[0] > 0.9f && cubes[i].verticesCentrales[0] < 1.1f){
            cubes[i].rotate_inversa(speed, 'x');
        }
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("R");
        printScramble();
    }
}
void rotateCamada_RI(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre 0.9 y 1.1, entonces es la camada frente
        if(cubes[i].verticesCentrales[0] > 0.9f && cubes[i].verticesCentrales[0] < 1.1f){
            cubes[i].rotate(speed, 'x');
        }
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("R'");
        printScramble();
    }
}
void rotateCamada_U(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre 0.9 y 1.1, entonces es la camada frente
        if(cubes[i].verticesCentrales[1] > 0.9f && cubes[i].verticesCentrales[1] < 1.1f){
            cubes[i].rotate_inversa(speed, 'y');
        }
    }
    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("U");
        printScramble();
    }
}
void rotateCamada_UI(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre 0.9 y 1.1, entonces es la camada frente
        if(cubes[i].verticesCentrales[1] > 0.9f && cubes[i].verticesCentrales[1] < 1.1f){
            cubes[i].rotate(speed, 'y');
        }
    }
    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("U'");
        printScramble();
    }
}
void rotateCamada_D(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre -1.1 y -0.9, entonces es la camada atras
        if( cubes[i].verticesCentrales[1] > -1.1f && cubes[i].verticesCentrales[1] < -0.9f){
            cubes[i].rotate(speed, 'y');
        }
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("D");
        printScramble();
    }
}
void rotateCamada_DI(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre -1.1 y -0.9, entonces es la camada atras
        if( cubes[i].verticesCentrales[1] > -1.1f && cubes[i].verticesCentrales[1] < -0.9f){
            cubes[i].rotate_inversa(speed, 'y');
        }
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("D'");
        printScramble();
    }
}
void rotateCamada_F(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre 0.9 y 1.1, entonces es la camada frente
        if(cubes[i].verticesCentrales[2] > 0.9f && cubes[i].verticesCentrales[2] < 1.1f){
            cubes[i].rotate_inversa(speed, 'z');
        }
    }
    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("F");
        printScramble();
    }
}
void rotateCamada_FI(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre 0.9 y 1.1, entonces es la camada frente
        if(cubes[i].verticesCentrales[2] > 0.9f && cubes[i].verticesCentrales[2] < 1.1f){
            cubes[i].rotate(speed, 'z');
        }
    }
    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("F'");
        printScramble();
    }
}
void rotateCamada_B(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre -1.1 y -0.9, entonces es la camada atras   
        if( cubes[i].verticesCentrales[2] > -1.1f && cubes[i].verticesCentrales[2] < -0.9f){
            cubes[i].rotate(speed, 'z');
        }
    }
    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("B");
        printScramble();
    }
}
void rotateCamada_BI(){
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre -1.1 y -0.9, entonces es la camada atras   
        if( cubes[i].verticesCentrales[2] > -1.1f && cubes[i].verticesCentrales[2] < -0.9f){
            cubes[i].rotate_inversa(speed, 'z');
        }
    }
    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("B'");
        printScramble();
    }
}

// * Rotaciones de camada (centros)
void rotateCamada_Centro_Z(){
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[0] > 0.9f && cubes[i].verticesCentrales[0] < 1.1f){
            cubes[i].rotate_inversa(speed, 'x');
        }
        if( cubes[i].verticesCentrales[0] > -1.1f && cubes[i].verticesCentrales[0] < -0.9f){
            cubes[i].rotate_inversa(speed, 'x');
        }
        
        
        // * Verificamos que si el centro esta entre -0.1 y 0.1, entonces es la camada central
        //if(cubes[i].verticesCentrales[0] > -0.1f && cubes[i].verticesCentrales[0] < 0.1f){
        //    cubes[i].rotate(speed, 'x');
        //}
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        scramble.push_back("R");
        scramble.push_back("L'");
        printScramble();
    }
}
void rotateCamada_Centro_ZI( ShaderClass shaderProgram ){
    for(int i = 0; i < 27; i++){
        // --> Movimiento de camada
        if(cubes[i].verticesCentrales[0] > 0.9f && cubes[i].verticesCentrales[0] < 1.1f){
            cubes[i].rotate(speed, 'x');
        }
        if( cubes[i].verticesCentrales[0] > -1.1f && cubes[i].verticesCentrales[0] < -0.9f){
            cubes[i].rotate(speed, 'x');
        }

        // * Verificamos que si el centro esta entre -0.1 y 0.1, entonces es la camada central
        //if(cubes[i].verticesCentrales[0] > -0.1f && cubes[i].verticesCentrales[0] < 0.1f){
        //    cubes[i].rotate_inversa(speed, 'x');
        //}
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;
        
        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter(); 
        }

        isMoving = false;
        scramble.push_back("R'");
        scramble.push_back("L");
        printScramble();

        // --> Movemos la camara (rotamos circularmente alrededor del cubo )
        /*
        // * Calculamos el radio entre el centro (0 0 0) y la posicion de la camara actual
        float radio = sqrt(pow(cameraPos.x, 2) + pow(cameraPos.y, 2) + pow(cameraPos.z, 2));

        // * Calculamos el angulo actual de la camara respecto al centro
        float angle = atan2(cameraPos.z, cameraPos.x);

        // * Movemos 45 grados en el eje x a la posicion actual de la camara
        float angle2 = 45;

        float new_x = radio * cos(angle - angle2);
        float new_y = radio * sin(angle - angle2);
        float new_z = cameraPos.z;
        
        // * Movemos 45 grados en el eje y a la posicion actual de la camara
        //angle += (PI / 4);
        //float new_x = radio * cos(angle);
        //float new_y = cameraPos.y;
        //float new_z = radio * sin(angle);

        // * Actualizamos la posicion de la camara
        cameraPos.x = new_x;
        cameraPos.y = new_y;
        cameraPos.z = new_z;

        cameraFront = glm::vec3(0.0f, 0.0f, 0.0f) - cameraPos;

        // * Miramos al centro con el glm::lookAt
        glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
        shaderProgram.setMat4("view", view);
        */
    }
}
void rotateCamada_Centro_Y(){
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[1] > 0.9f && cubes[i].verticesCentrales[1] < 1.1f){
            cubes[i].rotate_inversa(speed, 'y');
        }
        if( cubes[i].verticesCentrales[1] > -1.1f && cubes[i].verticesCentrales[1] < -0.9f){
            cubes[i].rotate_inversa(speed, 'y');
        }

        // * Verificamos que si el centro esta entre -0.1 y 0.1, entonces es la camada central
        //if(cubes[i].verticesCentrales[1] > -0.1f && cubes[i].verticesCentrales[1] < 0.1f){
        //    cubes[i].rotate(speed, 'y');
        //}
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        scramble.push_back("U");
        scramble.push_back("D'");
        printScramble();
    }
}
void rotateCamada_Centro_YI(){
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[1] > 0.9f && cubes[i].verticesCentrales[1] < 1.1f){
            cubes[i].rotate(speed, 'y');
        }
        if( cubes[i].verticesCentrales[1] > -1.1f && cubes[i].verticesCentrales[1] < -0.9f){
            cubes[i].rotate(speed, 'y');
        }

        // * Verificamos que si el centro esta entre -0.1 y 0.1, entonces es la camada central
        //if(cubes[i].verticesCentrales[1] > -0.1f && cubes[i].verticesCentrales[1] < 0.1f){
        //    cubes[i].rotate_inversa(speed, 'y');
        //}
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;
        
        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter(); 
        }

        isMoving = false;
        scramble.push_back("U'");
        scramble.push_back("D");
        printScramble();
    }
}
void rotateCamada_Centro_X(){
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[2] > 0.9f && cubes[i].verticesCentrales[2] < 1.1f){
            cubes[i].rotate_inversa(speed, 'z');
        }
        if( cubes[i].verticesCentrales[2] > -1.1f && cubes[i].verticesCentrales[2] < -0.9f){
            cubes[i].rotate_inversa(speed, 'z');
        }
        
        // * Verificamos que si el centro esta entre -0.1 y 0.1, entonces es la camada central
        //if(cubes[i].verticesCentrales[2] > -0.1f && cubes[i].verticesCentrales[2] < 0.1f){
        //    cubes[i].rotate(speed, 'z');
        //}
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        scramble.push_back("F");
        scramble.push_back("B'");
        printScramble();
    }
}
void rotateCamada_Centro_XI(){
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[2] > 0.9f && cubes[i].verticesCentrales[2] < 1.1f){
            cubes[i].rotate(speed, 'z');
        }
        if( cubes[i].verticesCentrales[2] > -1.1f && cubes[i].verticesCentrales[2] < -0.9f){
            cubes[i].rotate(speed, 'z');
        }

        // * Verificamos que si el centro esta entre -0.1 y 0.1, entonces es la camada central
        //if(cubes[i].verticesCentrales[2] > -0.1f && cubes[i].verticesCentrales[2] < 0.1f){
        //    cubes[i].rotate_inversa(speed, 'z');
        //}
    }

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;
        
        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter(); 
        }

        isMoving = false;
        scramble.push_back("F'");
        scramble.push_back("B");
        printScramble();
    }
}

void rotateCamada(ShaderClass shaderProgram){
    switch (camada){
        case 1:                         // Vertical (derecha)
            rotateCamada_R();
            break;

        case 2:                         // Vertical (centro)
            rotateCamada_Centro_ZI(shaderProgram);
            break;

        case 3:                         // Vertical (izquierda)
            rotateCamada_LI();
            break;

        case 4:                         // Vertical (derecha - Inversa)
            rotateCamada_RI();
            break;

        case 5:                         // Vertical (centro - Inversa)
            rotateCamada_Centro_Z();
            break;

        case 6:                         // Vertical (izquierda - Inversa)      
            rotateCamada_L();
            break;

        case 7:                         // Horizontal (arriba)
            rotateCamada_D();
            break;
            
        case 8:                         // Horizontal (centro)
            rotateCamada_Centro_Y();
            break;

        case 9:                         // Horizontal (abajo)
            rotateCamada_UI();
            break;
        
        case 10:                        // Horizontal (arriba - Inversa)
            rotateCamada_DI();
            break;
        
        case 11:                        // Horizontal (centro - Inversa)
            rotateCamada_Centro_YI();
            break;
        
        case 12:                        // Horizontal (abajo - Inversa)
            rotateCamada_U();
            break;
    }
}

// * Solver Functions
string convertMovments_toString(){
    string movments = "";
    for(int i = 0; i < scramble.size(); i++){
        movments += scramble[i];
    }
    return movments;
}

vector<string> convert_String_to_Movments(string movments, int N_movments){
    vector<string> movimientos_to_solve;

    string movimientoActual;
    for(int i = 1; i < movments.length(); i++){
        // * Verificamos si el siguiente caracter es un '
        if(movments[i+1] == '\''){
            movimientoActual = movments[i];
            movimientoActual += movments[i+1];
            movimientos_to_solve.push_back(movimientoActual);
            i++;
        }
        // * Verificamos si el siguiente caracter es un 2 (duplicamos el movimiento)
        else if(movments[i+1] == '2'){
            movimientoActual = movments[i];
            movimientos_to_solve.push_back(movimientoActual);
            movimientos_to_solve.push_back(movimientoActual);
            i++;
        }
        // * Verificamos si el caracter actual es un espacio (no hacemos nada)
        else if(movments[i] == ' '){
            continue;
        }
        // * Si no es ninguno de los anteriores, entonces es un movimiento normal
        else{
            movimientoActual = movments[i];
            movimientos_to_solve.push_back(movimientoActual);
        }
    }
    return movimientos_to_solve;
}

vector<string> convert_String_to_MovmentsV2(string movments){
    vector<string> movimientos_to_solve;

    // --> Preprocesado del string
    // * Eliminamos los espacios vacios
    for(int i = 0; i < movments.length(); i++){
        if(movments[i] == ' '){
            movments.erase(i, 1);
        }
    }
    cout << "movments: " << movments << endl;

    // * Filtramos por movimientos
    string movimientoActual;
    for(int i = 0; i < movments.length(); i++){
        // * Verificamos si el siguiente caracter es un '
        if(movments[i+1] == '\''){
            movimientoActual = movments[i];
            movimientoActual += movments[i+1]; // * Agregamos el '
            movimientos_to_solve.push_back(movimientoActual);
            i++;
        }
        // * Verificamos si el siguiente caracter es un 2 (duplicamos el movimiento)
        else if(movments[i+1] == '2'){
            movimientoActual = movments[i];
            movimientos_to_solve.push_back(movimientoActual);
            movimientos_to_solve.push_back(movimientoActual);
            i++;
        }
        // * Si no es ninguno de los anteriores, entonces es un movimiento normal
        else{
            movimientoActual = movments[i];
            movimientos_to_solve.push_back(movimientoActual);
        }

        // * Verificamos si ya es el final del string
        if(i == movments.length() - 1){
            cout << "Fin del string!!!!"  <<  endl;
            break;
        }
    }
    return movimientos_to_solve;
}

void getSolution(){
    // --> Convertimos el scramble a un string
    string scramble_Aux;
    scramble_Aux = convertMovments_toString();

    string solve;
    solve = testFromCube(scramble_Aux);

    // --> Convertimos el resultado a un vector de strings
    movimientos_to_solve = convert_String_to_MovmentsV2(solve);

    // --> Imprimimos los movimientos
    cout << "------------------------------ INFO SOLUCION ------------------------------" << endl;
    number_movments = movimientos_to_solve.size();
    cout << "Numero de movimientos: " << number_movments << endl << endl;

    cout << "Movimientos: ";
    for(int i = 0; i < movimientos_to_solve.size(); i++){
        cout << movimientos_to_solve[i];
    }
    cout << endl;
    cout << "-------------------------------------------------------------------------" << endl;
}

void moveSolution(){
    if(movimientos_to_solve.size() == 0){
        isSolving = false;
        isSolving2 = false;
        getSolucionActive = false;
        return;
    }
    else if(actual_movment == number_movments){
        isSolving = false;
        isSolving2 = false;
        getSolucionActive = false;
        return;
    }

    string movimientoActual = movimientos_to_solve[actual_movment];

    if(movimientoActual == "L"){
        rotateCamada_L();
    }
    else if(movimientoActual == "L'"){
        rotateCamada_LI();
    }
    else if(movimientoActual == "R"){
        rotateCamada_R();
    }
    else if(movimientoActual == "R'"){
        rotateCamada_RI();
    }
    else if(movimientoActual == "U"){
        rotateCamada_U();
    }
    else if(movimientoActual == "U'"){
        rotateCamada_UI();
    }
    else if(movimientoActual == "D"){
        rotateCamada_D();
    }
    else if(movimientoActual == "D'"){
        rotateCamada_DI();
    }
    else if(movimientoActual == "F"){
        rotateCamada_F();
    }
    else if(movimientoActual == "F'"){
        rotateCamada_FI();
    }
    else if(movimientoActual == "B"){
        rotateCamada_B();
    }
    else if(movimientoActual == "B'"){
        rotateCamada_BI();
    }
}

int main()
{
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

    glGenVertexArrays(screens, VAO);
    glGenBuffers(screens, VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    // FIGURE
    cubes = new Figure[27];

    int count = 0;
    for (int z = -1; z < 2; z += 1) {
        for (int y = -1; y < 2; y += 1) {
            for (int x = -1; x < 2; x += 1) {
                //printf("cube: %d \tx: %d, y: %d, z: %d\n", count, x, y, z);
                cubes[count++].move(x, y, z);
            }
        }
    } 

    int cantidadCubos = 27;
    std::cout << "sz cubes initial: " << cantidadCubos << std::endl;


    // --> Re-Move Cubes manually for better visualization (sepation between cubes)
    /*
    // * Cara trasera (fila 1)
    for(int i = 0; i < 3; i++){
        cubes[i].move(0.00f, 0.00f, -0.04f);
    }
    // * Cara trasera (fila 2)
    for(int i = 3; i < 6; i++){
        cubes[i].move(0.00f, 0.02f, -0.04f);
    }
    // * Cara trasera (fila 3)
    for(int i = 6; i < 9; i++){
        cubes[i].move(0.00f, 0.04f, -0.04f);
    }

    // * Cara central (fila 1)
    for(int i = 9; i < 12; i++){
        cubes[i].move(0.00f, 0.00f, -0.02f);
    }
    // * Cara central (fila 2)
    for(int i = 12; i < 15; i++){
        cubes[i].move(0.00f, 0.02f, -0.02f);
    }
    // * Cara central (fila 3)
    for(int i = 15; i < 18; i++){
        cubes[i].move(0.00f, 0.04f, -0.02f);
    }

    // * Cara frontal (fila 1)
    for(int i = 18; i < 21; i++){
        cubes[i].move(0.00f, 0.00f, 0.00f);
    }
    // * Cara frontal (fila 2)
    for(int i = 21; i < 24; i++){
        cubes[i].move(0.00f, 0.02f, 0.00f);
    }
    // * Cara frontal (fila 3)
    for(int i = 24; i < 27; i++){
        cubes[i].move(0.00f, 0.04f, 0.00f);
    }
    */

    // * Calculamos el centro de cada cubo
    for(int i = 0; i < cantidadCubos; i++){
        cubes[i].calculateCenter();
    }


    for (int i = 0; i < cantidadCubos; i++) {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, cubes[i].sizeVertexes * sizeof(float), cubes[i].vertexes, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0); 
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    shaderProgram.Activar_ShaderProgram();

    glLineWidth(10);
    glPointSize(10);

    cout << "------------------------------ INFO CENTROS ------------------------------" << endl;
    for(int i = 0; i < 27; i++){
        cout << "Cubo N° " << i << ": ";
        cout << cubes[i].verticesCentrales[0] << ", " << cubes[i].verticesCentrales[1] << ", " << cubes[i].verticesCentrales[2] << endl;
    }
    cout << "-------------------------------------------------------------------------" << endl;
    
    while (!glfwWindowShouldClose(window)) { 
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activar_ShaderProgram();

        // --> Matriz de transformacion
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaderProgram.setMat4("projection", projection);

        // --> View/cam Transformation
        //glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp); // --> Camara en el origen
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shaderProgram.setMat4("view", view);

        // print cameraFront
        //cout << "cameraFront: " << cameraFront.x << ", " << cameraFront.y << ", " << cameraFront.z << endl;

        // --> Verificacion de movimiento de camada
        if(camada >= 1 && camada <= 12){
            isMoving = true;
            rotateCamada(shaderProgram);
        }
        
        if(getSolucionActive == true && isSolving == false){
            getSolution();
            isSolving = true;
            actual_movment = 0;
            scramble.clear();
        }

        if(getSolucionActive == true && isSolving == true && isSolving2 == false){
            moveSolution();
            //isSolving2 = true;
        }

        for(unsigned int i = 0; i < cantidadCubos; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubes[i].center);
            shaderProgram.setMat4("model", model);

            glBindVertexArray(VAO[i]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
            glBufferData(GL_ARRAY_BUFFER, cubes[i].sizeVertexes * sizeof(float), cubes[i].vertexes, GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(screens, VAO);
    glDeleteBuffers(screens, VBO);
    glDeleteBuffers(screens, EBO);
    //glDeleteProgram(shaderProgram);

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
    float cameraSpeed = 0.1f; // --> Velocidad de movimiento de la cámara
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

    // MOVIMIENTO DE CAMADA 12 movimientos: 
    //                                      6 verticales (3 horario - 3 antihorario)
    //                                      6 horizontales (3 horario - 3 antihorario)
    if(isMoving == false){
        if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // Vertical (derecha)
            camada = 1;
        if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // Vertical (centro)
            camada = 2;
        if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // Vertical (izquierda)
            camada = 3;
        if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) // Vertical (derecha - inversa)
            camada = 4;
        if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) // Vertical (centro - inversa)
            camada = 5;
        if(glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) // Vertical (izquierda - inversa)
            camada = 6;
        if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // Horizontal (arriba)
            camada = 7;
        if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // Horizontal (centro)
            camada = 8;
        if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) // Horizontal (abajo)
            camada = 9;
        if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) // Horizontal (arriba - inversa)
            camada = 10;
        if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) // Horizontal (centro - inversa)
            camada = 11;
        if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) // Horizontal (abajo - inversa)
            camada = 12;
    }


    // Solver
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        getSolucionActive = true;
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
