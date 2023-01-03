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

//------------------------Function（Start）-----------------------
#pragma region
string loadShaderSrc(const char* filename) {

ifstream file;
stringstream buf;
string ret = "";

file.open(filename);//打开

if (file.is_open()) {
    buf << file.rdbuf();//把文件写入buf
    ret = buf.str();    //从buf里读取字符到ret;
}
else {
    cout << "Could not open" << filename << endl;
}
file.close();//关闭文件

return ret;

}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
#pragma endregion 
//------------------------Function（End）-----------------------
int main()
{
//------------------------初始化（Start）-----------------------
#pragma region
    //实例化窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //创建一个窗口
    GLFWwindow* window = glfwCreateWindow(1280, 980, "LearnOpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    //初始化Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //初始化shader
    const char* vs = "assets/vertexShader.glsl";
    const char* fs = "assets/fragmentShader0.glsl";
    const char* fs2 = "assets/fragmentShader1.glsl";
    Shader shader(vs, fs, fs2);
#pragma endregion  
 //------------------------初始化（End）-----------------------
 

 //------------------------顶点数据（Start）-----------------------
#pragma region
    //定义顶点
    float vertices[] = {
        //顶点                         vertex color                UV
        0.5f, 0.5f, 0.0f,              1,0,0,                      1.0f, 1.0f,// 右上角
        0.5f, -0.5f, 0.0f,             0,1,0,                      1.0f, 0.0f,// 右下角
        -0.5f, -0.5f, 0.0f,            0,0,1,                      0.0f, 0.0f,// 左下角
        -0.5f, 0.5f, 0.0f,             1,1,0,                      0.0f, 1.0f// 左上角
    };

    unsigned int indices[] = {
        // 注意索引从0开始! 
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    //创建VAO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //创建VBO

    glGenBuffers(1, &VBO);
    //绑定缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //调用glBufferData函数，使顶点复制到缓冲的内存
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //元素缓冲区对象

    glGenBuffers(1, &EBO);
    //先绑定EBO然后用glBufferData把索引复制到缓冲里
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //用glDrawElements来替换glDrawArrays函数
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //------------------------设置属性指针---------------------- -
    //顶点位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //颜色属性
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // UV Texcoord        ID,几个位置，精度   GL_FALse, 总共几个，     偏移几个单位
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

#pragma endregion  
//------------------------顶点数据（End）-----------------------



//------------------------Texture（Start）-----------------------
#pragma region
    //Texture1
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //过滤，相当于重新采样
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
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
    //OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部,需要转换
    stbi_set_flip_vertically_on_load(true);
    glActiveTexture(GL_TEXTURE1);//激活贴图
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //过滤，相当于重新采样
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    data = stbi_load("assets/T2.jpg", &width, &height, &nrChannels, 0);//第二章纹理重新加入就行，不需要重新指针了
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
    //必须先要程序先使用
    shader.use(0); 
    shader.setInt("t1", 0);
    shader.setInt("t2", 1);
    shader.use(1);
    shader.setInt("t1", 0);
    shader.setInt("t2", 1);
    stbi_image_free(data);
#pragma endregion  
//------------------------Texture（End）-----------------------


 //------------------------渲染循环（Start）-----------------------
  
#pragma region
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//循环体外面进入线框模式
    while (!glfwWindowShouldClose(window))
    {
        //输入
        processInput(window);
        
        //渲染指令
        glClearColor(1.0f, .5f, 0.3f, 1.0f);
        //清空屏幕
        glClear(GL_COLOR_BUFFER_BIT);
        //绘制三角形
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_TRIANGLES, 3, 3);
        //           type  ,画多少个值的类型
        
        shader.use(0);

        float timeValue = glfwGetTime();
        //float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // shader.setFloat("newColor", greenValue);


        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        shader.use(1);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3*sizeof(unsigned int)));
        //清零
        glBindVertexArray(0);
        //交换前后帧
        glfwPollEvents();
        glfwSwapBuffers(window);
        
    }
#pragma endregion 
 //------------------------渲染循环（End）-----------------------
    glfwTerminate();
    return 0;
}