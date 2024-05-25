#include "shaderClass.h"
//#include "../../glad/include/glad/gl.h"
//#include "glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

ShaderClass::ShaderClass(const char* vertexShaderSource, const char* fragmentShaderSource){
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

void
ShaderClass::Activar_ShaderProgram(){
    glUseProgram(ID_ShaderProgram);
}

void 
ShaderClass::Eliminar_ShaderProgram(){
    glDeleteProgram(ID_ShaderProgram);
}

// utility uniform functions
void 
ShaderClass::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(ID_ShaderProgram, name.c_str()), (int)value); 
}

void 
ShaderClass::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(ID_ShaderProgram, name.c_str()), value); 
}

void 
ShaderClass::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(ID_ShaderProgram, name.c_str()), value); 
}

void 
ShaderClass::setVec2(const std::string &name, const glm::vec2 &value) const
{ 
    glUniform2fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, &value[0]); 
}

void 
ShaderClass::setVec2(const std::string &name, float x, float y) const
{ 
    glUniform2f(glGetUniformLocation(ID_ShaderProgram, name.c_str()), x, y); 
}

void 
ShaderClass::setVec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, &value[0]); 
}

void 
ShaderClass::setVec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(ID_ShaderProgram, name.c_str()), x, y, z); 
}

void 
ShaderClass::setVec4(const std::string &name, const glm::vec4 &value) const
{ 
    glUniform4fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, &value[0]); 
}

void 
ShaderClass::setVec4(const std::string &name, float x, float y, float z, float w) const
{ 
    glUniform4f(glGetUniformLocation(ID_ShaderProgram, name.c_str()), x, y, z, w); 
}

void 
ShaderClass::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void 
ShaderClass::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void 
ShaderClass::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID_ShaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
