#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件

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
        string vertShaderSrc = loadShaderSrc(vs);//读取glsl到 vertShaderSrc
        const GLchar* vertexShaderSource = vertShaderSrc.c_str();
        string fragShaderSrc = loadShaderSrc(fs);//读取glsl到 vertShaderSrc
        const GLchar* fragmentShaderSource = fragShaderSrc.c_str();
        //string fragShaderSrc1 = loadShaderSrc(fs2);//读取glsl到 vertShaderSrc
        //const GLchar* fragmentShaderSource1 = fragShaderSrc1.c_str();
        //VS
        unsigned int vertexShader;  //创建shader对象，并且用ID来引用
        vertexShader = glCreateShader(GL_VERTEX_SHADER);//用glCreateShader创建这个着色器
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//附加到着色器对象上
        glCompileShader(vertexShader);//编译
        //FS[0]
        unsigned int fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//创建shader
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);//设置source
        glCompileShader(fragmentShader);

        //链接VS和FS[0]
       
        shaderProgram = glCreateProgram();//创建Program
        glAttachShader(shaderProgram, vertexShader);//链接VS
        glAttachShader(shaderProgram, fragmentShader);//链接FS
        glLinkProgram(shaderProgram);//链接


        //删除shader
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        //glDeleteShader(fragmentShader[1]);

	}

        string loadShaderSrc(const char* filePath) {

        ifstream file;
        stringstream buf;
        string ret = "";

        file.open(filePath);//打开

        if (file.is_open()) {
            buf << file.rdbuf();//把文件写入buf
            ret = buf.str();    //从buf里读取字符到ret;
        }
        else {
            cout << "Could not open" << filePath << endl;
        }
        file.close();//关闭文件

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
