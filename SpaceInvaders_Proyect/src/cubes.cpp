#include "cubes.h"
#include "../Headers/TestingFunctions.h"

// --> Constructor
cube::cube(){
    // --> For drawing the cube
    int space = 10;
    offx = (rand() % space) - (int)(space / 2);
    offy = (rand() % space) - (int)(space / 2);
    offz = (rand() % space) - (int)(space / 2);

    // * Actualizamos los centros de los cubos
    for(int i = 0; i < 27; i++){
        cubes[i].calculateCenter();
    }

    int count = 0;
    for (int z = -1; z < 2; z += 1) {
        for (int y = -1; y < 2; y += 1) {
            for (int x = -1; x < 2; x += 1) {
                //printf("cube: %d \tx: %d, y: %d, z: %d\n", count, x, y, z);
                cubes[count].scale(0.9);
                cubes[count++].move(x + offx, y + offy, z + offz);
            }
        }
    }

    // --> For the scramble of the cube (random movements)
    int numberOfMovScramble = 4;
    for(int i = 0; i < numberOfMovScramble; i++){
        int random = rand() % 12;
        scramble.push_back(posibleMovments[random]);
    }
}

cube::~cube(){
    cout << "Cubo destruido en: " << offx << ", " << offy << ", " << offz << endl;
}

/*
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
*/

