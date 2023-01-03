#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"shader.h"

#define STB_IMAGE_IMPLEMENTATION

#include"stb_image.h"
#include"Vertex.h"
#include"texture.h"
#include"Model.h"
#include"camera.h"

//------------GUI-----------------
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


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
Camera camera;
unsigned int loadTexture(const char* path);
//     --------------------------------------Light setting-----------------------------
vec3 dirlight_color = vec3(1.0, 1, 1);
vec3 lightPos = vec3(0.0f, 0.0f, 0.0f);
vec3 pointlight_color = vec3(1.0, 1, 1);
vec3 dirlight_position = vec3(-0.2f, -1.0f, -0.5f);
float lightSpeed = 2.0;
float lightRange = 1.0;
float PLight_Range = 1.0;
//=============================================���ñ���
bool mouseCapture = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouseCapture)
        camera.mouse_callback(xpos, ypos);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.scroll_callback(xoffset,yoffset);
}
void key_callback(GLFWwindow* window,int a ,int b, int c, int d) {
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        if (mouseCapture)
            mouseCapture = false;
        else
            mouseCapture = true;
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
    GLFWwindow* window = glfwCreateWindow(camera.screenWidth, camera.screenHeight, "Antler's Rendering", NULL, NULL);
    glfwMakeContextCurrent(window);

    //�ص�����
    glfwSetCursorPosCallback(window, mouse_callback);//���
    glfwSetScrollCallback(window, scroll_callback);//����
    glfwSetKeyCallback(window, key_callback);
    //��ʼ��Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //�߿�ģʽ
    //ZTest
    glEnable(GL_DEPTH_TEST);
    //��ʼ��shader
    const char* vs1 = "assets/vertexShader.glsl";
    const char* vs = "assets/vs.glsl";
    const char* framevs = "assets/FrameBuffer_vs.glsl";
    const char* fs = "assets/fragmentShader0.glsl";
    const char* fs2 = "assets/fragmentShader1.glsl";
    const char* framefs = "assets/FrameBuffer_fs.glsl";

    Shader shader(vs1, fs);
    Shader shader1(vs , fs2);
    Shader frameshader(framevs, framefs);
    //--------------------------------lead model----------------



    //--------------GUI��������--------------------------------
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); //������ɫ
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    bool show_light = true;
    float Light_pos = 0.;
    vec3 light_color = vec3(1., 1., 1.);
    int counter = 0;
    float Light_int = 1.;
    float outRange = 30; //�۹��
#pragma endregion  
//------------------------��ʼ����End��-----------------------
 

//------------------------�������ݣ�Start��-----------------------
#pragma region
    //���嶥��

    Vertex vertex(vertices, sizeof(vertices), sizeof(indices), indices,prop, 3);
    vertex.setProperty(0, 3, 8, 0);
    vertex.setProperty(1, 3, 8, 3);
    vertex.setProperty(2, 2, 8, 6);
    Vertex vertex2(quadVertices, sizeof(quadVertices), sizeof(indices), indices, prop, 2);
    vertex2.setProperty(0, 2, 4, 0);
    vertex2.setProperty(1, 2, 4, 2);
#pragma endregion  
//------------------------�������ݣ�End��-----------------------

//------------------------Texture��Start��-----------------------
#pragma region
    static unsigned char* data=0;
    //T1
    texture("assets/container2.jpg",0,true,data);
    //T2
    texture("assets/container2_specular.jpg", 1, false , data+1);
    unsigned int diffuseMap = loadTexture("assets/container2.jpg");
    unsigned int specularMap = loadTexture("assets/container2_specular.jpg");

 
#pragma endregion  
//------------------------Texture��End��-----------------------

