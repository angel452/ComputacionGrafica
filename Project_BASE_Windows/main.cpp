/*
Nombre: Angel Josue Loayza Huarachi
Curso: Computacion Grafica

Matriz de transformaciones: Ry-1  Tx-1  Sx
    - Rotacion = 45
    - Traslacion = 1
    - Escala = 1.5

Vertices:
    -0.5    0.5  0.0 
    -0.5   -0.5  0.5
      0     0     0
      1     1     1    
*/

// ################################# LIBRERIAS ###################################

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <iostream>
#include <string>

#include <filesystem>

#include <fstream>
#include <sstream>
#include <cerrno>

#include <windows.h> 
#include <unistd.h>

#include <vector>
#include <math.h>

//#include "clases.cpp"
//#include "VarGlobales.cpp"

using namespace std;

// ############################# VARIABLES GLOBALES ##############################

//--> Estructura de como se van a mover los objetos ( FORMULA (nMatrices*4)-4 )
//string informacion[9] = {
//                            "rotar"    , "angulo"       , "eje x/y/z"    , "null",
//                            "escalar"  , "cantidad en x", "cantidad en y", "cantidad en z",
//                            "trasladar", "cantidad en x", "cantidad en y", "cantidad en z"};

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 900;
int currentDrawMode = GL_TRIANGLES;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);\n"
    "}\n\0";

float radio = 0.08, pos_x, pos_y;
int nTriangles = 10;
float angle = (2*3.141516) / nTriangles;

bool start_estrella = false;
bool start_circulo = false;
bool start_casa = false;
bool start_Triangulo = false;

bool flagFixPosition_C = false; // --> Para la casa
bool flagFixPosition_E = false; // --> Para la estrella
bool flagFixPosition_Cir = false; // --> Para el circulo

// --> Para las inversas (Guardamos todas las transformaciones, para luego hacer la inversa)
bool start_estrella_inv = false;
bool start_circulo_inv = false;
bool start_casa_inv = false;
bool star_casa_inv2 = false;

vector<vector<string>> hist_trans_C;
vector<vector<string>> hist_trans_E;
vector<vector<string>> hist_trans_Cir;

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
};

// ############################ 2. CLASE VBO ########################################
class VBOClass{
    public:
        unsigned int ID_VBO;

        // --> Pasamos un vector como parámetro
        VBOClass( vector<float> vertices ){
            glGenBuffers(1, &ID_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, ID_VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data() , GL_STATIC_DRAW);
        }

        // --> Pasamos un array como parámetro
        VBOClass( float* vertices, unsigned int size ){
            glGenBuffers(1, &ID_VBO);
            glBindBuffer(GL_ARRAY_BUFFER, ID_VBO);
            glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        }

        void Bind_VBO(){
            glBindBuffer(GL_ARRAY_BUFFER, ID_VBO);
        }

        void Unbind_VBO(){
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        void Eliminar_VBO(){
            glDeleteBuffers(1, &ID_VBO);
        }
};

// ############################ 1. CLASE VAO ########################################
class VAOClass{
    public:
        unsigned int ID_VAO;

        VAOClass(){
            glGenVertexArrays(1, &ID_VAO);
        }

        void Link_VBO(VBOClass VBO, unsigned int layout){
            VBO.Bind_VBO();
            glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            VBO.Unbind_VBO();
        }

        void Bind_VAO(){
            glBindVertexArray(ID_VAO);
        }

        void Unbind_VAO(){
            glBindVertexArray(0);
        }

        void Eliminar_VAO(){
            glDeleteVertexArrays(1, &ID_VAO);
        }   
};

// ############################ 3. CLASE EBO ########################################
class EBOClass{
    public:
        unsigned int ID_EBO;

        // --> Pasamos un vector como parámetro
        EBOClass( vector<unsigned int> indices ){
            glGenBuffers(1, &ID_EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        }

        // --> Pasamos un array como parámetro
        EBOClass( unsigned int* indices, unsigned int size ){
            glGenBuffers(1, &ID_EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
        }

        void Bind_EBO(){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID_EBO);
        }

        void Unbind_EBO(){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        void Eliminar_EBO(){
            glDeleteBuffers(1, &ID_EBO);
        }
};

// ########################### 4. CLASE FIGURA ######################################
struct Figure_WidthIndices{
    vector<float> verticesVector;
    vector<unsigned int> indicesVector;
    vector<vector<float>> verticesMatrix;

    Figure_WidthIndices(float vertices[], int verticesSize, unsigned int indices[], int indicesSize) {
        // Creamos el vector de vertices
        for (int i = 0; i < verticesSize; i++) {
            verticesVector.push_back(vertices[i]);
        }

        // Creamos el vector de índices
        for (int i = 0; i < indicesSize; i++) {
            indicesVector.push_back(indices[i]);
        }

        // Creamos la matriz de vertices (4x4)
        for (int j = 0; j < 3; j++) {
            vector<float> auxVector;
            int i = j;
            for (i; i < verticesVector.size(); i = i + 3) {
                auxVector.push_back(verticesVector[i]);
            }
            verticesMatrix.push_back(auxVector);
        }
        vector<float> auxVector; // + Añadimos la última fila de la matriz (1, 1, 1, 1)
        for (int i = 0; i < verticesMatrix[0].size(); i++) {
            auxVector.push_back(1.0f);
        }
        verticesMatrix.push_back(auxVector);
    }

    Figure_WidthIndices(vector<float> vertices, vector<unsigned int> indices) {
        // Creamos el vector de vertices
        for (int i = 0; i < vertices.size(); i++) {
            verticesVector.push_back(vertices[i]);
        }

        // Creamos el vector de índices
        for (int i = 0; i < indices.size(); i++) {
            indicesVector.push_back(indices[i]);
        }

        // Creamos la matriz de vertices (4x4)
        for (int j = 0; j < 3; j++) {
            vector<float> auxVector;
            int i = j;
            for (i; i < verticesVector.size(); i = i + 3) {
                auxVector.push_back(verticesVector[i]);
            }
            verticesMatrix.push_back(auxVector);
        }
        vector<float> auxVector; // + Añadimos la última fila de la matriz (1, 1, 1, 1)
        for (int i = 0; i < verticesMatrix[0].size(); i++) {
            auxVector.push_back(1.0f);
        }
        verticesMatrix.push_back(auxVector);
    }

    // --> Función para bindear y unbind el VAO, VBO y EBO del objeto/figura que se está dibujando
    void makeBind( VAOClass VAO_Object, VBOClass VBO_Object, EBOClass EBO_Object ){
        VAO_Object.Link_VBO(VBO_Object, 0);

        VAO_Object.Unbind_VAO();
        VBO_Object.Unbind_VBO();
        EBO_Object.Unbind_EBO();
    }

    // --> Funcion para dibujar el objeto/figura
    void makeDraw( VBOClass VBO_Object, VAOClass VAO_Object, ShaderClass ShaderClass_Object ){
        // --> Enlace con VBO
        VBO_Object.Bind_VBO();
        // --> Cargamos los datos al VBO
        glBufferData(GL_ARRAY_BUFFER, verticesVector.size() * sizeof(float), verticesVector.data() , GL_STATIC_DRAW);

        ShaderClass_Object.Activar_ShaderProgram();

        VAO_Object.Bind_VAO();
        glDrawElements(currentDrawMode, indicesVector.size(), GL_UNSIGNED_INT, 0);
    }

    // --> Funcion para eliminar el VAO, VBO y EBO del objeto/figura que se está dibujando
    void makeDelete( VAOClass VAO_Object, VBOClass VBO_Object, EBOClass EBO_Object ){
        VAO_Object.Eliminar_VAO();
        VBO_Object.Eliminar_VBO();
        EBO_Object.Eliminar_EBO();
    }
};


// ################################# FUNCIONES ###################################
// --> Configurar la pantalla
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// --> Procesar la entrada del teclado
void processInput(GLFWwindow *window){
    // --> Cambiar primitiva
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        currentDrawMode = GL_POINTS;

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        currentDrawMode = GL_LINE_STRIP;

    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        currentDrawMode = GL_TRIANGLES;

    // --> Inicio el recorrido
    if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        start_casa = true;

    if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        start_estrella = true;
    
    if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        start_circulo = true;

    // --> Aplicar inversa a todas las transformaciones hechas
    if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
        start_casa_inv = true;
    }

    if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        start_estrella_inv = true;
    }

    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
        start_circulo_inv = true;
    }

