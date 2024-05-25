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

class cube {
    public:
    // * Solver
    float originalRubickVertexes[27][216];
    Figure cubes[27];
    int cantidadCubos = 27;
    vector<string> scramble;
    bool getSolucionActive = false;
    bool isSolving = false;
    bool isSolving2 = false;
    vector<string> movimientos_to_solve;
    int number_movments = 0;
    int actual_movment = 0;
    function<void()> rotateFunctions[9];

    // * Movimiento de camada
    bool isMoving = false;

    float offx, offy, offz;
    int xMov = 0, yMov = 0, zMov = 0;
    int movCant = 50;

    float speed = (PI / 2) / 10;
    float acumAngle = 0.0f;
    int camada = 0;

    vector<int> centers = {4, 10, 12, 13, 14, 16, 22};

    cube();
    ~cube();
    void theScramble();
    void renderInit();
    void infoCentros();
    void render(ShaderClass);
    void printScramble();
    void rotateCamada_L();
    void rotateCamada_LI();
    void rotateCamada_Centro_Z();
    void rotateCamada_R();
    void rotateCamada_RI();
    void rotateCamada_D();
    void rotateCamada_DI();
    void rotateCamada_Centro_Y();
    void rotateCamada_U();
    void rotateCamada_UI();
    void rotateCamada_B();
    void rotateCamada_BI();
    void rotateCamada_Centro_X();
    void rotateCamada_F();
    void rotateCamada_FI();
    void rotateCamada();
    void rotate(vector<int>, float, char);
    string convertMovments_toString();
    vector<string> convert_String_to_Movments(string, int);
    void getSolution();
    void moveSolution();
    bool isColliding(float, float, float);
    void randMove();
    glm::vec3 getCenter();
};

#endif
