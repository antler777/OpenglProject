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


//=============================================���ñ���
bool mouseCapture = true;
Camera camera;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void Defaultshader(Shader& shader, glm::vec3& light_color, float Light_int, float outRange, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& lightSpaceMatrix);
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouseCapture)
        camera.mouse_callback(xpos, ypos);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.scroll_callback(xoffset, yoffset);
}
void key_callback(GLFWwindow* window, int a, int b, int c, int d) {
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        if (mouseCapture)
            mouseCapture = false;
        else
            mouseCapture = true;
}
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//     --------------------------------------Light setting-----------------------------
vec3 dirlight_color = vec3(1.0, 1, 1);
vec3 p_lightPos = vec3(0.0f, 0.0f, 0.0f);
vec3 pointlight_color = vec3(1.0, 1, 1);
vec3 dirlight_position = vec3(-2.0f, 4.0f, -1.0f);
float lightSpeed = 2.0;
float lightRange = 1.0;
float PLight_Range = 1.0;


// lighting info
// ����Դ
vec3 lightPos(-2.0f, 3.0f, -1.0f);



int main()
{
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
    //--------------GUI��������--------------------------------
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); //������ɫ
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    //--------------GUI��������--------------------------------
    bool show_light = true;
    float Light_pos = 0.;
    vec3 light_color = vec3(1., 1., 1.);
    int counter = 0;
    float Light_int = 1.;
    float outRange = 30; //�۹��




    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader simpleDepthShader("assets/shadowmap_vs.glsl", "assets/shadowmap_fs.glsl");
    Shader debugDepthQuad("assets/FrameBuffer_vs.glsl", "assets/FrameBuffer_fs.glsl");
    Shader lightshader("assets/vs.glsl", "assets/fragmentShader1.glsl");
    Shader shader("assets/vertexShader.glsl", "assets/fragmentShader0.glsl");
    Shader planeshader("assets/vertexShader.glsl", "assets/fragmentShader0.glsl");
    // ������ͼ
    // -------------
    static unsigned char* data;



    unsigned int diffuseMap = loadTexture("assets/container2.jpg");
    unsigned int specularMap = loadTexture("assets/container2_specular.jpg");
    unsigned int WoodMap = loadTexture("assets/wood.jpg");
 

    //---------------------------------FBO׼���׶�------------------------------------------
    // configure depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    // ������ͼ
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    //��ͼ����
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //����FBO(֡�������)
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // ����depth map��FBO��depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    //��Ϊ�����ƾ��峡��
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    //���
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // -----------------------
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
    //---------------------------------׼���׶�------------------------------------------
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        //=========================������=====================
        if (mouseCapture)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//�������
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//��ʾ���
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        camera.processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. ��Ⱦ��ԴͶӰ�ռ�����ͼ
        // --------------------------------------------------------------
        mat4 lightProjection, lightView;
        mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f; //��Զ��Ļ
        lightProjection = ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane); //����ͶӰ
        lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView; //VP����
        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        //�޸���Ļ�ߴ�
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); //�� framebuffer
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, woodTexture);
        mat4 model = glm::mat4(1.0f);
        simpleDepthShader.setMat4("model", model);
        renderplane();
        renderScene(simpleDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

                //Bind ������
        glBindFramebuffer(GL_FRAMEBUFFER, texBuffer);
        glEnable(GL_DEPTH_TEST);
        // 2.���Ƴ���
        // ---------------------------------------------

        // reset��Ļ�ߴ�
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //��������VP����
        mat4 view(1);
        mat4 projection(1);
        projection = perspective(radians(camera.fov), 800.0f / 600.0f, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        planeshader.use();
        Defaultshader(planeshader, light_color, Light_int, outRange, model, view, projection, lightSpaceMatrix);
        //����ͼ
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, WoodMap);
        shader.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        shader.setInt("material.specular", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shader.setInt("shadowMap", 2);
        renderplane();

        shader.use();
     //   ����light����
     ////----------------------------ƽ�й�--------------------------------------
        Defaultshader(shader, light_color, Light_int, outRange, model, view, projection, lightSpaceMatrix);

        //����ͼ
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        shader.setInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        shader.setInt("material.specular", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shader.setInt("shadowMap", 2);

        renderScene(shader);

        

        //���Դ����
        mat4 model1(1);
        lightshader.use();
        lightPos.x = cos(Light_pos * lightSpeed) * lightRange;
        lightPos.z = sin(Light_pos * lightSpeed) * lightRange;
        model1 = translate(model1, lightPos);
        model1 = scale(model1, vec3(0.1, 0.1, 0.1));
        model1 = rotate(model1, radians(45.0f), vec3(1, 0, 1));//��ת
        lightshader.setMat4("model1", model1);
        lightshader.setMat4("view", view);
        lightshader.setMat4("projection", projection);
        lightshader.setVec3("lightcolor", light_color);

        if (show_light)
            renderCube();
        model1 = glm::mat4(1.0f);


        // 3.��Ⱦ��Ļ�ı��Σ�����depth map���
        // ---------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        debugDepthQuad.use();
        glActiveTexture(GL_TEXTURE0);
        debugDepthQuad.setInt("t0", 0);
        glBindTexture(GL_TEXTURE_2D, RT0);
        renderQuad();
       


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


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

void Defaultshader(Shader& shader, glm::vec3& light_color, float Light_int, float outRange, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& lightSpaceMatrix)
{
    shader.setVec3("dirlight.position", lightPos);
    shader.setVec3("dirlight.color", dirlight_color);
    //   //----------------------------���Դ1--------------------------------------
    shader.setVec3("pointlight[0].position", p_lightPos);
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
    shader.setFloat("light.outerCutOff", glm::cos(glm::radians(outRange + 5.0f)));

    ////���ò�������       
    shader.setVec3("material.ambient", vec3(0));
    shader.setVec3("material.diffuse", vec3(1, 0, 0));

    shader.setInt("material.specular", 1);
    shader.setFloat("material.shininess", 8);

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("viewPos", camera.cameraPos);
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
}



// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



// utility function for loading a 2D texture from file
// ---------------------------------------------------
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