    if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
        start_Triangulo = true;
    }
}

// --> Cambiar flagFixPosition a true
void makeFixPositionTrue(string key){
    if(key == "casa"){
        flagFixPosition_C = true;
    }
    else if(key == "circulo"){
        flagFixPosition_Cir = true;
    }
    else if(key == "estrella"){
        flagFixPosition_E = true;
    }
}

// --> Verificar si choque con la pared
bool checkCollision( vector<float> vertices, float radio ){
    float radio1 = radio;
    float radio2 = 0-radio;

    bool collision = false;
    for( int i = 0 ; i < vertices.size(); i++){
        if( vertices[i] <= radio2 || vertices[i] >= radio1 ){ // Hasta que vertices[i] sea menor o igual a -1.0f (hasta que lo pase del muro)
            collision = true;
            break;
        }
    }
    return collision;
}

bool checkCollisionV2( vector<vector<float>> vertices, float radio ){ 
    float radio1 = radio;
    float radio2 = 0-radio;

    for(int i = 0; i < vertices.size()-1; i++){ // -1 para que no tome la ultima fila de unos(1)
        for(int j = 0; j < vertices[i].size(); j++){
            if( vertices[i][j] < radio2 || vertices[i][j] > radio1 ){ // SI HAY COLISION
                /*
                cout << "###################################################################################################" << endl;
                
                cout << "--> Puntos Futuros:" << endl;
                for(int k = 0; k < vertices.size(); k++){
                    for(int l = 0; l < vertices[k].size(); l++){
                        cout << vertices[k][l] << " ";
                    }
                    cout << endl;
                }
                
                cout << "[ " << vertices[i][j] << " < " << radio2 << " || " << vertices[i][j] << " > " << radio1 << " ]" << endl;
                cout << "###################################################################################################" << endl;
                */
                return true;
            }
        }
    }
    return false;
}

