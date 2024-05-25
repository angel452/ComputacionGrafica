#include "glad.h"
#include <glm/glm.hpp>

class Figure {
    public:
        unsigned int VAO, VBO;
        int sizeVertexes;
        float* vertexes;
        float* centeredVertexes;
        glm::vec3 verticesCentrales;

        Figure();
        ~Figure();

        void move(float, float, float);
        void scale(float);
        void rotate(float, char);
        void rotate_inversa(float, char);
        void matrixVectorMult(const float *, const float *, float *);
        void transformVertices(const float*);
        void transformVerticesCenter(const float*);
        void calculateCenter();
        void printVertexes();
};
