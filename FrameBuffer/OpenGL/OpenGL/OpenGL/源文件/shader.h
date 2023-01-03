#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�

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
        string vertShaderSrc = loadShaderSrc(vs);//��ȡglsl�� vertShaderSrc
        const GLchar* vertexShaderSource = vertShaderSrc.c_str();
        string fragShaderSrc = loadShaderSrc(fs);//��ȡglsl�� vertShaderSrc
        const GLchar* fragmentShaderSource = fragShaderSrc.c_str();
        string fragShaderSrc1 = loadShaderSrc(fs2);//��ȡglsl�� vertShaderSrc
        const GLchar* fragmentShaderSource1 = fragShaderSrc1.c_str();
        //VS
        unsigned int vertexShader;  //����shader���󣬲�����ID������
        vertexShader = glCreateShader(GL_VERTEX_SHADER);//��glCreateShader���������ɫ��
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//���ӵ���ɫ��������
        glCompileShader(vertexShader);//����
        //FS[0]
        unsigned int fragmentShader[2];
        fragmentShader[0] = glCreateShader(GL_FRAGMENT_SHADER);//����shader
        glShaderSource(fragmentShader[0], 1, &fragmentShaderSource, NULL);//����source
        glCompileShader(fragmentShader[0]);
        //FS[1]
        fragmentShader[1] = glCreateShader(GL_FRAGMENT_SHADER);//����shader
        glShaderSource(fragmentShader[1], 1, &fragmentShaderSource1, NULL);//����source
        glCompileShader(fragmentShader[1]);
        //����VS��FS[0]
       
        shaderProgram[0] = glCreateProgram();//����Program
        glAttachShader(shaderProgram[0], vertexShader);//����VS
        glAttachShader(shaderProgram[0], fragmentShader[0]);//����FS
        glLinkProgram(shaderProgram[0]);//����

            //����VS��FS[1]
        shaderProgram[1] = glCreateProgram();//����Program
        glAttachShader(shaderProgram[1], vertexShader);//����VS
        glAttachShader(shaderProgram[1], fragmentShader[0]);//����FS   ����������¸�����shader����
        glLinkProgram(shaderProgram[1]);//����
        //ɾ��shader
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader[0]);
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