// --> Print Matrix
void printMatrix (vector<vector<float>> matrizPuntos){
    for (int i = 0; i < matrizPuntos.size(); ++i) {
        for (int j = 0; j < matrizPuntos[i].size(); ++j) {
            cout << matrizPuntos[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// --> Multiplicacion de matrices
vector<vector<float>> multiplicacionMat (vector<vector<float>> a, vector<vector<float>> b){
    // Verificar si las matrices son multiplicables
    if (a[0].size() != b.size()) {
        cerr << "Error: Las matrices no son multiplicables." << std::endl;
        return vector<vector<float>>(); // Matriz vacia
    }

    // Tamaño de las matrices de entrada
    int filasA = a.size();
    int columnasA = a[0].size();
    int columnasB = b[0].size();

    // Inicializar la matriz de resultado con ceros
    vector<vector<float>> resultado(filasA, vector<float>(columnasB, 0.0f));

    // Realizar la multiplicación de matrices
    for (int i = 0; i < filasA; ++i) {
        for (int j = 0; j < columnasB; ++j) {
            for (int k = 0; k < columnasA; ++k) {
                resultado[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    return resultado;
}

// --> RETURN [TRANSFORMACION: Traslacion]
vector<vector<float>> ReturnTraslacion( float tx, float ty, float tz ){
    vector<vector<float>> matriz_traslacion = {
        {1.0f, 0.0f, 0.0f, tx},
        {0.0f, 1.0f, 0.0f, ty},
        {0.0f, 0.0f, 1.0f, tz},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    return matriz_traslacion;

}

// --> RETURN [TRANSFORMACION: Traslacion - Inversa]
vector<vector<float>> ReturnTraslacionInversa( float tx, float ty, float tz ){
    vector<vector<float>> matriz_traslacion = {
        {1.0f, 0.0f, 0.0f, -tx},
        {0.0f, 1.0f, 0.0f, -ty},
        {0.0f, 0.0f, 1.0f, -tz},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    return matriz_traslacion;
}

// --> RERTURN [TRANSFORMACION: Escalado]
vector<vector<float>> ReturnEscalado( float sx, float sy, float sz ){
    vector<vector<float>> matriz_escalado = {
        {sx, 0.0f, 0.0f, 0.0f},
        {0.0f, sy, 0.0f, 0.0f},
        {0.0f, 0.0f, sz, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    return matriz_escalado;
}

// --> RETURN [TRANSFORMACION: Escalado - Inversa]
vector<vector<float>> ReturnEscaladoInversa( float sx, float sy, float sz ){
    vector<vector<float>> matriz_escalado = {
        {1/sx, 0.0f, 0.0f, 0.0f},
        {0.0f, 1/sy, 0.0f, 0.0f},
        {0.0f, 0.0f, 1/sz, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    return matriz_escalado;
}

// --> RETURN [TRANSFORMACION: Rotacion]
vector<vector<float>> ReturnRotacion( float angulo, string eje ){
    vector<vector<float>> matriz_rotacion;
    if(eje == "x"){
        matriz_rotacion = {
            {1.0f, 0.0f       , 0.0f        , 0.0f},
            {0.0f, cos(angulo), -sin(angulo), 0.0f},
            {0.0f, sin(angulo), cos(angulo) , 0.0f},
            {0.0f, 0.0f       , 0.0f        , 1.0f}
        };
    }
    else if(eje == "y"){
        matriz_rotacion = {
            {cos(angulo), 0.0f, sin(angulo), 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {-sin(angulo), 0.0f, cos(angulo) , 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
    }
    else if(eje == "z"){
        matriz_rotacion = {
            {cos(angulo), -sin(angulo), 0.0f, 0.0f},
            {sin(angulo), cos(angulo) , 0.0f, 0.0f},
            {0.0f       , 0.0f        , 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
    }
    return matriz_rotacion;
}

// --> RETURN [TRANSFORMACION: Rotacion - Inversa]
vector<vector<float>> ReturnRotacionInversa( float angulo, string eje ){
    vector<vector<float>> matriz_rotacion;
    if(eje == "x"){
        matriz_rotacion = {
            {1.0f, 0.0f       , 0.0f        , 0.0f},
            {0.0f, cos(angulo), sin(angulo) , 0.0f},
            {0.0f, -sin(angulo), cos(angulo), 0.0f},
            {0.0f, 0.0f       , 0.0f        , 1.0f}
        };
    }
    else if(eje == "y"){
        matriz_rotacion = {
            {cos(angulo), 0.0f, -sin(angulo), 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {sin(angulo), 0.0f, cos(angulo) , 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
    }
    else if(eje == "z"){
        matriz_rotacion = {
            {cos(angulo), sin(angulo), 0.0f, 0.0f},
            {-sin(angulo), cos(angulo) , 0.0f, 0.0f},
            {0.0f       , 0.0f        , 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        };
    }
    return matriz_rotacion;
}

// --> Funcion con las matrices para la transformacion (retorna la nueva matriz con los cambios hechos)
vector<vector<float>> getTransformationMatrix(vector<string> informacion, vector<vector<float>> matrizPuntos){
    /* LOGICA:
        - Si la cantidad de operaciones es 1:
            - Si solo pide trasladar, multiplicamos por la matriz de traslacion y retornamos la matriz de puntos
        - Si la canatidad de operaciones es > 1:
            - [Tenemos que llevar al centro de coordenadas, multiplicar los puntos por la matriz de escala o rotacion, y volver a la posicion original]
            - LOGICA:
            - Calcular el centro de coordenadas (Cx,Cy) = ( promedio de x, promedio de y )
            - Dichos Cx y Cy son la distancia que se tiene que trasladar la figura para llegar al centro de coordenadas
                - (distTransCentroX, distTrasCentroY) = (Cx, Cy)
            - Restamos a todos los puntos la distancia que se tiene que trasladar el centro de la figura
                - (Ax,Ay) - (distTransCentroX, distTrasCentroY) = (Ax',Ay')
                - (Bx,By) - (distTransCentroX, distTrasCentroY) = (Bx',By')
                - ...
            - Multiplicamos todas las matrices de transformacion en el orden que se nos pide
            - Multiplicamos las matriz de transformacion obtenida por la matriz de puntos que ya se traslado al centro de coordenadas
            - Sumamos a todos los puntos la distancia que se tiene que trasladar el centro de la figura
                - (Ax',Ay') + (distTransCentroX, distTrasCentroY) = (Ax,Ay)
                - (Bx',By') + (distTransCentroX, distTrasCentroY) = (Bx,By)
                - ...
            - Retornamos la matriz de puntos
        // RECORDAR: Multiplicar de derecha a izquierda
        // RECORDAR: Multiplicar por la matriz de puntos al final
        // RECORDAR: Multiplicar en orden Escala * Rotacion * Traslacion
    */
    
    // --> Calculamos cuantas matrices/operaciones se van a realizar
    int numero_matrices;
    if( (informacion.size())/4 == 1 ){
        numero_matrices = 1;
    }
    else{
        numero_matrices = (informacion.size())/4;
    }

    //cout << "Numero de matrices: " << numero_matrices << endl;

    if(numero_matrices == 1){
        // --> Guardamos la matriz de transformacion respectiva
        vector<vector<float>> matriz_acumulado;
        if( informacion[0] == "trasladar" ){
            matriz_acumulado = ReturnTraslacion( stof(informacion[1]), stof(informacion[2]), stof(informacion[3]) );
        }
        else if ( informacion[0] == "escalar" ){
            matriz_acumulado = ReturnEscalado( stof(informacion[1]), stof(informacion[2]), stof(informacion[3]) );
        }
        else if ( informacion[0] == "rotar" ){
            matriz_acumulado = ReturnRotacion( stof(informacion[1]), informacion[2] );
        }

        // --> Multiplicamos la matriz de transformacion por la matriz de puntos
        vector<vector<float>> matriz_resultado = multiplicacionMat( matriz_acumulado, matrizPuntos );

        // --> Retornamos la matriz de puntos con los cambios hechos
        return matriz_resultado;
    }

    else{
        // --> Calculamos el centro de coordenadas
        vector<float> centroCoordenadas;
        for(int i = 0; i < matrizPuntos.size()-1; i++){ // -1 para que no tome en cuenta la ultima fila de 1's
            float sumaFila = 0;
            for(int j = 0; j < matrizPuntos[i].size(); j++){
                sumaFila = sumaFila + matrizPuntos[i][j];
            }
            sumaFila = sumaFila / matrizPuntos[i].size();
            centroCoordenadas.push_back(sumaFila);
        }

        /*
        cout << "Centro de coordenadas: " << endl;
        for(int i = 0; i < centroCoordenadas.size(); i++){
            cout << centroCoordenadas[i] << " ";
        }
        cout << endl << endl;
        */

        // --> Ya tenemos la distancia que se tiene que trasladar la figura para llegar al centro de coordenadas en "centroCoordenadas" (Cx,Cy)

        // --> Restamos a todos los puntos la distancia que se tiene que trasladar el centro de la figura
        vector<vector<float>> matriz_Centrada;
        for(int i = 0; i < matrizPuntos.size()-1; i++){ // -1 para que no tome en cuenta la ultima fila de 1's
            vector<float> fila;
            for(int j = 0; j < matrizPuntos[i].size(); j++){
                fila.push_back( matrizPuntos[i][j] - centroCoordenadas[i] );
            }
            matriz_Centrada.push_back(fila);
        }
        vector<float> fila_unos; // * OJO: Añadimos la fila de 1's al final
        for(int i = 0; i < matrizPuntos[0].size(); i++){
            fila_unos.push_back(1.0f);
        }
        matriz_Centrada.push_back(fila_unos);

        /*
        cout << "Matriz Centrada: " << endl;
        for(int i = 0; i < matriz_Centrada.size(); i++){
            for(int j = 0; j < matriz_Centrada[i].size(); j++){
                cout << matriz_Centrada[i][j] << " ";
            }
            cout << endl;
        }
        */

        // --> Multiplicamos todas las matrices de transformacion en el orden que se nos pide
        // * Guardamos la matriz de trasnformacion de la ultima posicion
        vector<vector<float>> matriz_acumulado;
        int pnt_Aux = (numero_matrices*4)-4; // apuntamos a la pocicion de la ultima matriz de informacion. FORMULA (nMatrices*4)-4
        if( informacion[pnt_Aux] == "trasladar" ){
            matriz_acumulado = ReturnTraslacion( stof(informacion[pnt_Aux+1]), stof(informacion[pnt_Aux+2]), stof(informacion[pnt_Aux+3]) );
        }
        else if ( informacion[pnt_Aux] == "escalar" ){
            matriz_acumulado = ReturnEscalado( stof(informacion[pnt_Aux+1]), stof(informacion[pnt_Aux+2]), stof(informacion[pnt_Aux+3]) );
        }
        else if ( informacion[pnt_Aux] == "rotar" ){
            matriz_acumulado = ReturnRotacion( stof(informacion[pnt_Aux+1]), informacion[pnt_Aux+2] );
        }
        pnt_Aux = pnt_Aux - 4; // apuntamos a la pocicion de la penultima matriz de informacion. FORMULA (nMatrices*3)-3 - 3

        for(int i = 0; i < numero_matrices-1; i++){ //(numMatrices - 1 ) es el numero de operaciones
            // * Obtenemos informacion de la matriz actual
            vector<vector<float>> matriz_actual;
            if( informacion[pnt_Aux] == "trasladar"){
                matriz_actual = ReturnTraslacion( stof(informacion[pnt_Aux+1]), stof(informacion[pnt_Aux+2]), stof(informacion[pnt_Aux+3]) );
            }
            else if ( informacion[pnt_Aux] == "escalar" ){
                matriz_actual = ReturnEscalado( stof(informacion[pnt_Aux+1]), stof(informacion[pnt_Aux+2]), stof(informacion[pnt_Aux+3]) );
            }
            else if ( informacion[pnt_Aux] == "rotar" ){
                matriz_actual = ReturnRotacion( stof(informacion[pnt_Aux+1]), informacion[pnt_Aux+2] );
            }

            // * Multiplicamos la matriz acumulada con la matriz actual
            /*
            cout << endl <<  "Multiplicamos: " << endl;
            cout << "Matriz acumulada: " << endl;
            for(int i = 0; i < matriz_acumulado.size(); i++){
                for(int j = 0; j < matriz_acumulado[i].size(); j++){
                    cout << matriz_acumulado[i][j] << " ";
                }
                cout << endl;
            }
            cout << "Matriz actual: " << endl;
            for(int i = 0; i < matriz_actual.size(); i++){
                for(int j = 0; j < matriz_actual[i].size(); j++){
                    cout << matriz_actual[i][j] << " ";
                }
                cout << endl;
            }
            cout << endl << endl;
            */

            vector<vector<float>> matrixResult = multiplicacionMat( matriz_acumulado, matriz_actual );
            matriz_acumulado = matrixResult; 

            // * Actualizamos el apuntador
            pnt_Aux = pnt_Aux - 4;
        }

        // --> Multiplicamos las matriz de transformacion obtenida por la matriz de puntos que ya esta centrada
        vector<vector<float>> newMatrizPointsCentrada = multiplicacionMat( matriz_acumulado, matriz_Centrada );

        // --> Sumamos a todos los puntos la distancia que se tiene que trasladar el centro de la figura
        vector<vector<float>> newMatrizPoints;
        for(int i = 0; i < newMatrizPointsCentrada.size()-1; i++){ // -1 para que no tome en cuenta la ultima fila de 1's
            vector<float> fila;
            for(int j = 0; j < newMatrizPointsCentrada[i].size(); j++){
                fila.push_back( newMatrizPointsCentrada[i][j] + centroCoordenadas[i] );
            }
            newMatrizPoints.push_back(fila);
        }
        // * OJO: Añadimos la fila de 1's al final
        vector<float> fila_unos2;
        for(int i = 0; i < newMatrizPointsCentrada[0].size(); i++){
            fila_unos2.push_back(1.0f);
        }
        newMatrizPoints.push_back(fila_unos2);

        // --> Retornamos la nueva matriz de puntos
        return newMatrizPoints;
    }
}

// --> Funcion para animar un objeto
void animacion(vector<string> informacion, vector<vector<float>> &verticesMatrix_Casa, vector<float> &verticesVector_Casa, vector<bool> &isReady, int index, float colisionRadius, string keyFlagFixPosition){    
    // --> Obtengo la nueva matriz con las nuevas posiciones
    vector<vector<float>> newPoints = getTransformationMatrix(informacion, verticesMatrix_Casa);

    // --> Verifico si dichas posiciones no colisionan con los limites
    if(checkCollisionV2(newPoints, colisionRadius) == true){ // Si colisionan --> break
        isReady[index] = true;
        makeFixPositionTrue(keyFlagFixPosition); // flagFixPosition = true;

        return;
    }
    else{ // Si no colisionan --> Actualizo la matriz
        verticesMatrix_Casa = newPoints;
        
        // --> Pasar la informacion de la matriz a verticesVector_Casa
        verticesVector_Casa.clear();
        for(int i = 0; i < verticesMatrix_Casa[0].size(); i++){
            verticesVector_Casa.push_back(verticesMatrix_Casa[0][i]);
            verticesVector_Casa.push_back(verticesMatrix_Casa[1][i]);
            verticesVector_Casa.push_back(verticesMatrix_Casa[2][i]);
        }
        return;
    }
}

// --> Funcion para animar un objeto en su inversa
void animacion_Inv(vector<string> informacion, vector<vector<float>> &verticesMatrix_Casa, vector<float> &verticesVector_Casa){
    // --> Calculamos el numero de matrices
    int numero_matrices = informacion.size() / 4;
    if( (informacion.size())/4 == 1 ){
        numero_matrices = 1;
    }
    else{
        numero_matrices = (informacion.size())/4;
    }

    if(numero_matrices == 1){
        // --> Guardamos la matriz de transformacion respectiva
        vector<vector<float>> matriz_acumulado;
        if( informacion[0] == "trasladar" ){
            matriz_acumulado = ReturnTraslacionInversa( stof(informacion[1]), stof(informacion[2]), stof(informacion[3]) );
        }
        else if ( informacion[0] == "escalar" ){
            matriz_acumulado = ReturnEscaladoInversa( stof(informacion[1]), stof(informacion[2]), stof(informacion[3]) );
        }
        else if ( informacion[0] == "rotar" ){
            matriz_acumulado = ReturnRotacionInversa( stof(informacion[1]), informacion[2] );
        }

        // --> Multiplicamos la matriz de transformacion por la matriz de puntos
        vector<vector<float>> matriz_resultado = multiplicacionMat( matriz_acumulado, verticesMatrix_Casa );
        
        // --> Retornamos la nueva matriz de puntos
        verticesMatrix_Casa = matriz_resultado;
    }
    
    else{
        // --> Multiplicamos las matrices de transformacion en el orden inverso
        vector<vector<float>> matriz_acumulado;
        int pnt_Aux = 0;
        if( informacion[pnt_Aux] == "trasladar" ){
            matriz_acumulado = ReturnTraslacionInversa( stof(informacion[pnt_Aux+1]), stof(informacion[pnt_Aux+2]), stof(informacion[pnt_Aux+3]) );
        }
        else if ( informacion[pnt_Aux] == "escalar" ){
            matriz_acumulado = ReturnEscaladoInversa( stof(informacion[pnt_Aux+1]), stof(informacion[pnt_Aux+2]), stof(informacion[pnt_Aux+3]) );
        }
        else if ( informacion[pnt_Aux] == "rotar" ){
            matriz_acumulado = ReturnRotacionInversa( stof(informacion[pnt_Aux+1]), informacion[pnt_Aux+2] );
        }
        pnt_Aux = pnt_Aux + 4; // avanzamos a la siguiente matriz de informacion

        for(int i = 0; i < numero_matrices-1; i++){
            // * Obtenemos la informacion de la matriz actual
            vector<vector<float>> matriz_actual;
            if( informacion[pnt_Aux] == "trasladar" ){
                matriz_actual = ReturnTraslacionInversa( stof(informacion[pnt_Aux+1]), stof(informacion[pnt_Aux+2]), stof(informacion[pnt_Aux+3]) );
            }
            else if ( informacion[pnt_Aux] == "escalar" ){
                matriz_actual = ReturnEscaladoInversa( stof(informacion[pnt_Aux+1]), stof(informacion[pnt_Aux+2]), stof(informacion[pnt_Aux+3]) );
            }
            else if ( informacion[pnt_Aux] == "rotar" ){
                matriz_actual = ReturnRotacionInversa( stof(informacion[pnt_Aux+1]), informacion[pnt_Aux+2] );
            }

            vector<vector<float>> matriz_resultado = multiplicacionMat( matriz_acumulado, matriz_actual );
            matriz_acumulado = matriz_resultado;

            pnt_Aux = pnt_Aux + 4; // avanzamos a la siguiente matriz de informacion
        }

        // --> Multiplicamos la matriz de transformacion por la matriz de puntos
        vector<vector<float>> newMatrizPoints = multiplicacionMat( matriz_acumulado, verticesMatrix_Casa );

        // --> Retornamos la nueva matriz de puntos
        verticesMatrix_Casa = newMatrizPoints;
    }

    cout << "Resultado de las matrices Inversas" << endl;
    printMatrix(verticesMatrix_Casa);

    // --> Pasar la informacion de la matriz a verticesVector_Casa
    verticesVector_Casa.clear();
    for(int i = 0; i < verticesMatrix_Casa[0].size(); i++){
        verticesVector_Casa.push_back(verticesMatrix_Casa[0][i]);
        verticesVector_Casa.push_back(verticesMatrix_Casa[1][i]);
        verticesVector_Casa.push_back(verticesMatrix_Casa[2][i]);
    }
}

// --> Funciones para crear el vector de puntos e indices del circulo
vector<float> getCirclePoints(float _radio){
    vector<float> verticeVector_Circulo;

    for( int i = 0; i < nTriangles; i = i+1.0  ){
        float point1, point2;

        point1 = i * angle;

        pos_x = _radio * cos(point1);
        pos_y = _radio * sin(point1);

        verticeVector_Circulo.push_back(pos_x);
        verticeVector_Circulo.push_back(pos_y);
        verticeVector_Circulo.push_back(0.0f);

        point2 = (i+1.0) * angle;

        pos_x = _radio * cos(point2);
        pos_y = _radio * sin(point2);

        verticeVector_Circulo.push_back(pos_x);
        verticeVector_Circulo.push_back(pos_y);
        verticeVector_Circulo.push_back(0.0f);

        verticeVector_Circulo.push_back(0.0f);
        verticeVector_Circulo.push_back(0.0f);
        verticeVector_Circulo.push_back(0.0f);

        //cout << "Triangulo " << i << " listo: " << verticeVector_Circulo.size() << endl;
    }

    return verticeVector_Circulo;
}

vector<unsigned int> getCircleIndex (vector<float> verticeVector_Circulo){
    vector<unsigned int> indexVector_Circulo;
    for(int i = 0; i < verticeVector_Circulo.size(); i++){
        indexVector_Circulo.push_back(i);
    }
    //cout << "Indices listos: " << indexVector_Circulo.size() << endl;
    return indexVector_Circulo;
}

// ################################ FIGURAS #################################
// *Estrella
float vertices_Estrella[] = {
    0.0f ,  0.24f , 0.0f, // superior (A)               [0]
    0.15f,  0.13f, 0.0f, // superior derecha (B)       [1]
    0.10f, -0.06f, 0.0f, // inferior derecha (C)       [2]
    -0.1f, -0.05f, 0.0f, // inferior izquierda (D)     [3]
    -0.14f,  0.13f, 0.0f, // superior izquierda (E)     [4]
    0.04f,  0.13f, 0.0f, // vertice A-B                [5]
    0.06f,  0.06f, 0.0f, // vertice B-C                [6]
    0.0f , 0.02f, 0.0f, // vertice C-D                [7]
    -0.05f, 0.06f, 0.0f, // vertice D-E                [8]
    -0.03f,  0.13f, 0.0f, // vertice E-A                [9]
}; 

unsigned int indices_Estrella[] = {
    9, 5, 0, // Triangulo Arriba
    5, 6, 1, // Triangulo Derecha
    6, 7, 2, // Triangulo Abajo derecha
    7, 8, 3, // Triangulo Abajo izquierda
    8, 9, 4, // Triangulo Izquierda
    // Centro
    8, 5, 9,
    5, 7, 6,
    8, 7, 5
};

// *Casa
float vertices_Casa[] = {
    0.05f ,  0.0f, 0.0f,  
    0.05f , -0.1f, 0.0f,  
    -0.05f, -0.1f, 0.0f, 
    -0.05f,  0.0f, 0.0f, 
    0.1f  ,  0.0f, 0.0f,
    -0.1f ,  0.0f, 0.0f, 
    -0.0f ,  0.06f, 0.0f, 
};

unsigned int indices_Casa[] = {  
    0, 1, 3, 
    1, 2, 3, 
    5, 6, 4,
};

// Triangulo (EXAMEN)
float vertices_Triangulo[] = {
    -0.5f , -0.5f , 0.0f,
     0.5  , -0.5f , 0.0f,
     0.0f ,  0.5f , 0.0f,
};

unsigned int indices_Triangulo[]={
    0, 1, 2,
};

// ########################## FUNCION MAIN ###################################
int main()
{
    cout << "MENU" << endl;
    //cout << "Activar animacion de casa: ------------------ B" << endl;
    //cout << "Activar animacion de estrella: -------------- N" << endl;
    //cout << "Activar animacion de circulo: --------------- M" << endl;
    cout << "Activar animacion de triangulo: --------------- Z" << endl;

    cout << endl;
    cout << "Primitiva 1 --------------------------------- Q" << endl;
    cout << "Primitiva 2 --------------------------------- W" << endl;
    cout << "Primitiva 3 --------------------------------- E" << endl;
    cout << endl;
    //cout << "Activar animacion de casa (inversa): -------- I" << endl;
    //cout << "Activar animacion de estrella (inversa): ---- O" << endl;
    //cout << "Activar animacion de circulo (inversa): ----- P" << endl;

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

    // ---------------------------------------
    gladLoadGL(glfwGetProcAddress);
    // ---------------------------------------

    // --> Configuracion de los shaders
    ShaderClass shaderProgram(vertexShaderSource, fragmentShaderSource);

    // --> Creamos las figuras
    // * Casa
    Figure_WidthIndices figure_Casa(vertices_Casa, sizeof(vertices_Casa) / sizeof(float), indices_Casa, sizeof(indices_Casa) / sizeof(unsigned int));

    // * Estrella
    Figure_WidthIndices figure_Estrella(vertices_Estrella, sizeof(vertices_Estrella) / sizeof(float), indices_Estrella, sizeof(indices_Estrella) / sizeof(unsigned int));
    
    // * Circulo
    vector<float> verticeVector_Circulo = getCirclePoints(radio);
    vector<unsigned int> indicesVector_Circulo = getCircleIndex(verticeVector_Circulo);
    Figure_WidthIndices figure_Circulo(verticeVector_Circulo, indicesVector_Circulo);

    // TRIANGULO (EXAMEN)
    Figure_WidthIndices figure_Triangulo(vertices_Triangulo, sizeof(vertices_Triangulo)/sizeof(float), indices_Triangulo, sizeof(indices_Triangulo)/sizeof(unsigned int));

    // --> VAO, VBO, EBO for Casa
    VAOClass VAO_Casa; VAO_Casa.Bind_VAO();
    VBOClass VBO_Casa(figure_Casa.verticesVector); EBOClass EBO_Casa(figure_Casa.indicesVector);
    figure_Casa.makeBind(VAO_Casa, VBO_Casa, EBO_Casa);

    // --> VAO, VBO, EBO for Estrellaz
    VAOClass VAO_Estrella; VAO_Estrella.Bind_VAO();
    VBOClass VBO_Estrella(figure_Estrella.verticesVector); EBOClass EBO_Estrella(figure_Estrella.indicesVector);
    figure_Estrella.makeBind(VAO_Estrella, VBO_Estrella, EBO_Estrella);

    // --> VAO, VBO for Circulo
    VAOClass VAO_Circulo; VAO_Circulo.Bind_VAO();
    VBOClass VBO_Circulo(figure_Circulo.verticesVector); EBOClass EBO_Circulo(figure_Circulo.indicesVector);
    figure_Circulo.makeBind(VAO_Circulo, VBO_Circulo, EBO_Circulo);

    // --> VAO, VBO for triangulo
    VAOClass VAO_Triangulo; VAO_Triangulo.Bind_VAO();
    VBOClass VBO_Triangulo(figure_Triangulo.verticesVector); EBOClass EBO_Triangulo(figure_Triangulo.indicesVector);
    figure_Triangulo.makeBind(VAO_Triangulo, VBO_Triangulo, EBO_Triangulo);

    glPointSize(4.0f);
    glLineWidth(3.0f);

    float speedMovment = 0.000005f;

    vector<bool> isReady_C = {false, false};
    vector<bool> flags_C = {false, false, false, false}; // [0] = Arriba, [1] = Abajo, [2] = Derecha, [3] = Izquierda

    vector<bool> isReady_E = {false, false};
    vector<bool> flags_E = {false, false, false, false}; // [0] = Arriba, [1] = Abajo, [2] = Derecha, [3] = Izquierda

    vector<bool> isReady_Cir = {false, false};
    vector<bool> flags_Cir = {false, false, false, false}; // [0] = Arriba, [1] = Abajo, [2] = Derecha, [3] = Izquierda

    while (!glfwWindowShouldClose(window))
    {
        // --> Inputs de teclado
        processInput(window);

        // --> Renderizado
        // * Color de fondo: Plomo Oscuro
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ------------------ ANIMACION CASA (escalado y traslacion) -------------------------
        if(start_casa == true){
            if( start_casa_inv == true && star_casa_inv2 == false){ //if( start_casa_inv == true)
                // LOGICA:
                // * 1. Recorremos todas las transformaciones guardadas para esta figura
                // * 2. Aplicamos la inversa de cada transformacion
                // * 3. Si ya no hay mas transformaciones, reiniciamos los flags, y limpiamos el historial de transformaciones

                // --> Recorremos las transformaciones de atras hacia adelante
                for(int i = hist_trans_C.size()-1; i >= 0; i--){
                    // --> Aplicamos la inversa y movemos la figura
                    animacion_Inv(hist_trans_C[i], figure_Casa.verticesMatrix, figure_Casa.verticesVector);

                    // --> Renderizamos la figura
                    figure_Casa.makeDraw(VBO_Casa, VAO_Casa, shaderProgram);
                }

                // --> Reiniciamos los flags
                start_casa_inv = false;
                star_casa_inv2 = true; // ???
                isReady_C = {false, false};
                flags_C = {false, false, false, false};

                // --> Limpiamos el historial de transformaciones
                hist_trans_C.clear();

                cout << "Start_casa_inv es " << start_casa_inv << endl; 

                cout << "Fin de la lista de transformaciones" << endl;
            }

            else{
                if ( isReady_C[0] == false && isReady_C[1] == false ){  
                    // cout << "* Movemos al extremo izquierdo" << endl;
                    
                    // --> 1. Movemos al extremo izquierdo
                    vector<string> informacion = {
                                                    "escalar"  , "1.0002"  , "1.0002", "1.0000",
                                                    "trasladar", "-0.0003" , "0.000" , "0.0000" };

                    // * Guardamos las trasnformaciones para luego usar la inversa
                    hist_trans_C.push_back(informacion);

                    animacion(informacion, figure_Casa.verticesMatrix, figure_Casa.verticesVector, isReady_C, 0, 1, "casa" );
                }

                if ( isReady_C[0] == true && isReady_C[1] == false ){
                    if(flagFixPosition_C == true){
                        //cout << endl;
                        //cout << "!!! Acomodamos la posicion mas a la derecha" << endl << endl;

                        vector<string> informacionFix = {
                                                            "trasladar", "0.1" , "0.000", "0.0000" };

                        // * Guardamos las trasnformaciones para luego usar la inversa
                        hist_trans_C.push_back(informacionFix);
                        
                        animacion(informacionFix, figure_Casa.verticesMatrix, figure_Casa.verticesVector, isReady_C, 0, 1, "casa" );
                        flagFixPosition_C = false;
                    }
                    else{
                        //cout << "* Movemos hacia abajo" << endl;

                        // --> 2. Movemos hacia abajo
                        vector<string> informacion = {
                                                        "escalar"  , "0.9998"   , "0.9998", "1.0000",
                                                        "trasladar", "0.000"   , "-0.0003", "0.0000" };

                        // * Guardamos las trasnformaciones para luego usar la inversa
                        hist_trans_C.push_back(informacion);

                        animacion(informacion, figure_Casa.verticesMatrix, figure_Casa.verticesVector, isReady_C, 1, 1, "casa" );
                    }
                }

                if( isReady_C[0] == true && isReady_C[1] == true ){
                    // --> 4. Bucle en circulos
                    if(flags_C[0] == false && flags_C[1] == false && flags_C[2] == false && flags_C[3] == false){
                        if(flagFixPosition_C == true){
                            //cout << endl;
                            //cout << "!!! Acomodamos la posicion mas arriba" << endl << endl;

                            vector<string> informacionFix = {
                                                                "trasladar", "0.000" , "0.1" , "0.0000" };

                            // * Guardamos las trasnformaciones para luego usar la inversa
                            hist_trans_C.push_back(informacionFix);

                            animacion(informacionFix, figure_Casa.verticesMatrix, figure_Casa.verticesVector, flags_C, 0, 1, "casa" );
                            flagFixPosition_C = false;
                        }
                        else{
                            //cout << "** (B) Movemos a la derecha" << endl;

                            // --> 4.1. Movemos hacia la derecha
                            vector<string> informacion = {
                                                            "escalar"  , "1.00017" , "1.00017", "1.0000",
                                                            "trasladar", "0.00038"   , "0.000" , "0.0000" };
                            
                            // * Guardamos las trasnformaciones para luego usar la inversa
                            hist_trans_C.push_back(informacion);

                            animacion(informacion, figure_Casa.verticesMatrix, figure_Casa.verticesVector, flags_C, 0, 1, "casa");
                        }
                    }

                    if(flags_C[0] == true && flags_C[1] == false && flags_C[2] == false && flags_C[3] == false){
                        if(flagFixPosition_C == true){
                            //cout << endl;
                            //cout << "!!! Acomodamos la posicion mas a la izquierda" << endl << endl;

                            vector<string> informacionFix = {
                                                                "trasladar", "-0.1" , "0.000" , "0.0000" };
                            
                            // * Guardamos las trasnformaciones para luego usar la inversa
                            hist_trans_C.push_back(informacionFix);

                            animacion(informacionFix, figure_Casa.verticesMatrix, figure_Casa.verticesVector, flags_C, 0, 1, "casa");
                            flagFixPosition_C = false;
                        }
                        else{
                            //cout << "** (B) Movemos hacia arriba" << endl;

                            // --> 4.2. Movemos hacia arriba
                            vector<string> informacion = {
                                                            "escalar"  , "0.9999" , "0.9999", "1.0000",
                                                            "trasladar", "0.000"   , "0.0003" , "0.0000"   };

                            // * Guardamos las trasnformaciones para luego usar la inversa
                            hist_trans_C.push_back(informacion);

                            animacion(informacion, figure_Casa.verticesMatrix, figure_Casa.verticesVector, flags_C, 1, 1, "casa");
                        }
                    }

                    if(flags_C[0] == true && flags_C[1] == true && flags_C[2] == false && flags_C[3] == false){
                        if(flagFixPosition_C == true){
                            //cout << endl;
                            //cout << "!!! Acomodamos la posicion mas abajo" << endl << endl;

                            vector<string> informacionFix = {
                                                                "trasladar", "0.000" , "-0.1" , "0.0000"  };
                            
                            // * Guardamos las trasnformaciones para luego usar la inversa
                            hist_trans_C.push_back(informacionFix);

                            animacion(informacionFix, figure_Casa.verticesMatrix, figure_Casa.verticesVector, flags_C, 0, 1, "casa");
                            flagFixPosition_C = false;
                        }
                        else{
                            //cout << "** (B) Movemos a la izquierda" << endl;

                            // --> 4.3. Movemos hacia la izquierda
                            vector<string> informacion = {
                                                            "escalar"  , "1.00015" , "1.00015", "1.0000",
                                                            "trasladar", "-0.00038"   , "0.000" , "0.0000"   };
                            
                            // * Guardamos las trasnformaciones para luego usar la inversa
                            hist_trans_C.push_back(informacion);

                            animacion(informacion, figure_Casa.verticesMatrix, figure_Casa.verticesVector, flags_C, 2, 1, "casa");
                        }
                    }

                    if(flags_C[0] == true && flags_C[1] == true && flags_C[2] == true && flags_C[3] == false){
                        if(flagFixPosition_C == true){
                            //cout << endl;
                            //cout << "!!! Acomodamos la posicion mas a la derecha" << endl << endl;

                            vector<string> informacionFix = {
                                                                "trasladar", "0.1" , "0.000" , "0.0000"  };
                            
                            // * Guardamos las trasnformaciones para luego usar la inversa
                            hist_trans_C.push_back(informacionFix);

                            animacion(informacionFix, figure_Casa.verticesMatrix, figure_Casa.verticesVector, flags_C, 0, 1, "casa");
                            flagFixPosition_C = false;
                        }
                        else{
                            //cout << "** (B) Movemos hacia abajo" << endl;

                            // --> 4.4. Movemos hacia abajo
                            vector<string> informacion = {
                                                            "escalar"  , "0.99985" , "0.99985", "1.0000",
                                                            "trasladar", "0.000"   , "-0.0003", "0.0000"  };
                            
                            // * Guardamos las trasnformaciones para luego usar la inversa
                            hist_trans_C.push_back(informacion);
                            
                            animacion(informacion, figure_Casa.verticesMatrix, figure_Casa.verticesVector, flags_C, 3, 1, "casa");
                        }
                    }

                    if(flags_C[0] == true && flags_C[1] == true && flags_C[2] == true && flags_C[3] == true){

                        ////cout << "REINICIAMOS EL BUCLE" << endl << endl;
                        flags_C[0] = false;
                        flags_C[1] = false;
                        flags_C[2] = false;
                        flags_C[3] = false;
                    }
                }
            
                figure_Casa.makeDraw(VBO_Casa, VAO_Casa, shaderProgram);
            }
        }

        // ------------------ ANIMACION ESTRELLA (rotacion y traslacion) ------------------
        if( start_estrella == true ){
            if ( isReady_E[0] == false && isReady_E[1] == false ){
                //cout << "* Movemos al extremo izquierdo" << endl;

                // --> 1. Movemos al extremo izquierdo
                vector<string> informacion = {
                                                "rotar"    , "15"      , "x", "0.0000",
                                                "trasladar", "-0.0003" , "0.000" , "0.0000"  };
                animacion(informacion, figure_Estrella.verticesMatrix , figure_Estrella.verticesVector, isReady_E, 0, 0.7, "estrella");
            }

            if ( isReady_E[0] == true && isReady_E[1] == false ){
                if(flagFixPosition_E == true){
                    //cout << endl;
                    //cout << "!!! Acomodamos la posicion mas a la derecha" << endl << endl;

                    vector<string> informacionFix = {
                                                        "trasladar", "0.01" , "0.000" , "0.0000"  };
                    animacion(informacionFix, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, isReady_E, 0, 1, "estrella");
                    flagFixPosition_E = false;
                }
                else{
                    //cout << "* Movemos hacia arriba" << endl;

                    // --> 2. Movemos hacia arriba
                    vector<string> informacion = {
                                                    "rotar"     , "15"    , "x", "0.0000",
                                                    "trasladar" , "0.000" , "0.0003" , "0.0000"  };
                    animacion(informacion, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, isReady_E, 1, 0.7, "estrella");
                }
            }

            if ( isReady_E[0] == true && isReady_E[1] == true ){
                // --> 4. Bucle en circulos
                if(flags_E[0] == false && flags_E[1] == false && flags_E[2] == false && flags_E[3] == false){
                    if(flagFixPosition_E == true ){
                        //cout << endl;
                        //cout << "!!! Acomodamos la posicion mas a abajo" << endl << endl;

                        vector<string> informacionFix = {
                                                            "trasladar", "0.000" , "-0.01" , "0.0000"  };
                        animacion(informacionFix, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, flags_E, 0, 1, "estrella");
                        flagFixPosition_E = false;
                    }
                    else{
                        //cout << "** (E) Movemos hacia la derecha" << endl;

                        // --> 4.1. Movemos hacia la derecha
                        vector<string> informacion = {
                                                        "rotar"     , "15"    , "x", "0.0000",
                                                        "trasladar" , "0.0003" , "0.000" , "0.0000"  };
                        animacion(informacion, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, flags_E, 0, 0.7, "estrella");
                    }
                }

                if(flags_E[0] == true && flags_E[1] == false && flags_E[2] == false && flags_E[3] == false){
                    if(flagFixPosition_E == true){
                        //cout << endl;
                        //cout << "!!! Acomodamos la posicion mas a la izquierda" << endl << endl;

                        vector<string> informacionFix = {
                                                            "trasladar", "-0.01" , "0.000" , "0.0000"  };
                        animacion(informacionFix, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, flags_E, 0, 1, "estrella");
                        flagFixPosition_E = false;
                    }
                    else{
                        //cout << "** (E) Movemos hacia abajo" << endl;

                        // --> 4.2. Movemos hacia abajo
                        vector<string> informacion = {
                                                        "rotar"     , "15"    , "x", "0.0000",
                                                        "trasladar" , "0.000" , "-0.0003" , "0.0000"  };
                        animacion(informacion, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, flags_E, 1, 0.7, "estrella");
                    }
                }
            
                if(flags_E[0] == true && flags_E[1] == true && flags_E[2] == false && flags_E[3] == false){
                    if(flagFixPosition_E == true){
                        //cout << endl;
                        //cout << "!!! Acomodamos la posicion mas arriba" << endl << endl;

                        vector<string> informacionFix = {
                                                            "trasladar", "0.000" , "0.01" , "0.0000"  };
                        animacion(informacionFix, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, flags_E, 1, 1, "estrella");
                        flagFixPosition_E = false;
                    }
                    else{
                        //cout << "** (E) Movemos hacia la izquierda" << endl;

                        // --> 4.3. Movemos hacia la izquierda
                        vector<string> informacion = {
                                                        "rotar"     , "15"    , "x", "0.0000",
                                                        "trasladar" , "-0.0003" , "0.000" , "0.0000"  };
                        animacion(informacion, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, flags_E, 2, 0.7, "estrella");
                    }       
                }

                if(flags_E[0] == true && flags_E[1] == true && flags_E[2] == true && flags_E[3] == false){
                    if(flagFixPosition_E == true){
                        //cout << endl;
                        //cout << "!!! Acomodamos la posicion mas a la derecha" << endl << endl;

                        vector<string> informacionFix = {
                                                            "trasladar", "0.01" , "0.000" , "0.0000"  };
                        animacion(informacionFix, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, flags_E, 2, 1, "estrella");
                        flagFixPosition_E = false;
                    }
                    else{
                        //cout << "** (E) Movemos hacia arriba" << endl;

                        // --> 4.4. Movemos hacia arriba
                        vector<string> informacion = {
                                                        "rotar"     , "15"    , "x", "0.0000",
                                                        "trasladar" , "0.000" , "0.0003" , "0.0000"  };
                        animacion(informacion, figure_Estrella.verticesMatrix, figure_Estrella.verticesVector, flags_E, 3, 0.7, "estrella");
                    }
                }

                if(flags_E[0] == true && flags_E[1] == true && flags_E[2] == true && flags_E[3] == true){
                    flags_E[0] = false;
                    flags_E[1] = false;
                    flags_E[2] = false;
                    flags_E[3] = false;
                }
            }

            figure_Estrella.makeDraw(VBO_Estrella, VAO_Estrella, shaderProgram);
        }

        // ------------------ ANIMACIÓN CIRCULO (escala, rotacion y traslacion) ------------------
        if(start_circulo == true){
            if ( isReady_Cir[0] == false && isReady_Cir[1] == false ){
                //cout << "* Movemos al extremo izquierdo" << endl;

                // --> 1. Movemos al extremo izquierdo
                vector<string> informacion = {
                                                "escalar"  , "1.0002"  , "1.0002", "1.0000",
                                                "rotar"    , "15"      , "x"     , "0.0000",
                                                "trasladar", "-0.0003" , "0.000" , "0.0000"  };

                /*
                cout << "Matriz de vertices: " << endl;
                for(int i = 0; i < figure_Circulo.verticesMatrix.size(); i++){
                    for(int j = 0; j < figure_Circulo.verticesMatrix[i].size(); j++){   
                        cout << figure_Circulo.verticesMatrix[i][j] << " ";
                    }
                    cout << endl;
                }
                */

                animacion(informacion, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, isReady_Cir, 0, 0.5, "circulo");
            }

            if ( isReady_Cir[0] == true && isReady_Cir[1] == false ){
                if(flagFixPosition_Cir == true){
                    //cout << endl;
                    //cout << "!!! Acomodamos la posicion mas a la derecha" << endl << endl;

                    vector<string> informacionFix = {
                                                        "trasladar", "0.1" , "0.000" , "0.0000"  };
                    animacion(informacionFix, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, isReady_Cir, 0, 1, "circulo" );
                    flagFixPosition_Cir = false;
                }
                else{
                    //cout << "** (C) Movemos hacia abajo" << endl;

                    // --> 2. Movemos hacia abajo
                    vector<string> informacion = {
                                                    "escalar"  , "0.9997"   , "0.9997" , "1.0000",
                                                    "rotar"    , "15"      , "x"       , "0.0000",
                                                    "trasladar", "0.000"   , "-0.0003" , "0.0000"  };
                    animacion(informacion, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, isReady_Cir, 1, 0.5, "circulo");
                }
            }

            if( isReady_Cir[0] == true && isReady_Cir[1] == true ){
                // --> 4. Bucle en circulos
                if(flags_Cir[0] == false && flags_Cir[1] == false && flags_Cir[2] == false && flags_Cir[3] == false){
                    if(flagFixPosition_Cir == true){
                        //cout << endl;
                        //cout << "!!! Acomodamos la posicion mas arriba" << endl << endl;

                        vector<string> informacionFix = {
                                                            "trasladar", "0.000" , "0.1" , "0.0000"  };
                        animacion(informacionFix, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, flags_Cir, 0, 1, "circulo");
                        flagFixPosition_Cir = false;
                    }
                    else{
                        //cout << "** (B) Movemos a la derecha" << endl;

                        // --> 4.1. Movemos hacia la derecha
                        vector<string> informacion = {
                                                        "escalar"  , "1.0002" , "1.0002", "1.0000",
                                                        "rotar"    , "15"      , "x", "0.0000",
                                                        "trasladar", "0.00038"   , "0.000" , "0.0000"  };
                        animacion(informacion, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, flags_Cir, 0, 0.5, "circulo");
                    }
                }

                if(flags_Cir[0] == true && flags_Cir[1] == false && flags_Cir[2] == false && flags_Cir[3] == false){
                    if(flagFixPosition_Cir == true){
                        //cout << endl;
                        //cout << "!!! Acomodamos la posicion mas a la izquierda" << endl << endl;

                        vector<string> informacionFix = {
                                                            "trasladar", "-0.1" , "0.000" , "0.0000"  };
                        animacion(informacionFix, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, flags_Cir, 0, 1, "circulo");
                        flagFixPosition_Cir = false;
                    }
                    else{
                        //cout << "** (B) Movemos hacia arriba" << endl;

                        // --> 4.2. Movemos hacia arriba
                        vector<string> informacion = {
                                                        "escalar"  , "0.9997" , "0.9997", "1.0000",
                                                        "rotar"    , "15"      ,  "x", "0.0000",
                                                        "trasladar", "0.000"   , "0.0003" , "0.0000"  };
                        animacion(informacion, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, flags_Cir, 1, 0.5, "circulo");
                    }
                }

                if(flags_Cir[0] == true && flags_Cir[1] == true && flags_Cir[2] == false && flags_Cir[3] == false){
                    if(flagFixPosition_Cir == true){
                        //cout << endl;
                        //cout << "!!! Acomodamos la posicion mas abajo" << endl << endl;

                        vector<string> informacionFix = {
                                                            "trasladar", "0.000" , "-0.1" , "0.0000"  };
                        animacion(informacionFix, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, flags_Cir, 0, 1, "circulo");
                        flagFixPosition_Cir = false;
                    }
                    else{
                        //cout << "** (B) Movemos a la izquierda" << endl;

                        // --> 4.3. Movemos hacia la izquierda
                        vector<string> informacion = {
                                                        "escalar"  , "1.00045" , "1.00045", "1.0000",
                                                        "rotar"    , "15"      ,  "x", "0.0000",
                                                        "trasladar", "-0.00038"   , "0.000"  , "0.0000"  };
                        animacion(informacion, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, flags_Cir, 2, 0.5, "circulo");
                    }
                }

                if(flags_Cir[0] == true && flags_Cir[1] == true && flags_Cir[2] == true && flags_Cir[3] == false){
                    if(flagFixPosition_Cir == true){
                        //cout << endl;
                        //cout << "!!! Acomodamos la posicion mas a la derecha" << endl << endl;

                        vector<string> informacionFix = {
                                                            "trasladar", "0.1" , "0.000" , "0.0000"  };
                        animacion(informacionFix, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, flags_Cir, 0, 1, "circulo");
                        flagFixPosition_Cir = false;
                    }
                    else{
                        //cout << "** (B) Movemos hacia abajo" << endl;

                        // --> 4.4. Movemos hacia abajo
                        vector<string> informacion = {
                                                        "escalar"  , "0.9998" , "0.9998", "1.0000",
                                                        "rotar"    , "15"      , "x", "0.0000",
                                                        "trasladar", "0.000"   , "-0.0003" , "0.0000"  };

                        animacion(informacion, figure_Circulo.verticesMatrix, figure_Circulo.verticesVector, flags_Cir, 3, 0.5, "circulo");
                    }
                }

                if(flags_Cir[0] == true && flags_Cir[1] == true && flags_Cir[2] == true && flags_Cir[3] == true){
                    flags_Cir[0] = false;
                    flags_Cir[1] = false;
                    flags_Cir[2] = false;
                    flags_Cir[3] = false; 
                }
            }

            figure_Circulo.makeDraw(VBO_Circulo, VAO_Circulo, shaderProgram);
        }
        // ----------------------------------------------------------------------------------------

        if(start_Triangulo == true){
            // Dibujamos la figura base
            figure_Triangulo.makeDraw(VBO_Triangulo, VAO_Triangulo, shaderProgram);

            sleep(2);

            // Dibujamos la nueva figura

            vector<string> informacionINV = {
                                            "trasladar", "1", "0.0", "0.0",
                                            "rotar", "45", "y", "0.0000"};

            animacion_Inv(informacionINV, figure_Triangulo.verticesMatrix, figure_Triangulo.verticesVector);

            vector<string> informacion = {
                                            "escalar" , "1.5", "0.0", "0.0" };

            //animacion(informacion, figure_Triangulo.verticesMatrix, figure_Triangulo.verticesVector, )

            figure_Triangulo.makeDraw(VBO_Triangulo, VAO_Triangulo, shaderProgram);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --> Eliminación de recursos
    // * For casa
    figure_Casa.makeDelete(VAO_Casa, VBO_Casa, EBO_Casa);

    // * For estrella
    figure_Estrella.makeDelete(VAO_Estrella, VBO_Estrella, EBO_Estrella);

    // * For circulo
    figure_Circulo.makeDelete(VAO_Circulo, VBO_Circulo, EBO_Circulo);

    // --> Terminar proceso
    glfwTerminate();
    return 0;
}