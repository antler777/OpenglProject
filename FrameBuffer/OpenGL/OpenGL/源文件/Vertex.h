#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>

class Vertex {
public:
    unsigned int VAO, VBO, EBO;
	Vertex(float vertices[],int versize,int indsize, unsigned int indices[],unsigned int prop[], int propSize) {
        //=------------VAO,VBO,VEO-------------
//创建VAO
       
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        //创建VBO

        glGenBuffers(1, &VBO);
        //绑定缓冲对象
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //调用glBufferData函数，使顶点复制到缓冲的内存
        glBufferData(GL_ARRAY_BUFFER, versize, vertices, GL_STATIC_DRAW);
        //元素缓冲区对象

        glGenBuffers(1, &EBO);
        //先绑定EBO然后用glBufferData把索引复制到缓冲里
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indsize, indices, GL_STATIC_DRAW);
        //用glDrawElements来替换glDrawArrays函数
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBindVertexArray(VAO);

        //属性累加
        int sumPro = 0;
        int* propOffset = new int[propSize];
        int temp = 0;
        for (int i = 0; i < propSize; i++) {
            sumPro += prop[i];
            if (i == 0)
                propOffset[i] = 0;
            else
                temp += prop[i - 1];
            propOffset[i] = temp;
        }
        for (int i = 0; i < propSize; i++) {
            if (i==0)
                setProperty(i, prop[i], sumPro, 0);
            else
                setProperty(i, prop[i], sumPro, propOffset[i]);
            
        }
	}
    void setProperty(int PId,int PSize, int VSize, int POffset) {
        glVertexAttribPointer(PId, PSize, GL_FLOAT, GL_FALSE, VSize * sizeof(float), (void*)(POffset * sizeof(float)));
        glEnableVertexAttribArray(PId);
    }

    void deleteBuffer() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};