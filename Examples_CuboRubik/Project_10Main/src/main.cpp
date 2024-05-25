
//#define GLAD_GL_IMPLEMENTATION
//#include <glad/glad.h>
//#include <glad/gl.h>
//#define GLFW_INCLUDE_NONE
#include "glad.h"

#include <GLFW/glfw3.h>

//#include "linmath.h"
#include "deps/linmath.h"

//#include "shader.h"
#include <filesystem>

#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <vector>
#include <fstream>
#include <cstdlib> 
#include <ctime> 

//#include "Header.h"
//#include "solve.h"
//#include "random.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<unistd.h>
#include<string>



//Alumno: Paolo Rafael Delgado Vidal
//		  Fabian Concha Sifuentes

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 camUnif;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 proj;"
"void main()\n"
"{\n"
"   gl_Position = camUnif * vec4(aPos, 1.0f)\n;"
//"	camUnif = camUnif * vec;"
"}\0";


const char* fragmentShaderSource = "#version 330 core\n"
"layout(location=0) out vec4 FragColor;\n"
"uniform vec4 u_color;"
"void main()\n"
"{\n"
"   FragColor=u_color;\n"
"}\n\0";


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


char accion = 'g';
string movimientos = "";
vector<string> cambios;

string solution(string moves){
	int len = moves.length();
	string sol = "";
	moves+=" ";
	for(int i=0;i<len;i++){
		if (moves[i] == ' ') continue;
		if (moves[i+1] == '\''){
			if (moves[i] == 'U') sol+="n";
			else if (moves[i] == 'D') sol+="j";
			else if (moves[i] == 'R') sol+="t";
			else if (moves[i] == 'L') sol+="k";
			else if (moves[i] == 'F') sol+="b";
			else if (moves[i] == 'B') sol+="u";
			i+=1;
		}
		else if (moves[i+1] == '2'){
			if(moves[i+2] == '\''){
			    if (moves[i] == 'U') sol+="nn";
				else if (moves[i] == 'D') sol+="jj";
				else if (moves[i] == 'R') sol+="tt";
				else if (moves[i] == 'L') sol+="kk";
				else if (moves[i] == 'F') sol+="bb";
				else if (moves[i] == 'B') sol+="uu";
				i+=1;
			}
			else{
				if (moves[i] == 'U') sol+="hh";
				else if (moves[i] == 'D') sol+="mm";
				else if (moves[i] == 'R') sol+="ll";
				else if (moves[i] == 'L') sol+="rr";
				else if (moves[i] == 'F') sol+="ii";
				else if (moves[i] == 'B') sol+="vv";
			}
			i+=1;
		}
		else{
			if (moves[i] == 'U') sol+="h";
			else if (moves[i] == 'D') sol+="m";
			else if (moves[i] == 'R') sol+="l";
			else if (moves[i] == 'L') sol+="r";
			else if (moves[i] == 'F') sol+="i";
			else if (moves[i] == 'B') sol+="v";
		}
		//cout<<i<<" ";
	}
	return sol;
}

struct Transformaciones {
    float rot_x[4][4], rot_y[4][4], rot_z[4][4], T[4][4], S[4][4], lookat[4][4];
	float invrot_x[4][4], invrot_y[4][4], invrot_z[4][4], invT[4][4], invS[4][4];
    Transformaciones() {
        float temp[4][4] = { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,1} };
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                rot_x[i][j] = temp[i][j];
                rot_y[i][j] = temp[i][j];
                rot_z[i][j] = temp[i][j];
				invrot_x[i][j] = temp[i][j];
                invrot_y[i][j] = temp[i][j];
                invrot_z[i][j] = temp[i][j];
                T[i][j] = temp[i][j];
                S[i][j] = temp[i][j];
				invT[i][j] = temp[i][j];
                invS[i][j] = temp[i][j];
				lookat[i][j] = temp[i][j];
            }
        }
    }
    
    float* multiplicacion(float vertice[3], float transformacion[4][4], bool vector) {
        float temp[4][1];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 1; j++) {
                temp[i][j] = 0;
            }
        }
        float new_vertice[4][1];
        for (int i = 0; i < 3; i++) {
            new_vertice[i][0] = vertice[i];
        }
        if (!vector) {new_vertice[3][0] = 0;}
        else {new_vertice[3][0] = 1;}

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 1; j++) {
                for (int k = 0; k < 4; k++) {
                    temp[i][j] += transformacion[i][k] * new_vertice[k][j];
                }
            }
        }
        for (int i = 0; i < 3; i++) {
            vertice[i] = temp[i][0];
        }
        return vertice;
    }
    
    void rotx(float angulo, float* &vertices, int size) {
       
        float valor_radian = angulo * (3.141592653589793238463 / 180);
        float temp[3];
        float* res = new float[3];
        
        rot_x[0][0] = 1;
        rot_x[1][1] = cos(valor_radian); rot_x[1][2] = -sin(valor_radian);
        rot_x[2][1] = sin(valor_radian); rot_x[2][2] = cos(valor_radian);
       
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, rot_x, 0);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
    
	void invrotx(float angulo, float* &vertices, int size) {
       
        float valor_radian = angulo * (3.141592653589793238463 / 180);
        float temp[3];
        float* res = new float[3];
        
        invrot_x[0][0] = 1;
        invrot_x[1][1] = cos(valor_radian); invrot_x[1][2] = sin(valor_radian);
        invrot_x[2][1] = -sin(valor_radian); invrot_x[2][2] = cos(valor_radian);
       
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, invrot_x, 0);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
	
    void roty(float angulo, float* &vertices, int size) {
        float valor_radian = angulo * (3.141592653589793238463 / 180);
        float temp[3];
        float* res = new float[3];
		
        rot_y[0][0] = cos(valor_radian); rot_y[0][2] = sin(valor_radian);
        rot_y[1][1] = 1;
        rot_y[2][0] = -sin(valor_radian); rot_y[2][2] = cos(valor_radian);
		
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, rot_y, 0);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
	
	void invroty(float angulo, float* &vertices, int size) {
        float valor_radian = angulo * (3.141592653589793238463 / 180);
        float temp[3];
        float* res = new float[3];
		
        invrot_y[0][0] = cos(valor_radian); invrot_y[0][2] = -sin(valor_radian);
        invrot_y[1][1] = 1;
        invrot_y[2][0] = sin(valor_radian); invrot_y[2][2] = cos(valor_radian);
		
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, invrot_y, 1);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
    
    void rotz(float angulo, float* &vertices, int size) {
        float valor_radian = angulo * (3.141592653589793238463 / 180);
        float temp[3];
        float* res = new float[3];
		
        rot_z[0][0] = cos(valor_radian); rot_z[0][1] = -sin(valor_radian);
        rot_z[1][0] = sin(valor_radian); rot_z[1][1] = cos(valor_radian);
        rot_z[2][2] = 1;
		
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, rot_z, 0);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
    
	void invrotz(float angulo, float* &vertices, int size) {
        float valor_radian = angulo * (3.141592653589793238463 / 180);
        float temp[3];
        float* res = new float[3];
		
        invrot_z[0][0] = cos(valor_radian); invrot_z[0][1] = sin(valor_radian);
        invrot_z[1][0] = -sin(valor_radian); invrot_z[1][1] = cos(valor_radian);
        invrot_z[2][2] = 1;
		
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, invrot_z, 0);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
	
    void translacion(float tx, float ty, float tz, float* &vertices, int size) {
        float temp[3];
        float* res = new float[3];
		
        T[0][0] = 1; T[0][3] = tx;
        T[1][1] = 1; T[1][3] = ty;
        T[2][2] = 1; T[2][3] = tz;
		
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, T, 1);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
	
	void invtranslacion(float tx, float ty, float tz, float* &vertices, int size) {
        float temp[3];
        float* res = new float[3];
		
        invT[0][0] = 1; invT[0][3] = -tx;
        invT[1][1] = 1; invT[1][3] = -ty;
        invT[2][2] = 1; invT[2][3] = -tz;
		
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, invT, 1);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
    
    void escala(float sx, float sy, float sz, float* &vertices, int size) {
        float temp[3];
        float* res = new float[3];
		
        S[0][0] = sx;
        S[1][1] = sy;
        S[2][2] = sz;
		
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, S, 0);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
	
	void invescala(float sx, float sy, float sz, float* &vertices, int size) {
        float temp[3];
        float* res = new float[3];
		
        invS[0][0] = 1/sx;
        invS[1][1] = 1/sy;
        invS[2][2] = 1/sz;
		
        for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, invS, 0);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
    }
	
	void perspectiva (float Pax, float Pay, float Paz, float* &vertices, int size)
	{
		float temp[3];
        float* res = new float[3];
		
		float cx = 0, cy = 0, cz = 0;
		
		float 	ax = Pax-cx,
				ay = Pay-cy,
				az = Paz - cz;
		
		float u_norm = sqrt(pow(ax,2) + pow(ay,2) + pow(az,2));
		float 	ux = ax/u_norm, 
				uy = ay/u_norm, 
				uz = az/u_norm;
		
		float bx = 0.0, by = 1.0, bz = 0.0;
		
		float 	v1x = by * uz - uy * bz, 
				v1y = bz * ux - uz * bx, 
				v1z = bx * uy - ux * by;
				
		float v_norm = sqrt(pow(v1x,2) + pow(v1y,2) + pow(v1z,2));
				
		float 	vx = v1x/v_norm, 
				vy = v1y/v_norm, 
				vz = v1z/v_norm;
		
		float wx,wy,wz;
		wx = uy * vz - vy * uz;
		wy = uz * vx - vz * ux;
		wz = ux * vy - vx * uy;
		
		//Se arma la matriz
		lookat[0][0] = ux; 	lookat[0][1] = uy; 	lookat[0][2] = uz; 	lookat[0][3] = (-ux*Pax - uy*Pay - uz*Paz); 
		lookat[1][0] = vx; 	lookat[1][1] = vy; 	lookat[1][2] = vz;	lookat[1][3] = (-vx*Pax - vy*Pay - vz*Paz); 
		lookat[2][0] = wx; 	lookat[2][1] = wy; 	lookat[2][2] = wz;	lookat[2][3] = (-wx*Pax - wy*Pay - wz*Paz); 
		lookat[3][0] = 0.0; lookat[3][1] =0.0; 	lookat[3][2] = 0.0;	lookat[3][3] = 1.0; 
		
		//Multiplicacion de matriz con vertices
		for (int i = 0; i < size; i += 3) {
            temp[0] = vertices[i];
            temp[1] = vertices[i + 1];
            temp[2] = vertices[i + 2];
            res = multiplicacion(temp, lookat, 0);
            vertices[i] = res[0];
            vertices[i + 1] = res[1];
            vertices[i + 2] = res[2];
        }
	}
};

