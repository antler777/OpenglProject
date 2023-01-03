#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include <fstream>

void texture (const char* texpath, int id, bool flip, unsigned char* data) {
        unsigned int texture1;
        int width, height, nrChannels;

        stbi_set_flip_vertically_on_load(flip);
        glGenTextures(1, &texture1);
        glActiveTexture(GL_TEXTURE0 + id); //������ͼ����Ԫ
        glBindTexture(GL_TEXTURE_2D, texture1);
        // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //���ˣ��൱�����²���
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // ���ز���������
        data = stbi_load(texpath, &width, &height, &nrChannels, 0);
        if (data)
        {
            std::cout << "Success to load texture" << std::endl;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

}