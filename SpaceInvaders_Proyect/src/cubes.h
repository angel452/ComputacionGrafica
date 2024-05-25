#ifndef CUBES_H
#define CUBES_H

#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include "figure.h"
#include "shaderClass.h"

using std::vector;
using std::string;
using std::function;
using std::cout;
using std::endl;

const float PI = 3.14159265359;

// ########################### CLASE  CUBO ###############################
class cube {
    public:
        // --> For the cube
        Figure cubes[27];
        int cantidadCubos = 27;
        float originalRubickVertexes[27][216];

        // --> For movement of the cube
        float offx, offy, offz;
        int xMov = 0, yMov = 0, zMov = 0;
        int movCant = 50;
        vector<int> centers = {4, 10, 12, 16, 22};
        float speed = (PI / 2) / 20;
        float acumAngle = 0.0f;
        //int camada = 0;

        // --> Initial scramble per cube
        vector<string> posibleMovments = {"L", "L'", "R", "R'", "U", "U'", "D", "D'", "F", "F'", "B", "B'"};
        vector<string> scramble;
        bool flag_initialScramble = false;
        int actual_movment_scramble = 0;
        bool isScrambled = false;

        // --> For the solver
        bool getSolucionActive = false;
        bool isSolving = false;
        bool isSolving2 = false;
        bool isDead = false;
        vector<string> movimientos_to_solve;
        int number_movments = 0;
        int actual_movment_solution = 0;
        bool isMoving = false; // * Movimiento de camada

        // --> Constructor
        cube();

        // --> Destructor
        ~cube();

        // --> Funciones
        //void theScramble();
        void renderInit();
        void infoCentros();
        void render(ShaderClass);
        void printScramble();

        void rotateCamada_L(int &);
        void rotateCamada_LI(int &);
        void rotateCamada_R(int &);
        void rotateCamada_RI(int &);
        void rotateCamada_U(int &);
        void rotateCamada_UI(int &);
        void rotateCamada_D(int &);
        void rotateCamada_DI(int &);
        void rotateCamada_F(int &);
        void rotateCamada_FI(int &);
        void rotateCamada_B(int &);
        void rotateCamada_BI(int &);

        
        //void rotateCamada();
        void rotate(vector<int>, float, char);

        // * Scrable Functions
        void moveScramble();

        // * Solver Functions
        string convertMovments_toString();
        vector<string> convert_String_to_Movments(string, int);
        vector<string> convert_String_to_MovmentsV2(string);
        void getSolution();
        void moveSolution();

        // * Funciones de Colision
        bool isColliding(float, float, float);
        
        // * Funciones de Movimiento Aleatorio
        void randMove();
        
        // * Additional Functions
        glm::vec3 getCenter();
};
// ########################################################################

#endif