struct figura {
    float *vertices;
    unsigned int *indicesT, *indicesL;
    int vSize, iTSize, iLSize;
    unsigned int VBO, EBO1, EBO2;
    Transformaciones Transform;
	
	figura():vertices(NULL),indicesT(NULL),indicesL(NULL),vSize(0),iTSize(0),iLSize(0),VBO(0),EBO1(0),EBO2(0)
    {}
	
    figura(unsigned int VBO, unsigned int EBO1, unsigned int EBO2) :vertices(NULL), indicesT(NULL), indicesL(NULL), vSize(0), iTSize(0), iLSize(0)
    {
        this->VBO = VBO;
        this->EBO1 = EBO1;
        this->EBO2 = EBO2;
    }
    
	void set_figura(unsigned int VBO, unsigned int EBO1, unsigned int EBO2) 
    {
        this->VBO = VBO;
        this->EBO1 = EBO1;
        this->EBO2 = EBO2;
		this->vertices = NULL;
		this->indicesT = NULL;
		this->indicesL = NULL;
		this->vSize = 0; 
		this->iTSize = 0; 
		this->iLSize = 0;
    }
	
    void Mvertices(int size, float* vertices) {
        vSize = size;
        this->vertices = new float[vSize];
        this->vertices = vertices;
        this->VBO = VBO;
    }
    
    void Ilineas(int size, unsigned int* indices) {
        iLSize = size;
        this->indicesL = new unsigned int[iLSize];
        this->indicesL = indices;
        this->EBO1 = EBO1;
    }
    
    void Itriangulos(int size, unsigned int* indices) {
        iTSize = size;
        this->indicesT = new unsigned int[iTSize];
        this->indicesT = indices;
        this->EBO2 = EBO2;
    }
    
    void buffers() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize*sizeof(float), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, iLSize* sizeof(int), indicesL, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, iTSize* sizeof(int), indicesT, GL_STATIC_DRAW);
    }
	
	void print(){
		for(int i = 0;i<vSize;i+=3){
			cout<<vertices[i]<<" "<<vertices[i+1]<<" "<<vertices[i+2]<<endl;
		}
	}
		
    void rot_x(float angulo) { Transform.rotx(angulo, vertices, vSize); }
    void rot_y(float angulo) { Transform.roty(angulo, vertices, vSize); }
    void rot_z(float angulo) { Transform.rotz(angulo, vertices, vSize); }
	void invrot_x(float angulo) { Transform.invrotx(angulo, vertices, vSize); }
    void invrot_y(float angulo) { Transform.invroty(angulo, vertices, vSize); }
    void invrot_z(float angulo) { Transform.invrotz(angulo, vertices, vSize); }
    void traslation(float tx, float ty, float tz) { Transform.translacion(tx, ty, tz, vertices, vSize); }
	void invtraslation(float tx, float ty, float tz) { Transform.translacion(tx, ty, tz, vertices, vSize); }
    void scale(float sx, float sy, float sz) { Transform.escala(sx, sy, sz, vertices, vSize); }
	void invscale(float sx, float sy, float sz) { Transform.invescala(sx, sy, sz, vertices, vSize); }
	void movcam(float Pax, float Pay, float Paz) {Transform.perspectiva(Pax,Pay,Paz,vertices, vSize); }
};
	
	void buff_circulo(int angulo, figura &circulo);
	void create_figure(figura& temp, int location);
	void create_only_lines(figura &temp, int location,GLfloat r,GLfloat g,GLfloat b);
	void create_only_triangulos(figura &temp, int location,GLfloat r,GLfloat g,GLfloat b);
	void create_only_points(figura &temp, int location,GLfloat r,GLfloat g,GLfloat b);

class pieza{
		
