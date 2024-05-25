#include <glm/glm.hpp>
class Figure {
    public:
        glm::vec3 center;
        glm::mat4 transformationMatrix;
        float* vertexes;

        float* centeredVertexes; // No se para que se usa
        glm::vec3 verticesCentrales;
        
        unsigned int* indexes;
        int sizeVertexes;
        //int sizeIndexes;
        //int scaleAmount;
        //int scaleDirection;

        void calculateCenter();
        void matrixVectorMult(const float*, const float*, float*);
        void transformVertices(const float*);
        Figure();
        //Figure(float*, int);
        //Figure(float*, int, unsigned int*, int);
        ~Figure();
        void scale(float);
        void rotate(float, char);
        void rotate_inversa(float, char);
        void move(float, float, float);
        void updateTransMatrix();
        void printVertexes();
};
