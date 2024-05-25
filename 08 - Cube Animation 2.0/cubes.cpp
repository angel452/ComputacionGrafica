#include "cubes.h"
#include "Headers/TestingFunctions.h"

cube::cube()
{
    rotateFunctions[0] = [&]() { rotateCamada_L(); };
    rotateFunctions[1] = [&]() { rotateCamada_Centro_Z(); };
    rotateFunctions[2] = [&]() { rotateCamada_RI(); };
    rotateFunctions[3] = [&]() { rotateCamada_D(); };
    rotateFunctions[4] = [&]() { rotateCamada_Centro_Y(); };
    rotateFunctions[5] = [&]() { rotateCamada_UI(); };
    rotateFunctions[6] = [&]() { rotateCamada_B(); };
    rotateFunctions[7] = [&]() { rotateCamada_Centro_X(); };
    rotateFunctions[8] = [&]() { rotateCamada_FI(); };

    int space = 100;
    offx = (rand() % space) - (int)(space / 2);
    offy = (rand() % space) - (int)(space / 2);
    offz = (rand() % space) - (int)(space / 2);

    for(int i = 0; i < cantidadCubos; i++){
        cubes[i].calculateCenter();
    }

    int count = 0;
    for (int z = -1; z < 2; z += 1) {
        for (int y = -1; y < 2; y += 1) {
            for (int x = -1; x < 2; x += 1) {
                printf("cube: %d \tx: %d, y: %d, z: %d\n", count, x, y, z);
                cubes[count].scale(0.9);
                cubes[count++].move(x + offx, y + offy, z + offz);
            }
        }
    } 
}

cube::~cube()
{
    printf("Destructor\n");
}

void 
cube::theScramble() 
{
    float pastSpeed = speed;
    speed = (PI / 2);

    //std::vector<int> scrambleCombination = {0, 2, 3, 5, 6, 8, 2, 3, 5, 2, 0, 0, 0, 2, 6};
    std::vector<int> scrambleCombination = {0};

    for (int i: scrambleCombination) {
        camada = 1;
    }

    speed = pastSpeed;
}

void 
cube::renderInit()
{
    for (int i = 0; i < cantidadCubos; i++) {
        glGenVertexArrays(1, &cubes[i].VAO);
        glGenBuffers(1, &cubes[i].VBO);

        glBindVertexArray(cubes[i].VAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubes[i].VBO);
        glBufferData(GL_ARRAY_BUFFER, cubes[i].sizeVertexes * sizeof(float), cubes[i].vertexes, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0); 
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void 
cube::infoCentros()
{

    cout << "------------------------------ INFO CENTROS ------------------------------" << endl;
    for(int i = 0; i < 27; i++){
        cout << "Cubo N° " << i << ": ";
        cout << cubes[i].verticesCentrales[0] << ", " << cubes[i].verticesCentrales[1] << ", " << cubes[i].verticesCentrales[2] << endl;
    }
    cout << "-------------------------------------------------------------------------" << endl;
    
}

void 
cube::render(ShaderClass shp)
{
    // --> Verificacion de movimiento de camada
    if(camada >= 1 && camada <= 9){
        isMoving = true;
        rotateCamada();
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
        shp.setMat4("model", model);

        glBindVertexArray(cubes[i].VAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubes[i].VBO);
        glBufferData(GL_ARRAY_BUFFER, cubes[i].sizeVertexes * sizeof(float), cubes[i].vertexes, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void 
cube::printScramble()
{
    cout << "Scramble: ";
    for(int i = 0; i < scramble.size(); i++){
        cout << scramble[i] << " ";
    }
    cout << endl;
}

void
cube::rotate(vector<int> indexCamada, float angle, char axis)
{
    for (int i: indexCamada) {
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

        float *translationMatrix = new float[16];
        switch (axis) {
            case 'x':
                translationMatrix = rotationMatrixX;
                break;
            case 'y':
                translationMatrix = rotationMatrixY;
                break;
            case 'z':
                translationMatrix = rotationMatrixZ;
                break;
            default:
                exit(-1);
                break;
        }

        for (int j = 0; j < cubes[i].sizeVertexes; j += 6) {
            float originalVertex[4] = {originalRubickVertexes[i][j], originalRubickVertexes[i][j + 1], originalRubickVertexes[i][j + 2], 1.0f};

            float transformedVertex[4];
            cubes[i].matrixVectorMult(translationMatrix, originalVertex, transformedVertex);

            originalRubickVertexes[i][j] = transformedVertex[0];
            originalRubickVertexes[i][j + 1] = transformedVertex[1];
            originalRubickVertexes[i][j + 2] = transformedVertex[2];

            glm::vec3 mCentro = getCenter();

            cubes[i].vertexes[j] = transformedVertex[0] + mCentro[0];
            cubes[i].vertexes[j + 1] = transformedVertex[1] + mCentro[1];
            cubes[i].vertexes[j + 2] = transformedVertex[2] + mCentro[2];
        }

        cubes[i].calculateCenter();
    }
}

// * Rotaciones de camada
void 
cube::rotateCamada_L()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if (cubes[i].verticesCentrales[0] > (-1.1f + offx) && cubes[i].verticesCentrales[0] < (-0.9f + offx)) {
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'x');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;
        scramble.push_back("L");
        printScramble();
    }
}

void 
cube::rotateCamada_LI()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[0] > (-1.1f + offx) && cubes[i].verticesCentrales[0] < (-0.9f + offx)) {
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'x');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("L'");
        printScramble();
    }
}