		float *vertices;
		figura caras[6];
		int vSize=24, iTSize = 6,iLsize=5, location,ebo_ini;
		unsigned int VBO, *EBO;
		unsigned int itc1[6] = {0,1,2,0,3,2};//Cara1
		unsigned int ilc1[5] = {0,1,2,3,0};
		unsigned int itc2[6] = {4,5,6,4,7,6};//Cara2
		unsigned int ilc2[5] = {4,5,6,7,4};
		unsigned int itc3[6] = {7,4,0,7,3,0};//Cara3
		unsigned int ilc3[5] = {4,0,3,7,4};
		unsigned int itc4[6] = {6,5,1,6,2,1};//Cara4
		unsigned int ilc4[5] = {5,1,2,6,5};
		unsigned int itc5[6] = {0,4,5,0,1,5};//Cara5
		unsigned int ilc5[5] = {4,5,1,0,4};
		unsigned int itc6[6] = {7,3,2,7,6,2};//Cara6
		unsigned int ilc6[5] = {3,2,6,7,3};
		
		public:
		
		pieza(float *vert,int locat,unsigned int VBO, unsigned int *EBO,int ini){
			this->vertices = vert;
			this->location = locat;
			this->VBO = VBO;
			this->EBO = EBO;
		    this->ebo_ini = ini;
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glBufferData(GL_ARRAY_BUFFER, vSize*sizeof(float), vertices, GL_STATIC_DRAW);
		}
		
		pieza(){
			this->vertices = NULL;
			this->location = 0;
			this->VBO = 0;
			this->EBO = 0;
		    this->ebo_ini = 0;
		}
		
		void set_pieza(float *vert,int locat,unsigned int VBO, unsigned int *EBO,int ini){
			this->vertices = vert;
			this->location=locat;
			this->VBO = VBO;
			this->EBO = EBO;
		    this->ebo_ini = ini;
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glBufferData(GL_ARRAY_BUFFER, vSize*sizeof(float), vertices, GL_STATIC_DRAW);
		}
		
		void construir(){
			caras[0].set_figura(VBO,EBO[ebo_ini],EBO[ebo_ini+1]); caras[0].Mvertices(vSize,vertices);
			caras[0].Ilineas(iLsize,ilc1); caras[0].Itriangulos(iTSize,itc1);
			caras[0].buffers();
			caras[1].set_figura(VBO,EBO[ebo_ini+2],EBO[ebo_ini+3]); caras[1].Mvertices(vSize,vertices);
			caras[1].Ilineas(iLsize,ilc2); caras[1].Itriangulos(iTSize,itc2);
			caras[1].buffers();
			caras[2].set_figura(VBO,EBO[ebo_ini+4],EBO[ebo_ini+5]); caras[2].Mvertices(vSize,vertices);
			caras[2].Ilineas(iLsize,ilc3); caras[2].Itriangulos(iTSize,itc3);
			caras[2].buffers();
			caras[3].set_figura(VBO,EBO[ebo_ini+6],EBO[ebo_ini+7]); caras[3].Mvertices(vSize,vertices);
			caras[3].Ilineas(iLsize,ilc4); caras[3].Itriangulos(iTSize,itc4);
			caras[3].buffers();
			caras[4].set_figura(VBO,EBO[ebo_ini+8],EBO[ebo_ini+9]); caras[4].Mvertices(vSize,vertices);
			caras[4].Ilineas(iLsize,ilc5); caras[4].Itriangulos(iTSize,itc5);
			caras[4].buffers();
			caras[5].set_figura(VBO,EBO[ebo_ini+10],EBO[ebo_ini+11]); caras[5].Mvertices(vSize,vertices);
			caras[5].Ilineas(iLsize,ilc6); caras[5].Itriangulos(iTSize,itc6);
			caras[5].buffers();
		}
		
		void buffs(){
			for(int i=0;i<6;i++) caras[i].buffers();
		}
		
		void dibujar(){
			glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f);	// BLANCO
			create_figure(caras[0],location);			 	// BACK
			glUniform4f(location, 1.0f, 1.0f, 0.0f, 1.0f);	// AMARILLO
			create_figure(caras[1],location);				//FRONT
			glUniform4f(location, 0.0f, 0.0f, 1.0f, 1.0f);	//AZUL
			create_figure(caras[2],location);				//TOP
			glUniform4f(location, 0.0f, 0.5f, 0.0f, 1.0f);	//VERDE
			create_figure(caras[3],location);				//DOWN
			glUniform4f(location, 1.0f, 0.5f, 0.0f, 1.0f);	//NARANJA
			create_figure(caras[4],location);				//RIGHT
			glUniform4f(location, 1.0f, 0.0f, 0.0f, 1.0f);	//ROJO
			create_figure(caras[5],location);				//LEFT
			
		}
		
		void trasladar(float tx, float ty, float tz){
			caras[0].traslation(tx,ty,tz);
			caras[1].traslation(tx,ty,tz);
			caras[2].traslation(tx,ty,tz);
			caras[3].traslation(tx,ty,tz);
			caras[4].traslation(tx,ty,tz);
			caras[5].traslation(tx,ty,tz);
		}
		
		void rotarx(float angulo){
			caras[0].rot_x(angulo);
			caras[1].rot_x(angulo);
			caras[2].rot_x(angulo);
			caras[3].rot_x(angulo);
			caras[4].rot_x(angulo);
			caras[5].rot_x(angulo);
		}
		
		void rotary(float angulo){
			caras[0].rot_y(angulo);
			caras[1].rot_y(angulo);
			caras[2].rot_y(angulo);
			caras[3].rot_y(angulo);
			caras[4].rot_y(angulo);
			caras[5].rot_y(angulo);
		}
		
		void rotarz(float angulo){
			caras[0].rot_z(angulo);
			caras[1].rot_z(angulo);
			caras[2].rot_z(angulo);
			caras[3].rot_z(angulo);
			caras[4].rot_z(angulo);
			caras[5].rot_z(angulo);
		}
		
		void pov(float pax, float pay, float paz)
		{
			caras[0].movcam(pax, pay, paz);
			caras[1].movcam(pax, pay, paz);
			caras[2].movcam(pax, pay, paz);
			caras[3].movcam(pax, pay, paz);
			caras[4].movcam(pax, pay, paz);
			caras[5].movcam(pax, pay, paz);
		}
		
		void set_color(int location){
			this->location = location;
			glUniform4f(location, 1.0f, 0.5f, 0.2f, 1.0f);	
		}
		
		void imprimir(){
			for(int i = 0;i<24;i+=3){
				cout<<vertices[i]<<" "<<vertices[i+1]<<" "<<vertices[i+2]<<endl;
			}
		}
};
	
	
class camada{
		public:
		pieza *cubitos;
		int indices_top[3];
		int indices_mid[3];
		int indices_bot[3];
		int indices_aux[3];
		int transform[3][3];
		int aux[3][3];
		int transform_inv[3][3];
		
		camada(pieza *rubik, int t1,int t2,int t3,int m1,int m2,int m3,int b1,int b2,int b3){
			this->cubitos = rubik;
			this->indices_top[0] = t1;
			this->indices_top[1] = t2;
			this->indices_top[2] = t3;
			this->indices_mid[0] = m1;
			this->indices_mid[1] = m2;
			this->indices_mid[2] = m3;
			this->indices_bot[0] = b1;
			this->indices_bot[1] = b2;
			this->indices_bot[2] = b3;
			cubitos[indices_mid[1]].imprimir();
		}
		
		void define_trans(){
			for(int i = 0;i<3;i++){
				this->transform[0][i] = indices_top[i];
				this->transform[1][i] = indices_mid[i];
				this->transform[2][i] = indices_bot[i];
			}
		}
		
