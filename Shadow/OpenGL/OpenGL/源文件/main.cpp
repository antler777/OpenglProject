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


//=============================================设置变量
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
// 主光源
vec3 lightPos(-2.0f, 3.0f, -1.0f);



int main()
{
    //实例化窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //创建一个窗口
    GLFWwindow* window = glfwCreateWindow(camera.screenWidth, camera.screenHeight, "Antler's Rendering", NULL, NULL);
    glfwMakeContextCurrent(window);

    //回调函数
    glfwSetCursorPosCallback(window, mouse_callback);//鼠标
    glfwSetScrollCallback(window, scroll_callback);//滚轮
    glfwSetKeyCallback(window, key_callback);
    //初始化Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //--------------GUI窗口设置--------------------------------
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); //主题颜色
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    //--------------GUI变量设置--------------------------------
    bool show_light = true;
    float Light_pos = 0.;
    vec3 light_color = vec3(1., 1., 1.);
    int counter = 0;
    float Light_int = 1.;
    float outRange = 30; //聚光灯




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
    // 加载贴图
    // -------------
    static unsigned char* data;



    unsigned int diffuseMap = loadTexture("assets/container2.jpg");
    unsigned int specularMap = loadTexture("assets/container2_specular.jpg");
    unsigned int WoodMap = loadTexture("assets/wood.jpg");
 

    //---------------------------------FBO准备阶段------------------------------------------
    // configure depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    // 创建贴图
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    //贴图属性
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //创建FBO(帧缓冲对象)
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // 附加depth map到FBO的depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    //因为不绘制具体场景
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    //解绑
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // -----------------------
    //---------------------------------准备阶段------------------------------------------
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        //=========================鼠标控制=====================
        if (mouseCapture)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//隐藏鼠标
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//显示鼠标
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

        // 1. 渲染光源投影空间的深度图
        // --------------------------------------------------------------
        mat4 lightProjection, lightView;
        mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f; //近远屏幕
        lightProjection = ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane); //正交投影
        lightView = lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView; //VP矩阵
        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        //修改屏幕尺寸
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); //绑定 framebuffer
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, woodTexture);
        mat4 model = glm::mat4(1.0f);
        simpleDepthShader.setMat4("model", model);
        renderplane();
        renderScene(simpleDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 2.绘制场景
        // ---------------------------------------------

        // reset屏幕尺寸
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //重新设置VP矩阵
        mat4 view(1);
        mat4 projection(1);
        projection = perspective(radians(camera.fov), 800.0f / 600.0f, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        planeshader.use();
        Defaultshader(planeshader, light_color, Light_int, outRange, model, view, projection, lightSpaceMatrix);
        //绑定贴图
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
     //   设置light属性
     ////----------------------------平行光--------------------------------------
        Defaultshader(shader, light_color, Light_int, outRange, model, view, projection, lightSpaceMatrix);

        //绑定贴图
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

        

        //点光源绘制
        mat4 model1(1);
        lightshader.use();
        lightPos.x = cos(Light_pos * lightSpeed) * lightRange;
        lightPos.z = sin(Light_pos * lightSpeed) * lightRange;
        model1 = translate(model1, lightPos);
        model1 = scale(model1, vec3(0.1, 0.1, 0.1));
        model1 = rotate(model1, radians(45.0f), vec3(1, 0, 1));//旋转
        lightshader.setMat4("model1", model1);
        lightshader.setMat4("view", view);
        lightshader.setMat4("projection", projection);
        lightshader.setVec3("lightcolor", light_color);

        if (show_light)
            renderCube();
        model1 = glm::mat4(1.0f);


        // 3.渲染屏幕四边形，并对depth map输出
        // ---------------------------------------------
        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        debugDepthQuad.setInt("depthMap", 0);
        glBindTexture(GL_TEXTURE_2D, depthMap);




        //绘制GUI
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
    //   //----------------------------点光源1--------------------------------------
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

    ////设置材质属性       
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