void
cube::rotateCamada_R()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[0] > (0.9f + offx) && cubes[i].verticesCentrales[0] < (1.1f + offx)){
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'x');

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

void 
cube::rotateCamada_RI()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[0] > (0.9f + offx) && cubes[i].verticesCentrales[0] < (1.1f + offx)){
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'x');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("R'");
        printScramble();
    }
}

void
cube::rotateCamada_U()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[1] > (0.9f + offy) && cubes[i].verticesCentrales[1] < (1.1f + offy)) {
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'y');

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

void 
cube::rotateCamada_UI() 
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre 0.9 y 1.1, entonces es la camada frente
        if(cubes[i].verticesCentrales[1] > (0.9f + offy) && cubes[i].verticesCentrales[1] < (1.1f + offy)) {
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'y');

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

void 
cube::rotateCamada_D()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[1] > (-1.1f + offy) && cubes[i].verticesCentrales[1] < (-0.9f + offy)){
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'y');

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

void 
cube::rotateCamada_DI()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[1] > (-1.1f + offy) && cubes[i].verticesCentrales[1] < (-0.9f + offy)) {
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'y');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        camada = 0;
        acumAngle = 0;

        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        scramble.push_back("D'");
        printScramble();
    }
}

void 
cube::rotateCamada_F() 
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[2] > (0.9f + offz) && cubes[i].verticesCentrales[2] < (1.1f + offz)) {
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'z');

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

void 
cube::rotateCamada_FI()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[2] > (0.9f + offz) && cubes[i].verticesCentrales[2] < (1.1f + offz)){
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'z');
    
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

void 
cube::rotateCamada_B()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[2] > (-1.1f + offz) && cubes[i].verticesCentrales[2] < (-0.9f + offz)){
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'z');

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

void 
cube::rotateCamada_BI()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[2] > (-1.1f + offz) && cubes[i].verticesCentrales[2] < (-0.9f + offz)){
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'z');

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
void 
cube::rotateCamada_Centro_Z(){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        // * Verificamos que si el centro esta entre -0.1 y 0.1, entonces es la camada central
        if(cubes[i].verticesCentrales[0] > (-0.1f + offx) && cubes[i].verticesCentrales[0] < (0.1f + offx)) {
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'x');

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

void 
cube::rotateCamada_Centro_Y()
{
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[1] > (-0.1f + offy) && cubes[i].verticesCentrales[1] < (0.1f + offy)) {
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'y');

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

void 
cube::rotateCamada_Centro_X(){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[2] > (-0.1f + offz) && cubes[i].verticesCentrales[2] < (0.1f + offz)) {
            actualCamada.push_back(i);
        }
    }

    rotate(actualCamada, speed, 'z');

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