		void gira_izquierda(){
			int t = 0;
			for(int i = 3-1; i >= 0; --i){
				for(int j = 0; j < 3; j++){
					aux[t][j] = transform[j][i];
				}
				t++;
			}
			
			for(int i = 0;i<3;i++){
				for(int j=0;j<3;j++){
					transform[i][j] = aux[i][j];
				}
			}
		}
		
		void gira(){
			for(int i = 0;i<3;i++){
				this->transform[i][0] = indices_top[i];
				this->transform[i][1] = indices_mid[i];
				this->transform[i][2] = indices_bot[i];
			}
		}
		
		void define_inv(){
			for(int i = 0;i<3;i++){
				transform[2][i] = indices_top[i];
				transform[1][i] = indices_mid[i];
				transform[0][i] = indices_bot[i];
			}
		}
		
		void print_index(){
		
			cout<<indices_top[0]<<" "<<indices_top[1]<<" "<<indices_top[2]<<endl;
			cout<<indices_mid[0]<<" "<<indices_mid[1]<<" "<<indices_mid[2]<<endl;
			cout<<indices_bot[0]<<" "<<indices_bot[1]<<" "<<indices_bot[2]<<endl;
		}
		
		
		void update_top(int t1,int t2,int t3){
			this->indices_top[0] = t1;
			this->indices_top[1] = t2;
			this->indices_top[2] = t3;
		}
		
		void update_bot(int b1,int b2,int b3){
			this->indices_bot[0] = b1;
			this->indices_bot[1] = b2;
			this->indices_bot[2] = b3;
		}
		
		void update_left(int t2, int m2, int b2){
			this->indices_top[0] = t2;
			this->indices_mid[0] = m2;
			this->indices_bot[0] = b2;
		}
		
		void update_right(int t3, int m3, int b3){
			this->indices_top[2] = t3;
			this->indices_mid[2] = m3;
			this->indices_bot[2] = b3;
		}
		
		void update_indices(int t1,int t2,int t3,int m1,int m2,int m3,int b1,int b2,int b3){
			this->indices_top[0] = t1;
			this->indices_top[1] = t2;
			this->indices_top[2] = t3;
			this->indices_mid[0] = m1;
			this->indices_mid[1] = m2;
			this->indices_mid[2] = m3;
			this->indices_bot[0] = b1;
			this->indices_bot[1] = b2;
			this->indices_bot[2] = b3;
		}
		
		void rotar_horizontal_r(float angle){
			
			//define_trans();
			
			for (int i =0;i<3;i++){
				cubitos[indices_top[i]].rotarx(7.5);
				cubitos[indices_mid[i]].rotarx(7.5);
				cubitos[indices_bot[i]].rotarx(7.5);
				cubitos[indices_top[i]].buffs();
				cubitos[indices_mid[i]].buffs();
				cubitos[indices_bot[i]].buffs();
			}
			cout<<"vertical:"<<endl;print_index();
			//indices_top[0] = transform[2][0] ;//medio
			//indices_top[1] = transform[1][0];
			//indices_top[2] = transform[0][0];
			
			//indices_mid[0] = transform[2][1];//medio
			//indices_mid[1] = transform[1][1];
			//indices_mid[2] = transform[0][1];
			
			//indices_bot[0] = transform[2][2];//medio
			//indices_bot[1] = transform[1][2];
			//indices_bot[2] = transform[0][2];
			
			//define_trans();
		}
		
		void rotar_vertical_f(float angle){
			for (int i =0;i<3;i++){
				cubitos[indices_top[i]].rotary(angle);
				cubitos[indices_mid[i]].rotary(angle);
				cubitos[indices_bot[i]].rotary(angle);
				cubitos[indices_top[i]].buffs();
				cubitos[indices_mid[i]].buffs();
				cubitos[indices_bot[i]].buffs();
			}
			cout<<"a ";
		}
		
		void rotar_vertical(float angle){
			for (int i =0;i<3;i++){
				cubitos[indices_top[i]].rotarz(angle);
				cubitos[indices_mid[i]].rotarz(angle);
				cubitos[indices_bot[i]].rotarz(angle);
				cubitos[indices_top[i]].buffs();
				cubitos[indices_mid[i]].buffs();
				cubitos[indices_bot[i]].buffs();
				
			}
		}
		
		void rotar_horizontal_ra(float angle){
			for (int i =0;i<3;i++){
				cubitos[indices_top[i]].rotarx(angle);
				cubitos[indices_mid[i]].rotarx(angle);
				cubitos[indices_bot[i]].rotarx(angle);
				cubitos[indices_top[i]].buffs();
				cubitos[indices_mid[i]].buffs();
				cubitos[indices_bot[i]].buffs();
			}
		}
		
		void rotar_vertical_fa(float angle){
			for (int i =0;i<3;i++){
				cubitos[indices_top[i]].rotary(angle);
				cubitos[indices_mid[i]].rotary(angle);
				cubitos[indices_bot[i]].rotary(angle);
				cubitos[indices_top[i]].buffs();
				cubitos[indices_mid[i]].buffs();
				cubitos[indices_bot[i]].buffs();
			}
			//cubitos[indices_mid[1]].imprimir();
		}
		
		void rotar_vertical_a(float angle){
			for (int i =0;i<3;i++){
				cubitos[indices_top[i]].rotarz(angle);
				cubitos[indices_mid[i]].rotarz(angle);
				cubitos[indices_bot[i]].rotarz(angle);
				cubitos[indices_top[i]].buffs();
				cubitos[indices_mid[i]].buffs();
				cubitos[indices_bot[i]].buffs();
			}
		}
		
		void update(){
			int aux[6]; aux[0] = indices_top[0];
			aux[1] = indices_top[1]; aux[2] = indices_top[2];
			aux[3] = indices_mid[0]; aux[4] = indices_bot[0];
			
			indices_top[0] = indices_top[2];
			indices_top[1] = indices_mid[2];
			indices_top[2] = indices_bot[2];
			
			indices_mid[0] = aux[1];
			//indices_mid[1] = indices_mid[1];
			indices_mid[2] = indices_bot[1];
			
			indices_bot[0] = aux[0];
			indices_bot[1] = aux[3];
			indices_bot[2] = aux[4];
			print_index();cout<<"-----------"<<endl;
		}
		
		void update_inv(){
		
			int aux[6]; 
			aux[0] = indices_top[0];
			aux[1] = indices_top[1]; 
			aux[2] = indices_top[2];
			aux[3] = indices_mid[2]; 
			aux[4] = indices_bot[2];
			
			indices_top[0] = indices_bot[0];
			indices_top[1] = indices_mid[0];//medio
			indices_top[2] = aux[0];
			
			indices_mid[0] = indices_bot[1];
			//indices_mid[1] = indices_mid[1];//medio
			indices_mid[2] = aux[1];
			
			indices_bot[0] = aux[4];
			indices_bot[1] = aux[3];//medio
			indices_bot[2] = aux[2];
			print_index();cout<<"-----------"<<endl;
		}
		
		
	};

class shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor that build the Shader Program from 2 different shaders
	shader(const char* vertexFile, const char* fragmentFile);

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();
private:
	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);
};

GLuint shaderProgram;

