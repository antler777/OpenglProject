#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Shader
{
    
public:
	Shader(const char* vs, const char*fs) {
        this->vs = vs;
        this->fs = fs;
        string vertShaderSrc = loadShaderSrc(vs);//��ȡglsl�� vertShaderSrc
        const GLchar* vertexShaderSource = vertShaderSrc.c_str();
        string fragShaderSrc = loadShaderSrc(fs);//��ȡglsl�� vertShaderSrc
        const GLchar* fragmentShaderSource = fragShaderSrc.c_str();
        //string fragShaderSrc1 = loadShaderSrc(fs2);//��ȡglsl�� vertShaderSrc
        //const GLchar* fragmentShaderSource1 = fragShaderSrc1.c_str();
        //VS
        unsigned int vertexShader;  //����shader���󣬲�����ID������
        vertexShader = glCreateShader(GL_VERTEX_SHADER);//��glCreateShader���������ɫ��
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//���ӵ���ɫ��������
        glCompileShader(vertexShader);//����
        //FS[0]
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//����shader
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);//����source
        glCompileShader(fragmentShader);

        //����VS��FS[0]
       
        shaderProgram = glCreateProgram();//����Program
        glAttachShader(shaderProgram, vertexShader);//����VS
        glAttachShader(shaderProgram, fragmentShader);//����FS
        glLinkProgram(shaderProgram);//����


        //ɾ��shader
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        //glDeleteShader(fragmentShader[1]);

	}

        string loadShaderSrc(const char* filePath) {

        ifstream file;
        stringstream buf;
        string ret = "";

        file.open(filePath);//��

        if (file.is_open()) {
            buf << file.rdbuf();//���ļ�д��buf
            ret = buf.str();    //��buf���ȡ�ַ���ret;
        }
        else {
            cout << "Could not open" << filePath << endl;
        }
        file.close();//�ر��ļ�

        return ret;

    }
        void use() {

                return glUseProgram(shaderProgram);

        }
        void setBool(const string& name, bool value) const
        {
            glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
        }
        void setInt(const string& name, int value) const
        {
            glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
        }
        void setFloat(const string& name, float value) const
        {
            glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
        }
        void setMat4(const string& name, mat4 value) const
        {
            unsigned int transformLoc = glGetUniformLocation(shaderProgram, name.c_str());
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(value));
        }
        void setVec3(const std::string& name, const glm::vec3& value) const
        {
            glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
        }
        void setVec3(const std::string& name, float x, float y, float z) const
        {
            glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
        }

        const char* vs; const char* fs; 
        unsigned int shaderProgram;
};

#endif