// --> Funciones
void 
cube::renderInit(){
    for(int i = 0; i < cantidadCubos; i++){
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
cube::infoCentros(){
    cout << "------------------------------ INFO CENTROS ------------------------------" << endl;
    for(int i = 0; i < 27; i++){
        cout << "Cubo N° " << i << ": ";
        cout << cubes[i].verticesCentrales[0] << ", " << cubes[i].verticesCentrales[1] << ", " << cubes[i].verticesCentrales[2] << endl;
    }
    cout << "-------------------------------------------------------------------------" << endl;
}

void 
cube::render(ShaderClass shp){
    // --> Verificacion de movimiento de camada
    //if(camada >= 1 && camada <= 9){
    //    isMoving = true;
    //    rotateCamada();
    //}

    // if(getSolucionActive == true && isSolving == false){
    //     getSolution();
    //     isSolving = true;
    //     //actual_movment = 0;
    //     scramble.clear();
    // }

    // if(getSolucionActive == true && isSolving == true && isSolving2 == false){
    //     moveSolution();
    //     //isSolving2 = true;
    // }

    for(unsigned int i = 0; i < cantidadCubos; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, cubes[i].center);
        shp.setMat4("model", model);

        glBindVertexArray(cubes[i].VAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubes[i].VBO);
        glBufferData(GL_ARRAY_BUFFER, cubes[i].sizeVertexes * sizeof(float), cubes[i].vertexes, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void 
cube::printScramble(){
    //cout << "Scramble (" << actual_movment << "): ";
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
            float originalVertex[4] = { originalRubickVertexes[i][j], 
                                        originalRubickVertexes[i][j + 1], 
                                        originalRubickVertexes[i][j + 2], 
                                        1.0f};

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

void 
cube::rotateCamada_L(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[0] > (-1.1f + offx) && cubes[i].verticesCentrales[0] < (-0.9f + offx)){
            cubes[i].rotate(speed, 'x');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'x');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("L");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_LI(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[0] > (-1.1f + offx) && cubes[i].verticesCentrales[0] < (-0.9f + offx)){
            cubes[i].rotate_inversa(speed, 'x');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'x');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("L'");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_R(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[0] > (0.9f + offx) && cubes[i].verticesCentrales[0] < (1.1f + offx)){
            cubes[i].rotate(speed, 'x');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'x');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("R");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_RI(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[0] > (0.9f + offx) && cubes[i].verticesCentrales[0] < (1.1f + offx)){
            cubes[i].rotate_inversa(speed, 'x');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'x');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("R'");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_U(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[1] > (0.9f + offy) && cubes[i].verticesCentrales[1] < (1.1f + offy)){
            cubes[i].rotate_inversa(speed, 'y');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'y');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("U");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_UI(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if(cubes[i].verticesCentrales[1] > (0.9f + offy) && cubes[i].verticesCentrales[1] < (1.1f + offy)){
            cubes[i].rotate(speed, 'y');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'y');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("U'");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_D(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[1] > (-1.1f + offy) && cubes[i].verticesCentrales[1] < (-0.9f + offy)){
            cubes[i].rotate(speed, 'y');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'y');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("D");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_DI(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[1] > (-1.1f + offy) && cubes[i].verticesCentrales[1] < (-0.9f + offy)){
            cubes[i].rotate_inversa(speed, 'y');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'y');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("D'");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_F(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[2] > (0.9f + offz) && cubes[i].verticesCentrales[2] < (1.1f + offz)){
            cubes[i].rotate_inversa(speed, 'z');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'z');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("F");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_FI(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[2] > (0.9f + offz) && cubes[i].verticesCentrales[2] < (1.1f + offz)){
            cubes[i].rotate(speed, 'z');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'z');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("F'");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_B(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[2] > (-1.1f + offz) && cubes[i].verticesCentrales[2] < (-0.9f + offz)){
            cubes[i].rotate(speed, 'z');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'z');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("B");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

void 
cube::rotateCamada_BI(int &actual_movment){
    std::vector<int> actualCamada;
    for(int i = 0; i < 27; i++){
        if( cubes[i].verticesCentrales[2] > (-1.1f + offz) && cubes[i].verticesCentrales[2] < (-0.9f + offz)){
            cubes[i].rotate_inversa(speed, 'z');
            //actualCamada.push_back(i);
        }
    }

    //rotate(actualCamada, speed, 'z');

    acumAngle += speed;
    if(acumAngle >= (PI / 2)){
        //camada = 0;
        acumAngle = 0;

        // * Actualizamos los centros de los cubos
        for(int i = 0; i < 27; i++){
            cubes[i].calculateCenter();
        }

        isMoving = false;
        actual_movment++;

        //scramble.push_back("B'");
        //cout << "Actual movment: " << actual_movment << endl;
    }
}

// * Solver Functions
string 
cube::convertMovments_toString(){
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

vector<string> 
cube::convert_String_to_MovmentsV2(string movments){
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

void 
cube::getSolution(){
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
    cout << "Numero de movimientos: " << number_movments << endl;
    cout << "Scramble: " << scramble_Aux << endl;

    cout << "Movimientos: ";
    for(int i = 0; i < movimientos_to_solve.size(); i++){
        cout << movimientos_to_solve[i];
    }
    cout << endl;
    cout << "-------------------------------------------------------------------------" << endl;
}

void 
cube::moveSolution(){
    if(movimientos_to_solve.size() == 0){
        isSolving = false;
        isSolving2 = false;
        getSolucionActive = false;
        isDead = true;
        return;
    }
    else if(actual_movment_solution == number_movments){
        isSolving = false;
        isSolving2 = false;
        getSolucionActive = false;
        isDead = true;
        cout << "Cube is dead" << endl;
        return;
    }

    string movimientoActual = movimientos_to_solve[actual_movment_solution];

    if(movimientoActual == "L"){
        rotateCamada_L(actual_movment_solution);
    }
    else if(movimientoActual == "L'"){
        rotateCamada_LI(actual_movment_solution);
    }
    else if(movimientoActual == "R"){
        rotateCamada_R(actual_movment_solution);
    }
    else if(movimientoActual == "R'"){
        rotateCamada_RI(actual_movment_solution);
    }
    else if(movimientoActual == "U"){
        rotateCamada_U(actual_movment_solution);
    }
    else if(movimientoActual == "U'"){
        rotateCamada_UI(actual_movment_solution);
    }
    else if(movimientoActual == "D"){
        rotateCamada_D(actual_movment_solution);
    }
    else if(movimientoActual == "D'"){
        rotateCamada_DI(actual_movment_solution);
    }
    else if(movimientoActual == "F"){
        rotateCamada_F(actual_movment_solution);
    }
    else if(movimientoActual == "F'"){
        rotateCamada_FI(actual_movment_solution);
    }
    else if(movimientoActual == "B"){
        rotateCamada_B(actual_movment_solution);
    }
    else if(movimientoActual == "B'"){
        rotateCamada_BI(actual_movment_solution);
    }
}

void
cube::moveScramble(){
    if(actual_movment_scramble == scramble.size()){
        isScrambled = true; 
        return;
    }

    string movimientoActual = scramble[actual_movment_scramble];

    if(movimientoActual == "L"){
        rotateCamada_L(actual_movment_scramble);
    }
    else if(movimientoActual == "L'"){
        rotateCamada_LI(actual_movment_scramble);
    }
    else if(movimientoActual == "R"){
        rotateCamada_R(actual_movment_scramble);
    }
    else if(movimientoActual == "R'"){
        rotateCamada_RI(actual_movment_scramble);
    }
    else if(movimientoActual == "U"){
        rotateCamada_U(actual_movment_scramble);
    }
    else if(movimientoActual == "U'"){
        rotateCamada_UI(actual_movment_scramble);
    }
    else if(movimientoActual == "D"){
        rotateCamada_D(actual_movment_scramble);
    }
    else if(movimientoActual == "D'"){
        rotateCamada_DI(actual_movment_scramble);
    }
    else if(movimientoActual == "F"){
        rotateCamada_F(actual_movment_scramble);
    }
    else if(movimientoActual == "F'"){
        rotateCamada_FI(actual_movment_scramble);
    }
    else if(movimientoActual == "B"){
        rotateCamada_B(actual_movment_scramble);
    }
    else if(movimientoActual == "B'"){
        rotateCamada_BI(actual_movment_scramble);
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

    //printf("Distance: %f\n", distance);

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
        //printf("randMove %d, %d, %d\n", xMov, yMov, zMov);
        
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