class Camera
{
public:
    // Stores the main vectors of the camera
    glm::vec3 Camara;
    glm::vec3 Right = glm::vec3(0.0f, 0.0f, -2.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    bool firstClick = true;
    float speed = 0.1f;
    float sensitivity = 100.0f;

    Camera(glm::vec3 position)
    {
        Camara = position;
    }

    void Matrix(float FOVdeg, float nearPlane, float farPlane, const char* uniform)
    {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::lookAt(Camara, Camara + Right, Up);
        projection = glm::perspective(glm::radians(FOVdeg), (float)SCR_WIDTH / SCR_HEIGHT, nearPlane, farPlane);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
    }



    void Inputs(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            Camara += speed * Right;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            Camara += speed * -glm::normalize(glm::cross(Right, Up));
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            Camara += speed * -Right;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
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
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            speed = 0.04f;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        {
            speed = 0.01f;
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

/*
void setSolve() {
		vector<string> move = get_solution(to_cube_not(cambios));
		cout << "\nSOLUTION: ";
		for (int i = 0; i < move.size(); i++) cout << move[i] << " ";
		//solve(move);
		cambios.clear();
		"\nSHUFFLE: ";

		return;
	}*/

int main()
{
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
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

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
  
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

	glEnable(GL_DEPTH_TEST);

    float vertices_flecha[] = { //Vertice inicial
		0.125f,  0.125f, 0.0f,//0
		0.125f, -0.125f,0.0f,//1
        -0.125f, -0.125f, 0.0f,//2
		-0.125f, 0.125f, 0.0f, //3
		
		0.125f,  0.125f, -0.25f,//4
		0.125f, -0.125f,-0.25f,//5
        -0.125f, -0.125f, -0.25f,//6
		-0.125f, 0.125f, -0.25f //7
       
    };
	
	float vertices_flechas[27][24];//Vertices del cubo
	for(int i = 0;i<27;i++){
		for(int j = 0;j<24;j++){
			vertices_flechas[i][j] = vertices_flecha[j];
		}
	}
	//float Pax, float Pay, float Paz, float ux, float uy, float uz, float vx, float vy, float vz,float* &vertices, int size
	float cx=0,cy=0,cz=0;
	
    unsigned int VAO, VBO[27], EBO[324];
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(27, VBO);
    glGenBuffers(324, EBO);
    
   
    glBindVertexArray(VAO);

    int location = glGetUniformLocation(shaderProgram, "u_color");
	
    //pieza cubito(vertices_flechas[0],location,VBO[0],EBO,0);
	//cubito.construir();
	//pieza cubito2(vertices_flechas[1],location,VBO[1],EBO,12);
	//cubito2.construir();
	//cubito2.trasladar(0.05,0,0);
	//cubito2.buffs();
	int iniebo=0;
	pieza cubitos[27];
	for(int i =0;i<27;i++){
		cubitos[i].set_pieza(vertices_flechas[i],location,VBO[i],EBO,iniebo);
		cubitos[i].construir();
		iniebo+=12;
	}
	//Cara frontal (incluye cubitos[0])
	cubitos[0].trasladar(0.05,0,0); cubitos[0].buffs();
	//cubitos[1]
	cubitos[2].trasladar(-0.05,0,0); cubitos[2].buffs();
	
	cubitos[3].trasladar(0.05,0.05,0); cubitos[3].buffs();
	cubitos[4].trasladar(0,0.05,0); cubitos[4].buffs();//arriba
	cubitos[5].trasladar(-0.05,0.05,0); cubitos[5].buffs();
	
	cubitos[6].trasladar(0.05,-0.05,0); cubitos[6].buffs();
	cubitos[7].trasladar(0,-0.05,0); cubitos[7].buffs(); //abajo
	cubitos[8].trasladar(-0.05,-0.05,0); cubitos[8].buffs();
	//Cara Intermedia
	
	cubitos[9].trasladar(0.05,0,0.05); cubitos[9].buffs();
	cubitos[10].trasladar(0,0,0.05); cubitos[10].buffs();
	cubitos[11].trasladar(-0.05,0,0.05); cubitos[11].buffs();
	
	cubitos[12].trasladar(0.05,0.05,0.05); cubitos[12].buffs();
	cubitos[13].trasladar(0,0.05,0.05); cubitos[13].buffs();//arriba
	cubitos[14].trasladar(-0.05,0.05,0.05); cubitos[14].buffs();
	
	cubitos[15].trasladar(0.05,-0.05,0.05); cubitos[15].buffs();
	cubitos[16].trasladar(0,-0.05,0.05); cubitos[16].buffs();//abajo
	cubitos[17].trasladar(-0.05,-0.05,0.05); cubitos[17].buffs();
	
	//Cara del fondo
	cubitos[18].trasladar(0.05,0,0.1); cubitos[18].buffs();
	cubitos[19].trasladar(0,0,0.1); cubitos[19].buffs();
	cubitos[20].trasladar(-0.05,0,0.1); cubitos[20].buffs();
	
	cubitos[21].trasladar(0.05,0.05,0.1); cubitos[21].buffs();
	cubitos[22].trasladar(0,0.05,0.1); cubitos[22].buffs();//arriba
	cubitos[23].trasladar(-0.05,0.05,0.1); cubitos[23].buffs();
	
	cubitos[24].trasladar(0.05,-0.05,0.1); cubitos[24].buffs();
	cubitos[25].trasladar(0,-0.05,0.1); cubitos[25].buffs();//abajo
	cubitos[26].trasladar(-0.05,-0.05,0.1); cubitos[26].buffs();
	
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
	
	for(int i=0;i<27;i++){ 
			cubitos[i].trasladar(0,0,-0.03);
			cubitos[i].buffs();
	}
	
	camada arriba(cubitos,5,4,3,14,13,12,23,22,21);//
	camada abajo(cubitos,8,7,6,17,16,15,26,25,24);//
	camada izquierda(cubitos,23,14,5,20,11,2,26,17,8);//
	camada derecha(cubitos,21,12,3,18,9,0,24,15,6);//
	camada atras(cubitos,23,22,21,20,19,18,26,25,24);//
	camada frente(cubitos,5,4,3,2,1,0,8,7,6);//
	
    
    
	
	glLineWidth((GLfloat)5);
    glPointSize((GLfloat)5);
	
	float pax = 0,pay = 0,paz = 0;
	int update[13] = {3,3,3,3,3,3,3,3,3,3,3,3,3};
	
	glfwSetKeyCallback(window, keyCallback);
	
	float rotation = 0.0f;
	double prevTime = glfwGetTime();
	//Cube Cube1;
	Camera cam(glm::vec3(0.0f,0.0f,2.0f));
	
    while (!glfwWindowShouldClose(window))
    {
		
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
		glUniform4f(location, 1.0f, 0.5f, 0.2f, 1.0f);	

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
		
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f,1.0f,0.0f));
		view = glm::translate(view, glm::vec3(0.0f,0.5f,-2.0f));
		proj = glm::perspective(glm::radians(45.0f),(float)(SCR_WIDTH/SCR_HEIGHT),0.1f,100.0f);
		
		// camera/view transformation
		//ourShader.setMat4("view", view);
		
        glBindVertexArray(VAO);
		glUniform4f(location, 1.0f, 0.5f, 0.2f, 1.0f);	
		
		cam.Inputs(window);
		
		cam.Matrix(45.0f,0.1f,100.0f,"camUnif");
		
		//float pax, float pay, float paz, float ux, float uy, float uz, float vx, float vy, float vz
		for(int i=0;i<27;i++) {cubitos[i].dibujar();}
		
		switch(accion)
		{
			case 'g':{for(int i=0;i<27;i++) {cubitos[i].buffs();}}
			break;
			/////////////////
			case 'n':{
			arriba.rotar_vertical_f(7.5);//Rota solo una cara
			if(update[0]%2==1){
			arriba.update();
			frente.update_top(arriba.indices_top[0],arriba.indices_top[1],arriba.indices_top[2]);
			derecha.update_top(arriba.indices_bot[2],arriba.indices_mid[2],arriba.indices_top[2]);
			izquierda.update_top(arriba.indices_bot[0],arriba.indices_mid[0],arriba.indices_top[0]);
			atras.update_top(arriba.indices_bot[0],arriba.indices_bot[1],arriba.indices_bot[2]);
			movimientos += "U' ";
			cambios.push_back("U' ");
			}
			update[0]++;
			accion = 'g';
			}
			break;

			case 'm':{
			abajo.rotar_vertical_f(7.5);
			if(update[1]%2==1){
			abajo.update();
			frente.update_bot(abajo.indices_top[0],abajo.indices_top[1],abajo.indices_top[2]);
			atras.update_bot(abajo.indices_bot[0],abajo.indices_bot[1],abajo.indices_bot[2]);
			derecha.update_bot(abajo.indices_bot[2],abajo.indices_mid[2],abajo.indices_top[2]);
			izquierda.update_bot(abajo.indices_bot[0],abajo.indices_mid[0],abajo.indices_top[0]);
			movimientos += "D ";
			cambios.push_back("D ");
			}
			update[1]++;
			accion = 'g';
			}
			break;

			case 'r':{
			izquierda.rotar_horizontal_r(7.5);
			if(update[2]%2==1){
			izquierda.update();
			frente.update_left(izquierda.indices_top[2],izquierda.indices_mid[2],izquierda.indices_bot[2]);
			atras.update_left(izquierda.indices_top[0],izquierda.indices_mid[0],izquierda.indices_bot[0]);

			arriba.update_left(izquierda.indices_top[2],izquierda.indices_top[1],izquierda.indices_top[0]);//
			abajo.update_left(izquierda.indices_bot[2],izquierda.indices_bot[1],izquierda.indices_bot[0]);//
			movimientos += "L ";
			cambios.push_back("L ");
			}
			update[2]++;
			accion = 'g';
			}
			break;

			case 't':{
			derecha.rotar_horizontal_r(7.5);
			if(update[3]%2==1){
			derecha.update();
			frente.update_right(derecha.indices_top[2],derecha.indices_mid[2],derecha.indices_bot[2]);
			atras.update_right(derecha.indices_top[0],derecha.indices_mid[0],derecha.indices_bot[0]);

			arriba.update_right(derecha.indices_top[2],derecha.indices_top[1],derecha.indices_top[0]);//
			abajo.update_right(derecha.indices_bot[2],derecha.indices_bot[1],derecha.indices_bot[0]);//
			movimientos += "R' ";
			cambios.push_back("R' ");
			}
			update[3]++;
			accion = 'g';
			}
			break;

			case 'v':{
			frente.rotar_vertical(7.5);
			if(update[4]%2==1){
			frente.update();
			izquierda.update_right(frente.indices_top[0],frente.indices_mid[0],frente.indices_bot[0]);
			derecha.update_right(frente.indices_top[2],frente.indices_mid[2],frente.indices_bot[2]);
			arriba.update_top(frente.indices_top[0],frente.indices_top[1],frente.indices_top[2]);
			abajo.update_top(frente.indices_bot[0],frente.indices_bot[1],frente.indices_bot[2]);
			movimientos += "B ";
			cambios.push_back("B ");
			}
			update[4]++;
			accion = 'g';
			}
			break;

			case 'b':{
			atras.rotar_vertical(7.5);
			if(update[5]%2==1){
			atras.update();
			izquierda.update_left(atras.indices_top[0],atras.indices_mid[0],atras.indices_bot[0]);
			derecha.update_left(atras.indices_top[2],atras.indices_mid[2],atras.indices_bot[2]);
			arriba.update_bot(atras.indices_top[0],atras.indices_top[1],atras.indices_top[2]);
			abajo.update_bot(atras.indices_bot[0],atras.indices_bot[1],atras.indices_bot[2]);
			movimientos += "F' ";
			cambios.push_back("F' ");
			}
			update[5]++;
			accion = 'g';
			}
			break;
			//////Inversas
			case 'h':{
			arriba.rotar_vertical_fa(-7.5);//Rota solo una cara
			if(update[6]%2==1){
			arriba.update_inv();
			frente.update_top(arriba.indices_top[0],arriba.indices_top[1],arriba.indices_top[2]);
			derecha.update_top(arriba.indices_bot[2],arriba.indices_mid[2],arriba.indices_top[2]);
			izquierda.update_top(arriba.indices_bot[0],arriba.indices_mid[0],arriba.indices_top[0]);
			atras.update_top(arriba.indices_bot[0],arriba.indices_bot[1],arriba.indices_bot[2]);
			movimientos += "U ";
			cambios.push_back("U ");
			}
			update[6]++;
			accion = 'g';
			}
			break;

			case 'j':{
			abajo.rotar_vertical_fa(-7.5);
			if(update[7]%2==1){
			abajo.update_inv();
			frente.update_bot(abajo.indices_top[0],abajo.indices_top[1],abajo.indices_top[2]);
			atras.update_bot(abajo.indices_bot[0],abajo.indices_bot[1],abajo.indices_bot[2]);
			derecha.update_bot(abajo.indices_bot[2],abajo.indices_mid[2],abajo.indices_top[2]);
			izquierda.update_bot(abajo.indices_bot[0],abajo.indices_mid[0],abajo.indices_top[0]);
			movimientos += "D' ";
			cambios.push_back("D' ");
			}
			update[7]++;
			accion = 'g';
			}
			break;

			case 'k':{
			izquierda.rotar_horizontal_ra(-7.5);
			if(update[8]%2==1){
			izquierda.update_inv();
			frente.update_left(izquierda.indices_top[2],izquierda.indices_mid[2],izquierda.indices_bot[2]);
			atras.update_left(izquierda.indices_top[0],izquierda.indices_mid[0],izquierda.indices_bot[0]);

			arriba.update_left(izquierda.indices_top[2],izquierda.indices_top[1],izquierda.indices_top[0]);//
			abajo.update_left(izquierda.indices_bot[2],izquierda.indices_bot[1],izquierda.indices_bot[0]);//
			movimientos += "L' ";
			cambios.push_back("L' ");
			}
			update[8]++;
			accion = 'g';
			}
			break;

			case 'l':{
			derecha.rotar_horizontal_ra(-7.5);
			if(update[9]%2==1){
			derecha.update_inv();
			frente.update_right(derecha.indices_top[2],derecha.indices_mid[2],derecha.indices_bot[2]);
			atras.update_right(derecha.indices_top[0],derecha.indices_mid[0],derecha.indices_bot[0]);

			arriba.update_right(derecha.indices_top[2],derecha.indices_top[1],derecha.indices_top[0]);//
			abajo.update_right(derecha.indices_bot[2],derecha.indices_bot[1],derecha.indices_bot[0]);//
			movimientos += "R ";
			cambios.push_back("R ");
			}
			update[9]++;
			accion = 'g';
			}
			break;

			case 'u':{
			frente.rotar_vertical_a(-7.5);
			if(update[10]%2==1){
			frente.update_inv();
			izquierda.update_right(frente.indices_top[0],frente.indices_mid[0],frente.indices_bot[0]);
			derecha.update_right(frente.indices_top[2],frente.indices_mid[2],frente.indices_bot[2]);
			arriba.update_top(frente.indices_top[0],frente.indices_top[1],frente.indices_top[2]);
			abajo.update_top(frente.indices_bot[0],frente.indices_bot[1],frente.indices_bot[2]);
			movimientos += "B' ";
			cambios.push_back("B' ");
			}
			update[10]++;
			accion = 'g';
			}
			break;

			case 'i':{
			atras.rotar_vertical_a(-7.5);
			if(update[11]%2==1){
			atras.update_inv();
			izquierda.update_left(atras.indices_top[0],atras.indices_mid[0],atras.indices_bot[0]);
			derecha.update_left(atras.indices_top[2],atras.indices_mid[2],atras.indices_bot[2]);
			arriba.update_bot(atras.indices_top[0],atras.indices_top[1],atras.indices_top[2]);
			abajo.update_bot(atras.indices_bot[0],atras.indices_bot[1],atras.indices_bot[2]);
			movimientos += "F ";
			cambios.push_back("F ");
			}
			update[11]++;
			accion = 'g';
			}
			break;
			
			case '_':{
				for(int i = 0; i<27; i++)
				{
					cubitos[i].rotary(0.2);
					cubitos[i].buffs();
				}
			}
			break;
			
			case 'z':{
				if(update[12]%2==1){
					//setSolve();
					Cube Cube1;
					string src = movimientos;
					string solve,moves;
					movimientos = "";
					int am, len;
					cout<<"Scramble "<<src<<endl;
					Cube1.SetScramble(src);
					Cube1.PrintCube();
					solve = SolveCube(Cube1, "Default");
					am = Cube1.SetScramble(solve);
					cout << "Moves: " << am << '\n';
					cout << "Solution: " << solve << '\n';
					//Cube1.PrintBoolIsSolved();
					moves = solution(solve);
					cout << "Solution2: " << moves << '\n';
					len=moves.length();
					
					for(int i=0;i<len;i++){
						//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
						//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						
						if(moves[i] == 'n'){
							arriba.rotar_vertical_f(7.5);//Rota solo una cara
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							arriba.rotar_vertical_f(7.5);//Rota solo una cara
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							arriba.update();
							frente.update_top(arriba.indices_top[0],arriba.indices_top[1],arriba.indices_top[2]);
							derecha.update_top(arriba.indices_bot[2],arriba.indices_mid[2],arriba.indices_top[2]);
							izquierda.update_top(arriba.indices_bot[0],arriba.indices_mid[0],arriba.indices_top[0]);
							atras.update_top(arriba.indices_bot[0],arriba.indices_bot[1],arriba.indices_bot[2]);
						}
						else if(moves[i] == 'm'){
							abajo.rotar_vertical_f(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							abajo.rotar_vertical_f(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							abajo.update();
							frente.update_bot(abajo.indices_top[0],abajo.indices_top[1],abajo.indices_top[2]);
							atras.update_bot(abajo.indices_bot[0],abajo.indices_bot[1],abajo.indices_bot[2]);
							derecha.update_bot(abajo.indices_bot[2],abajo.indices_mid[2],abajo.indices_top[2]);
							izquierda.update_bot(abajo.indices_bot[0],abajo.indices_mid[0],abajo.indices_top[0]);
						}
						else if(moves[i] == 'r'){
							izquierda.rotar_horizontal_r(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							izquierda.rotar_horizontal_r(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							izquierda.update();
							frente.update_left(izquierda.indices_top[2],izquierda.indices_mid[2],izquierda.indices_bot[2]);
							atras.update_left(izquierda.indices_top[0],izquierda.indices_mid[0],izquierda.indices_bot[0]);
							arriba.update_left(izquierda.indices_top[2],izquierda.indices_top[1],izquierda.indices_top[0]);//
							abajo.update_left(izquierda.indices_bot[2],izquierda.indices_bot[1],izquierda.indices_bot[0]);//
						}
						else if(moves[i] == 't'){
							derecha.rotar_horizontal_r(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							derecha.rotar_horizontal_r(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							derecha.update();
							frente.update_right(derecha.indices_top[2],derecha.indices_mid[2],derecha.indices_bot[2]);
							atras.update_right(derecha.indices_top[0],derecha.indices_mid[0],derecha.indices_bot[0]);
							arriba.update_right(derecha.indices_top[2],derecha.indices_top[1],derecha.indices_top[0]);//
							abajo.update_right(derecha.indices_bot[2],derecha.indices_bot[1],derecha.indices_bot[0]);//
						}
						else if(moves[i] == 'v'){
							frente.rotar_vertical(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							frente.rotar_vertical(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							frente.update();
							izquierda.update_right(frente.indices_top[0],frente.indices_mid[0],frente.indices_bot[0]);
							derecha.update_right(frente.indices_top[2],frente.indices_mid[2],frente.indices_bot[2]);
							arriba.update_top(frente.indices_top[0],frente.indices_top[1],frente.indices_top[2]);
							abajo.update_top(frente.indices_bot[0],frente.indices_bot[1],frente.indices_bot[2]);
						}
						else if(moves[i] == 'b'){
							atras.rotar_vertical(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							atras.rotar_vertical(7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							atras.update();
							izquierda.update_left(atras.indices_top[0],atras.indices_mid[0],atras.indices_bot[0]);
							derecha.update_left(atras.indices_top[2],atras.indices_mid[2],atras.indices_bot[2]);
							arriba.update_bot(atras.indices_top[0],atras.indices_top[1],atras.indices_top[2]);
							abajo.update_bot(atras.indices_bot[0],atras.indices_bot[1],atras.indices_bot[2]);
						}
						//Inversas
						else if(moves[i] == 'h'){
							arriba.rotar_vertical_fa(-7.5);//Rota solo una cara
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							arriba.rotar_vertical_fa(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							arriba.update_inv();
							frente.update_top(arriba.indices_top[0],arriba.indices_top[1],arriba.indices_top[2]);
							derecha.update_top(arriba.indices_bot[2],arriba.indices_mid[2],arriba.indices_top[2]);
							izquierda.update_top(arriba.indices_bot[0],arriba.indices_mid[0],arriba.indices_top[0]);
							atras.update_top(arriba.indices_bot[0],arriba.indices_bot[1],arriba.indices_bot[2]);
						}
						else if(moves[i] =='j'){
							abajo.rotar_vertical_fa(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							abajo.rotar_vertical_fa(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							abajo.update_inv();
							frente.update_bot(abajo.indices_top[0],abajo.indices_top[1],abajo.indices_top[2]);
							atras.update_bot(abajo.indices_bot[0],abajo.indices_bot[1],abajo.indices_bot[2]);
							derecha.update_bot(abajo.indices_bot[2],abajo.indices_mid[2],abajo.indices_top[2]);
							izquierda.update_bot(abajo.indices_bot[0],abajo.indices_mid[0],abajo.indices_top[0]);
						}
						else if(moves[i] == 'k'){
							izquierda.rotar_horizontal_ra(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							izquierda.rotar_horizontal_ra(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							izquierda.update_inv();
							frente.update_left(izquierda.indices_top[2],izquierda.indices_mid[2],izquierda.indices_bot[2]);
							atras.update_left(izquierda.indices_top[0],izquierda.indices_mid[0],izquierda.indices_bot[0]);
							arriba.update_left(izquierda.indices_top[2],izquierda.indices_top[1],izquierda.indices_top[0]);//
							abajo.update_left(izquierda.indices_bot[2],izquierda.indices_bot[1],izquierda.indices_bot[0]);//
						}
						else if(moves[i] == 'l'){
							derecha.rotar_horizontal_ra(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							derecha.rotar_horizontal_ra(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							derecha.update_inv();
							frente.update_right(derecha.indices_top[2],derecha.indices_mid[2],derecha.indices_bot[2]);
							atras.update_right(derecha.indices_top[0],derecha.indices_mid[0],derecha.indices_bot[0]);
							arriba.update_right(derecha.indices_top[2],derecha.indices_top[1],derecha.indices_top[0]);//
							abajo.update_right(derecha.indices_bot[2],derecha.indices_bot[1],derecha.indices_bot[0]);//
						}
						else if(moves[i] == 'u'){
							frente.rotar_vertical_a(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							frente.rotar_vertical_a(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							frente.update_inv();
							izquierda.update_right(frente.indices_top[0],frente.indices_mid[0],frente.indices_bot[0]);
							derecha.update_right(frente.indices_top[2],frente.indices_mid[2],frente.indices_bot[2]);
							arriba.update_top(frente.indices_top[0],frente.indices_top[1],frente.indices_top[2]);
							abajo.update_top(frente.indices_bot[0],frente.indices_bot[1],frente.indices_bot[2]);	
						}
						else if(moves[i] == 'i'){
							atras.rotar_vertical_a(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							atras.rotar_vertical_a(-7.5);
							for(int i=0;i<27;i++) {cubitos[i].buffs();}
							atras.update_inv();
							izquierda.update_left(atras.indices_top[0],atras.indices_mid[0],atras.indices_bot[0]);
							derecha.update_left(atras.indices_top[2],atras.indices_mid[2],atras.indices_bot[2]);
							arriba.update_bot(atras.indices_top[0],atras.indices_top[1],atras.indices_top[2]);
							abajo.update_bot(atras.indices_bot[0],atras.indices_bot[1],atras.indices_bot[2]);
						}
						sleep(1);
						glfwSwapBuffers(window);	
					}
				}
				update[12]++;
				accion = 'g';
			}	
			break;
			
			default:{}	
			break;
			
		}
		
		//update = true;
		
		glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(27, VBO);
    glDeleteBuffers(324, EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void create_figure(figura &temp, int location) {
    glBindBuffer(GL_ARRAY_BUFFER, temp.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.EBO1);
    glDrawElements(GL_TRIANGLES, temp.iTSize, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.EBO2);
    glUniform4f(location, 0.0f, 0.0f, 0.0f, 1.0f);
    glDrawElements(GL_LINE_STRIP, temp.iLSize, GL_UNSIGNED_INT, 0);
    glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f);
    glDrawElements(GL_POINTS, temp.iTSize*2, GL_UNSIGNED_INT, 0);
}

void create_only_lines(figura &temp, int location,GLfloat r,GLfloat g,GLfloat b){
	glBindBuffer(GL_ARRAY_BUFFER, temp.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.EBO2);
	glUniform4f(location, r, g, b, 1.0f);
    glDrawElements(GL_LINE_STRIP, temp.iLSize, GL_UNSIGNED_INT, 0);
}

void create_only_triangulos(figura &temp, int location,GLfloat r,GLfloat g,GLfloat b){
	glBindBuffer(GL_ARRAY_BUFFER, temp.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.EBO1);
	glUniform4f(location, r, g, b, 1.0f);
    glDrawElements(GL_TRIANGLES, temp.iTSize, GL_UNSIGNED_INT, 0);
}

void create_only_points(figura &temp, int location,GLfloat r,GLfloat g,GLfloat b){
	glBindBuffer(GL_ARRAY_BUFFER, temp.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp.EBO2);
	glUniform4f(location, r, g, b, 1.0f);
    glDrawElements(GL_POINTS, temp.iTSize*2, GL_UNSIGNED_INT, 0);
}

//Key input
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //si presionas a muestra la estrella
    if (key == GLFW_KEY_Q)
    {
        accion='a';
    }
    //si presiones b muestra la flecha
    if (key == GLFW_KEY_W)
    {
        accion = '.';
    }
    //si presiones d muestra el circulo
    if (key == GLFW_KEY_E)
    {
        accion = 'c';
    }
	if (key == GLFW_KEY_A)
    {
        accion = 'd';
    }
	if (key == GLFW_KEY_S)
    {
        accion = 'e';
    }
	if (key == GLFW_KEY_D)
    {
        accion = 'f';
    }
	if (key == GLFW_KEY_N)
    {
        accion = 'n';
		
    }
	if (key == GLFW_KEY_M)
    {
        accion = 'm';
		
    }
	if (key == GLFW_KEY_V)
    {
        accion = 'v';
		
    }
	if (key == GLFW_KEY_B)
    {
        accion = 'b';
		
    }
	if (key == GLFW_KEY_T)
    {
        accion = 't';
		
    }
	if (key == GLFW_KEY_R)
    {
        accion = 'r';
		
    }
	if (key == GLFW_KEY_J)
    {
        accion = 'j';
		
    }
	if (key == GLFW_KEY_H)
    {
        accion = 'h';
		
    }
	if (key == GLFW_KEY_K)
    {
        accion = 'k';
		
    }
	if (key == GLFW_KEY_L)
    {
        accion = 'l';
		
    }
	if (key == GLFW_KEY_U)
    {
        accion = 'u';
		
    }
	if (key == GLFW_KEY_I)
    {
        accion = 'i';
		
    }
	if (key == GLFW_KEY_P)
    {
        accion = '_';
    }
	if (key == GLFW_KEY_Z)
    {
        accion = 'z';
    }

}

void buff_circulo(int angulo, figura &Circulo) {

    int triangulos = 360 / angulo,pos = 1;; 
    float x2 = 0.4f, y2 = 0.0f, radio = 0.4f;
	float angulo_triangulo = 0.0, valor_radian = 0.0;
	
    float *vertices = new float[(triangulos * 3) + 6];
    unsigned int*indices = new unsigned int[(triangulos * 3) + 3];
    
    vertices[0] = 0.0f;
    vertices[1] = 0.0f;
    vertices[2] = 0.0f;

    vertices[3] = x2;
    vertices[4] = y2;
    vertices[5] = 0.0f;

    for (int i = 6; i <= triangulos * 3 + 3; i += 3) {
        angulo_triangulo += angulo;
        valor_radian = angulo_triangulo * (M_PI/ 180);
        x2 = radio * cos(valor_radian);
        y2 = radio * sin(valor_radian);
        
        vertices[i] = x2;
        vertices[i + 1] = y2;
        vertices[i + 2] = 0.0f;
    }
    
    for (int i = 0; i <= triangulos * 3; i += 3) {
        indices[i] = 0;
        indices[i + 1] = pos;
        indices[i + 2] = pos + 1;
        pos++;
    }
    
       Circulo.Mvertices((triangulos * 3 + 6), vertices);
	   Circulo.Itriangulos((triangulos * 3 + 3), indices);
	   Circulo.Ilineas((triangulos * 3 + 3), indices);
	   Circulo.buffers();
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