#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;



class Shader
{
public:
	Shader(const char* vs, const char*fs, const char* fs2) {
        this->vs = vs;
        this->fs = fs;
        this->fs2 = fs2;
        string vertShaderSrc = loadShaderSrc(vs);//读取glsl到 vertShaderSrc
        const GLchar* vertexShaderSource = vertShaderSrc.c_str();
        string fragShaderSrc = loadShaderSrc(fs);//读取glsl到 vertShaderSrc
        const GLchar* fragmentShaderSource = fragShaderSrc.c_str();
        string fragShaderSrc1 = loadShaderSrc(fs2);//读取glsl到 vertShaderSrc
        const GLchar* fragmentShaderSource1 = fragShaderSrc1.c_str();
        //VS
        unsigned int vertexShader;  //创建shader对象，并且用ID来引用
        vertexShader = glCreateShader(GL_VERTEX_SHADER);//用glCreateShader创建这个着色器
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//附加到着色器对象上
        glCompileShader(vertexShader);//编译
        //FS[0]
        unsigned int fragmentShader[2];
        fragmentShader[0] = glCreateShader(GL_FRAGMENT_SHADER);//创建shader
        glShaderSource(fragmentShader[0], 1, &fragmentShaderSource, NULL);//设置source
        glCompileShader(fragmentShader[0]);
        //FS[1]
        fragmentShader[1] = glCreateShader(GL_FRAGMENT_SHADER);//创建shader
        glShaderSource(fragmentShader[1], 1, &fragmentShaderSource1, NULL);//设置source
        glCompileShader(fragmentShader[1]);
        //链接VS和FS[0]
       
        shaderProgram[0] = glCreateProgram();//创建Program
        glAttachShader(shaderProgram[0], vertexShader);//链接VS
        glAttachShader(shaderProgram[0], fragmentShader[0]);//链接FS
        glLinkProgram(shaderProgram[0]);//链接

            //链接VS和FS[1]
        shaderProgram[1] = glCreateProgram();//创建Program
        glAttachShader(shaderProgram[1], vertexShader);//链接VS
        glAttachShader(shaderProgram[1], fragmentShader[0]);//链接FS   我在这边重新更改了shader内容
        glLinkProgram(shaderProgram[1]);//链接
        //删除shader
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader[0]);
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
        void use(int i) {
            if (i == 0) {
                return glUseProgram(shaderProgram[0]);
            }
            else if (i == 1) {
                return glUseProgram(shaderProgram[1]);
            }
            
        }
        void setBool(const string& name, bool value) const
        {
            glUniform1i(glGetUniformLocation(shaderProgram[0], name.c_str()), (int)value);
        }
        void setInt(const string& name, int value) const
        {
            glUniform1i(glGetUniformLocation(shaderProgram[0], name.c_str()), value);
        }
        void setFloat(const string& name, float value) const
        {
            glUniform1f(glGetUniformLocation(shaderProgram[0], name.c_str()), value);
        }
        const char* vs; const char* fs; const char* fs2;
        unsigned int shaderProgram[2];
};

#endif
