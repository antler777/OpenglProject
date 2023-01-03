#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include"shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
using namespace std;

//------------------------Function��Start��-----------------------
#pragma region
string loadShaderSrc(const char* filename) {

ifstream file;
stringstream buf;
string ret = "";

file.open(filename);//��

if (file.is_open()) {
    buf << file.rdbuf();//���ļ�д��buf
    ret = buf.str();    //��buf���ȡ�ַ���ret;
}
else {
    cout << "Could not open" << filename << endl;
}
file.close();//�ر��ļ�

return ret;

}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
#pragma endregion 
//------------------------Function��End��-----------------------
int main()
{
//------------------------��ʼ����Start��-----------------------
#pragma region
    //ʵ��������
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //����һ������
    GLFWwindow* window = glfwCreateWindow(1280, 980, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    //��ʼ��Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //��ʼ��shader
    const char* vs = "assets/vertexShader.glsl";
    const char* fs = "assets/fragmentShader0.glsl";
    const char* fs2 = "assets/fragmentShader1.glsl";
    Shader shader(vs, fs, fs2);
#pragma endregion  
 //------------------------��ʼ����End��-----------------------
 

 //------------------------�������ݣ�Start��-----------------------
#pragma region
    //���嶥��
    float vertices[] = {
        //����                         vertex color                UV
        0.5f, 0.5f, 0.0f,              1,0,0,                      1.0f, 1.0f,// ���Ͻ�
        0.5f, -0.5f, 0.0f,             0,1,0,                      1.0f, 0.0f,// ���½�
        -0.5f, -0.5f, 0.0f,            0,0,1,                      0.0f, 0.0f,// ���½�
        -0.5f, 0.5f, 0.0f,             1,1,0,                      0.0f, 1.0f// ���Ͻ�
    };

    unsigned int indices[] = {
        // ע��������0��ʼ! 
        // ����������(0,1,2,3)���Ƕ�������vertices���±꣬
        // �����������±��������ϳɾ���

        0, 1, 3, // ��һ��������
        1, 2, 3  // �ڶ���������
    };

    //����VAO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //����VBO

    glGenBuffers(1, &VBO);
    //�󶨻������
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //����glBufferData������ʹ���㸴�Ƶ�������ڴ�
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //Ԫ�ػ���������

    glGenBuffers(1, &EBO);
    //�Ȱ�EBOȻ����glBufferData���������Ƶ�������
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //��glDrawElements���滻glDrawArrays����
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //------------------------��������ָ��---------------------- -
    //����λ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //��ɫ����
    // ��ɫ����
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // UV Texcoord        ID,����λ�ã�����   GL_FALse, �ܹ�������     ƫ�Ƽ�����λ
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

#pragma endregion  
//------------------------�������ݣ�End��-----------------------



//------------------------Texture��Start��-----------------------
#pragma region
    //Texture1
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //���ˣ��൱�����²���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ���ز���������
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/container.jpg", &width, &height, &nrChannels, 0);
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
    //Texture2
    unsigned int texture2;
    glGenTextures(1, &texture2);
    //OpenGLҪ��y��0.0��������ͼƬ�ĵײ��ģ�����ͼƬ��y��0.0����ͨ���ڶ���,��Ҫת��
    stbi_set_flip_vertically_on_load(true);
    glActiveTexture(GL_TEXTURE1);//������ͼ
    glBindTexture(GL_TEXTURE_2D, texture2);
    // Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //���ˣ��൱�����²���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ���ز���������
    data = stbi_load("assets/T2.jpg", &width, &height, &nrChannels, 0);//�ڶ����������¼�����У�����Ҫ����ָ����
    if (data)
    {
        std::cout << "T2 Success to load texture" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "T2 Failed to load texture" << std::endl;
    }
    //������Ҫ������ʹ��
    shader.use(0); 
    shader.setInt("t1", 0);
    shader.setInt("t2", 1);
    shader.use(1);
    shader.setInt("t1", 0);
    shader.setInt("t2", 1);
    stbi_image_free(data);
#pragma endregion  
//------------------------Texture��End��-----------------------


 //------------------------��Ⱦѭ����Start��-----------------------
  
#pragma region
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//ѭ������������߿�ģʽ
    while (!glfwWindowShouldClose(window))
    {
        //����
        processInput(window);
        
        //��Ⱦָ��
        glClearColor(1.0f, .5f, 0.3f, 1.0f);
        //�����Ļ
        glClear(GL_COLOR_BUFFER_BIT);
        //����������
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_TRIANGLES, 3, 3);
        //           type  ,�����ٸ�ֵ������
        
        shader.use(0);

        float timeValue = glfwGetTime();
        //float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // shader.setFloat("newColor", greenValue);


        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        shader.use(1);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3*sizeof(unsigned int)));
        //����
        glBindVertexArray(0);
        //����ǰ��֡
        glfwPollEvents();
        glfwSwapBuffers(window);
        
    }
#pragma endregion 
 //------------------------��Ⱦѭ����End��-----------------------
    glfwTerminate();
    return 0;
}