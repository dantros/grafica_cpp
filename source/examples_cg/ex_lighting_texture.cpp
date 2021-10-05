/**
 * @file ex_lighting_texture.cpp
 * @brief Showing lighting effects over two textured objects with Phong
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <numbers>
#include <ciso646>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <grafica/root_directory.h>
#include <grafica/shape.h>
#include <grafica/basic_shapes.h>
#include <grafica/load_shaders.h>
#include <grafica/performance_monitor.h>
#include <grafica/easy_shaders.h>
#include <grafica/gpu_shape.h>
#include <grafica/transformations.h>
#include <grafica/simple_timer.h>
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>


namespace gr = Grafica;
namespace tr = Grafica::Transformations;

// A global variable to control the application
struct Controller
{
    bool fillPolygon = true;
    bool showAxis = true;
} controller;

// This function will be executed whenever a key is pressed or released
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_SPACE)
    {
        controller.fillPolygon = not controller.fillPolygon;
    }
    else if (key == GLFW_KEY_LEFT_CONTROL)
    {
        controller.showAxis = not controller.showAxis;
    }
}

gr::Shape createDice()
{
    gr::Shape shape(8);
    shape.vertices = {
    //   positions         tex coords   normals
    // Z+: number 1
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f/3,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.5f, 1.0f/3,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,

    // Z-: number 6
        -0.5f, -0.5f, -0.5f, 0.5f, 1.0f,      0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f,      0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 2.0f/3,    0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f, 0.5f, 2.0f/3,    0.0f, 0.0f, -1.0f,
        
    // X+: number 5
         0.5f, -0.5f, -0.5f, 0.0f,   1.0f,   1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.5f,   1.0f,   1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.5f, 2.0f/3,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 2.0f/3,   1.0f, 0.0f, 0.0f,
 
    // X-: number 2
        -0.5f, -0.5f, -0.5f, 0.5f, 1.0f/3,   -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f/3,   -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f,   0.0f,   -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.5f,   0.0f,   -1.0f, 0.0f, 0.0f,

    // Y+: number 4
        -0.5f,  0.5f, -0.5f, 0.5f, 2.0f/3,   0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 2.0f/3,   0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f/3,   0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.5f, 1.0f/3,   0.0f, 1.0f, 0.0f,

    // Y-: number 3
        -0.5f, -0.5f, -0.5f, 0.0f, 2.0f/3,   0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.5f, 2.0f/3,   0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.5f, 1.0f/3,   0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f/3,   0.0f, -1.0f, 0.0f
    };
    // One face of the cube per row
    shape.indices = {
         0, 1, 2, 2, 3, 0, // Z+
         7, 6, 5, 5, 4, 7, // Z-
         8, 9,10,10,11, 8, // X+
        15,14,13,13,12,15, // X-
        19,18,17,17,16,19, // Y+
        20,21,22,22,23,20 // Y-
    };

    return shape;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE); // uncomment this statement to fix compilation on OS X
#endif

    // Creating a glfw window
    constexpr unsigned int SCR_WIDTH = 600;
    constexpr unsigned int SCR_HEIGHT = 600;
    std::string title = "ex_lighting_texture";
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), NULL, NULL);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Connecting the callback function 'key_callback' to handle keyboard events
    glfwSetKeyCallback(window, key_callback);

    // Loading all OpenGL function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // This must be executed after loading OpenGL with GLAD
    /* ImGui Start ---- */

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    /* ImGui End ---- */

    // Creating our shader programs
    gr::ModelViewProjectionShaderProgram colorPipeline;
    gr::PhongTextureShaderProgram phongPipeline;

    // Creating shapes on GPU memory
    gr::GPUShape gpuAxis = gr::toGPUShape(colorPipeline, gr::createAxis(7));
    
    gr::GPUShape gpuWhiteDice = gr::toGPUShape(phongPipeline, createDice());
    gpuWhiteDice.texture = gr::textureSimpleSetup(
        gr::getPath("assets/imgs/dice.jpg"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);

    gr::GPUShape gpuBlueDice  = gr::toGPUShape(phongPipeline, createDice());
    gpuBlueDice.texture = gr::textureSimpleSetup(
        gr::getPath("assets/imgs/dice_blue.jpg"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);

    // Setting up the clear screen color
    glClearColor(0.85f, 0.85f, 0.85f, 1.0f);

    // As we work in 3D, we need to check which part is in front,
    // and which one is at the back enabling the depth testing
    glEnable(GL_DEPTH_TEST);

    // Computing some transformations
    float t0 = glfwGetTime(), t1, dt;
	float cameraTheta = std::numbers::pi / 4;

    gr::Matrix4f projection = tr::perspective(45, float(SCR_WIDTH)/float(SCR_HEIGHT), 0.1, 100);

    gr::Matrix4f modelWhiteDice = tr::translate(-0.75,0,0) * tr::rotationZ(-std::numbers::pi/16.0f) * tr::rotationX( std::numbers::pi );
    gr::Matrix4f modelBlueDice  = tr::translate( 0.75,0,0) * tr::rotationZ( std::numbers::pi/16.0f);

    gr::PerformanceMonitor performanceMonitor(glfwGetTime(), 0.5f);
    // glfw will swap buffers as soon as possible
    glfwSwapInterval(0);

    std::vector<gr::ProfileResult> stats;

    // Application loop
    while (!glfwWindowShouldClose(window))
    {
        // Measuring performance
        performanceMonitor.update(glfwGetTime());
        std::stringstream ss;
        ss << title << " " << performanceMonitor;
        glfwSetWindowTitle(window, ss.str().c_str());
        
        // Using GLFW to check and process input events
        glfwPollEvents();

        // Filling or not the shapes depending on the controller state
        glPolygonMode(GL_FRONT_AND_BACK, controller.fillPolygon ? GL_FILL : GL_LINE);

        /* ImGui Start ---- */

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* ImGui End ---- */

        // Getting the time difference from the previous iteration
        t1 = glfwGetTime();
        dt = t1 - t0;
        t0 = t1;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cameraTheta -= 2 * dt;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cameraTheta += 2 * dt;

        const gr::Vector3f viewPos(
            3 * std::sin(cameraTheta),
            3 * std::cos(cameraTheta),
            2);
        const gr::Vector3f eye(0,0,0);
        const gr::Vector3f at(0,0,1);

        gr::Matrix4f view = tr::lookAt(viewPos, eye, at);

        {
            PROFILE_SCOPE("uniforms", stats);
            
            //Clearing the screen in both, color and depth
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Drawing shapes with different uniforms and different shader programs
            glUseProgram(colorPipeline.shaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(colorPipeline.shaderProgram, "projection"), 1, GL_FALSE, projection.data());
            glUniformMatrix4fv(glGetUniformLocation(colorPipeline.shaderProgram, "view"), 1, GL_FALSE, view.data());
            glUniformMatrix4fv(glGetUniformLocation(colorPipeline.shaderProgram, "model"), 1, GL_FALSE, tr::identity().data());
            colorPipeline.drawCall(gpuAxis, GL_LINES);


            glUseProgram(phongPipeline.shaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "projection"), 1, GL_FALSE, projection.data()); 
            glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "view"), 1, GL_FALSE, view.data());

            // Sending phong lighting parameters

            // White light in all components: ambient, diffuse and specular.
            glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "La"), 1.0, 1.0, 1.0);
            glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Ld"), 1.0, 1.0, 1.0);
            glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Ls"), 1.0, 1.0, 1.0);

            // Object is barely visible at only ambient. Diffuse behavior is slightly red. Sparkles are white
            glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Ka"), 0.2, 0.2, 0.2);
            glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Kd"), 0.9, 0.9, 0.9);
            glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Ks"), 1.0, 1.0, 1.0);

            // TO DO: Explore different parameter combinations to understand their effect!

            glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "lightPosition"), -5,-5, 5);
            glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "viewPosition"), viewPos[0], viewPos[1], viewPos[2]);
            glUniform1ui(glGetUniformLocation(phongPipeline.shaderProgram, "shininess"), 100);
            
            glUniform1f(glGetUniformLocation(phongPipeline.shaderProgram, "constantAttenuation"), 0.0001);
            glUniform1f(glGetUniformLocation(phongPipeline.shaderProgram, "linearAttenuation"), 0.03);
            glUniform1f(glGetUniformLocation(phongPipeline.shaderProgram, "quadraticAttenuation"), 0.01);
        }

        {
            PROFILE_SCOPE("draw calls", stats);

            // Drawing the shape
            glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "model"), 1, GL_FALSE, modelWhiteDice.data());
            phongPipeline.drawCall(gpuWhiteDice);

            glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "model"), 1, GL_FALSE, modelBlueDice.data());
            phongPipeline.drawCall(gpuBlueDice);
        }

        {
            PROFILE_SCOPE("ImGui", stats);

            /* ImGui Start ---- */

            // render your GUI
            ImGui::Begin("Statistics");

            for (auto result : stats)
            {
                std::stringstream ss;
                ss << result;
                const std::string resultAsString = ss.str();
                ImGui::Text(resultAsString.c_str());
            }
            stats.clear();

            ImGui::End();

            // Render dear imgui into screen
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* ImGui End ---- */
        }
            
        // Once the drawing is rendered, buffers are swap so an uncomplete drawing is never seen.
        glfwSwapBuffers(window);

    }
    
    /* ImGui Start ---- */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    /* ImGui End ---- */

    // freeing GPU memory
    gpuAxis.clear();
    gpuWhiteDice.clear();
    gpuBlueDice.clear();
    
    glfwTerminate();
    return 0;
}