//------------------------����任��Start��-----------------------
#pragma region
    glm::vec3 cubePositions[] = {
          glm::vec3(0.0f,  0.0f,  0.0f),
          glm::vec3(2.0f,  5.0f, -15.0f),
          glm::vec3(-1.5f, -2.2f, -2.5f),
          glm::vec3(-3.8f, -2.0f, -12.3f),
          glm::vec3(2.4f, -0.4f, -3.5f),
          glm::vec3(-1.7f,  3.0f, -7.5f),
          glm::vec3(1.3f, -2.0f, -2.5f),
          glm::vec3(1.5f,  2.0f, -2.5f),
          glm::vec3(1.5f,  0.2f, -1.5f),
          glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // �����ʹ�õ���0.9.9��0.9.9���ϵİ汾������Ҫ�����еľ����ʼ����Ϊ glm::mat4 mat = glm::mat4(1.0f)��
    mat4 model(1);
    model = rotate(model, radians(-55.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 view(1);

    mat4 projection(1);
#pragma endregion  
//------------------------����任��End��-----------------------

//------------------------Framebuffer��Start��-----------------------
#pragma region

    // ���� buffer
    // -------------------------

    unsigned int texBuffer;
    glGenFramebuffers(1, &texBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, texBuffer);
    // ����һ��������
    unsigned int RT0;
    glGenTextures(1, &RT0);
    glBindTexture(GL_TEXTURE_2D, RT0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, camera.screenWidth, camera.screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //�������ӵ�buffer����
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RT0, 0);
    // ����RBO
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //���� Depth Sencil����
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, camera.screenWidth, camera.screenHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  

#pragma endregion  
    //------------------------Framebuffer��End��-----------------------

 //------------------------��Ⱦѭ����Start��-----------------------
  
#pragma region

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//ѭ������������߿�ģʽ
    while (!glfwWindowShouldClose(window))
    {


        //����
        camera.processInput(window);

        //Bind ������
        glBindFramebuffer(GL_FRAMEBUFFER, texBuffer);
        glEnable(GL_DEPTH_TEST);
        //=========================������=====================
        if (mouseCapture)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//�������
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//��ʾ���
        //��Ⱦָ��
        glClearColor(1.0f, .5f, 0.3f, 1.0f);
        //�����Ļ
       // glClear(GL_COLOR_BUFFER_BIT);
        // �����Ȼ��� �����Ļ
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //����������

        //�任���� 
        float timeValue = glfwGetTime();
        float sintime = (sin(timeValue)*360.0f) + 0.5f;
        //mat4 trans = mat4(1.0f);
        //trans = translate(trans, vec3(1.0f, 1.0f, 0.0f));
        //trans = rotate(trans, radians(sintime), vec3(0.0, 0.0, 1.0));
        //trans = scale(trans, vec3(2.0, 0.5, 0.5));
        //����    shader����
       
        shader.use();
        //View ���ã�������Զ��ƶ�
        projection = perspective(radians(camera.fov), 800.0f / 600.0f, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        //shader��ȡ��ͼ

        //============================����light����====================================
        //----------------------------ƽ�й�--------------------------------------
        shader.setVec3("dirlight.direction", dirlight_position);
 
        shader.setVec3("dirlight.color", dirlight_color);
        //----------------------------���Դ1--------------------------------------
        shader.setVec3("pointlight[0].position", lightPos);
        shader.setVec3("pointlight[0].color", pointlight_color);
        shader.setFloat("pointlight[0].constant", 1.0f);
        shader.setFloat("pointlight[0].linear", 0.09f);
        shader.setFloat("pointlight[0].quadratic", 0.032f);
        shader.setFloat("pointlight[0].range", PLight_Range);
        shader.setVec3("light.ambient", vec3(0));
        shader.setVec3("light.diffuse", light_color);
        shader.setVec3("light.specular", light_color);
        shader.setFloat("light.intensity", Light_int);


        shader.setFloat("light.cutOff", glm::cos(glm::radians(outRange)));
        shader.setFloat("light.outerCutOff", glm::cos(glm::radians(outRange+5.0f)));

        //���ò�������       
        shader.setVec3("material.ambient", vec3(0));
        shader.setVec3("material.diffuse", vec3(1,0,0));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        shader.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", 8);
        
        
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("viewPos", camera.cameraPos);
        glBindVertexArray(vertex.VAO);
      
        //============���ƶ���ķ�ʽ============
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);        
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(3*sizeof(unsigned int)));
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model(1);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //����shader1 
        mat4 model1(1);
        shader1.use();
        lightPos.x = cos(Light_pos * lightSpeed)*lightRange;
        lightPos.z = sin(Light_pos * lightSpeed) * lightRange;
        model1 = translate(model1, lightPos);
        model1 = scale(model1, vec3(0.1, 0.1, 0.1));
        model1 = rotate(model1, radians(45.0f), vec3(1, 0, 1));//��ת
        shader1.setMat4("model1", model1);
        shader1.setMat4("view", view);
        shader1.setMat4("projection", projection);
        shader1.setVec3("lightcolor", light_color);
 
        if(show_light)
            glDrawArrays(GL_TRIANGLES, 0, 36);



       //Bind ��Ĭ�ϻ���

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

       
        //������Ļ�ı���

        
        
        glActiveTexture(GL_TEXTURE5);
        frameshader.use();
        frameshader.setInt("t0", 5);
     
        glBindVertexArray(vertex2.VAO);
        glBindTexture(GL_TEXTURE_2D, RT0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //����GUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("GUI");
        ImGui::Text("This is some useful text.");
        ImGui::Checkbox("Light", &show_light);
 
        ImGui::SliderFloat("LightPosition", &Light_pos, 0.0f, 3.14f);
        ImGui::SliderFloat("LightIntensity", &Light_int, 0.0f, 10.f);
        ImGui::SliderFloat("PointLightRange", &PLight_Range, 0.0f, 10.f);
        ImGui::SliderFloat("FlashlightRange", &outRange, 0.0f, 90.f);
        ImGui::ColorEdit3("Point Light color", (float*)&pointlight_color);
        ImGui::ColorEdit3("Direction Light color", (float*)&dirlight_color);

        if (ImGui::Button("Test"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);


        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //����ǰ��֡
        glfwPollEvents();
        glfwSwapBuffers(window);
        
    }        
        //--------Vertex ����
       glDeleteVertexArrays(1, &vertex2.VAO);
       glBindVertexArray(0);
       // GUI  Cleanup
       ImGui_ImplOpenGL3_Shutdown();
       ImGui_ImplGlfw_Shutdown();
       ImGui::DestroyContext();
#pragma endregion 
 //------------------------��Ⱦѭ����End��-----------------------
    vertex.deleteBuffer();
    vertex2.deleteBuffer();
    glfwTerminate();
    return 0;
}
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}