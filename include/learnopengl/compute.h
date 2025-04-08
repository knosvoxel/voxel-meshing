#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ComputeShader
{
public:
	unsigned int ID;

    ComputeShader() {};


	ComputeShader(const char* computePath)
	{
        // 1. retrieve the compute source code from filePath
        std::string computeCode;
        std::ifstream cShaderFile;
        //ensure ifstream objects can throw exceptions:
        cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open file
            cShaderFile.open(computePath);
            std::stringstream cShaderStream;
            // read file's buffer content into stream
            cShaderStream << cShaderFile.rdbuf();
            // close file handler
            cShaderFile.close();
            // convert stream into string
            computeCode = cShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* cShaderCode = computeCode.c_str();
        // 2. compile shaders
		unsigned int compute;
		// compute shader
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &cShaderCode, NULL);
		glCompileShader(compute);
		checkCompileErrors(compute, "COMPUTE");
        
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(compute);
	}

    // use/activate the shader
    void use()
    {
        glUseProgram(ID);
    }

private:
    // utility for checking shader compilation/linking errors
    // --------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" <<
                    infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" <<
                    infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};