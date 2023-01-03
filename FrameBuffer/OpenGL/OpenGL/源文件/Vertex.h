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
//����VAO
       
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        //����VBO

        glGenBuffers(1, &VBO);
        //�󶨻������
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //����glBufferData������ʹ���㸴�Ƶ�������ڴ�
        glBufferData(GL_ARRAY_BUFFER, versize, vertices, GL_STATIC_DRAW);
        //Ԫ�ػ���������

        glGenBuffers(1, &EBO);
        //�Ȱ�EBOȻ����glBufferData���������Ƶ�������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indsize, indices, GL_STATIC_DRAW);
        //��glDrawElements���滻glDrawArrays����
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBindVertexArray(VAO);

        //�����ۼ�
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