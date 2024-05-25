#include <glm/glm.hpp>

class Figure {
    public:
        unsigned int VAO, VBO;
        int sizeVertexes;
        float* vertexes;
        float* centeredVertexes; // No se para que se usa
        glm::vec3 verticesCentrales;

        //glm::vec3 center;
        //glm::mat4 transformationMatrix; 
        //unsigned int* indexes;
        
        //int sizeIndexes;
        //int scaleAmount;
        //int scaleDirection;

        Figure();
        ~Figure();
        //Figure(float, float, float);
        //Figure(float*, int);
        //Figure(float*, int, unsigned int*, int);
        
        void move(float, float, float);
        void scale(float);
        void rotate(float, char);
        void rotate_inversa(float, char);
        void matrixVectorMult(const float*, const float*, float*);
        void transformVertices(const float*);
        void transformVerticesCenter(const float*);
        void calculateCenter();
        //void updateTransMatrix();
        void printVertexes();
};