void 
cube::rotateCamada() {
    switch (camada) {
        case 1: 
            rotateCamada_L();
            break;

        case 2: 
            rotateCamada_Centro_Z();
            break;

        case 3: 
            rotateCamada_RI();
            break;

        case 4:
            rotateCamada_D();
            break;

        case 5:
            rotateCamada_Centro_Y();
            break;

        case 6:
            rotateCamada_UI();
            break;

        case 7:
            rotateCamada_B();
            break;

        case 8:
            rotateCamada_Centro_X();
            break;

        case 9:
            rotateCamada_FI();
            break;
    }
}

// * Solver Functions
string 
cube::convertMovments_toString() {
    string movments = "";
    for(int i = 0; i < scramble.size(); i++){
        movments += scramble[i];
    }
    return movments;
}

vector<string> 
cube::convert_String_to_Movments(string movments, int N_movments){
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

void 
cube::getSolution() {
    // --> Convertimos el scramble a un string
    string scramble;
    scramble = convertMovments_toString();
    
    // --> Creamos el cubo
    Cube Cube1;
    Cube1.SetScramble(scramble);
    Cube1.PrintCube();
    
    string solve;
    int movimientos;
    solve = SolveCube(Cube1, "CMD");
    movimientos = Cube1.SetScramble(solve);

    std::cout << "Movess: " << movimientos << '\n';
    std::cout << "Solutions: " << solve << '\n';
    cout << endl;
    //Cube1.PrintCube();
    Cube1.PrintBoolIsSolved();
    cout << endl << endl << endl;

    // --> Convertimos el resultado a un vector de strings
    movimientos_to_solve = convert_String_to_Movments(solve, movimientos);
    number_movments = movimientos_to_solve.size();

    // --> Imprimimos los movimientos
    cout << "------------------------------ INFO SOLUCION ------------------------------" << endl;
    cout << "Movimientos: ";
    for(int i = 0; i < movimientos_to_solve.size(); i++){
        cout << movimientos_to_solve[i] << " ";
    }
    cout << endl;
    cout << "-------------------------------------------------------------------------" << endl;
}

void 
cube::moveSolution(){
    if (movimientos_to_solve.size() == 0) {
        isSolving = isSolving2 = getSolucionActive = 0;
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

bool
cube::isColliding(float x, float y, float z)
{
    float distance = std::sqrt(
            std::pow(x - cubes[13].verticesCentrales.x, 2) +
            std::pow(y - cubes[13].verticesCentrales.y, 2) +
            std::pow(z - cubes[13].verticesCentrales.z, 2)
            );

    printf("Distance: %f\n", distance);

    return distance <= 3;;
}

void 
cube::randMove() 
{
    if (movCant < 0) {
        xMov = rand() % 20;
        yMov = rand() % 20;
        zMov = rand() % 20;
        movCant = rand() % 100 + 20;
    }

    for (int i = 0; i < cantidadCubos; i++) {
        printf("randMove %d, %d, %d\n", xMov, yMov, zMov);
        
        cubes[i].move((0.1 * (xMov > 10 ? 1 : -1)), (0.1 * (yMov > 10 ? 1 : -1)), (0.1 * (zMov > 10 ? 1 : -1)));
    }

    offx = cubes[13].verticesCentrales[0];
    offy = cubes[13].verticesCentrales[1];
    offz = cubes[13].verticesCentrales[2];

    movCant--;
}

glm::vec3
cube::getCenter()
{
    glm::vec3 centro = glm::vec3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < cantidadCubos; i ++){
        for (int j = 0; j < cubes[i].sizeVertexes; j += 6) {
            centro.x += cubes[i].vertexes[j];
            centro.y += cubes[i].vertexes[j + 1];
            centro.z += cubes[i].vertexes[j + 2];
        }
    }

    centro.x /= ((216.0f / 6.0f) * cantidadCubos);
    centro.y /= ((216.0f / 6.0f) * cantidadCubos);
    centro.z /= ((216.0f / 6.0f) * cantidadCubos);

    return centro;
}
