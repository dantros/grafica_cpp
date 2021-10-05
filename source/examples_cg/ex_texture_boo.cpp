/**
 * @file ex_texture_boo.cpp
 * @brief Textures and transformations in 2D
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <ciso646>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <grafica/root_directory.h>
#include <grafica/shape.h>
#include <grafica/basic_shapes.h>
#include <grafica/load_shaders.h>
#include <grafica/easy_shaders.h>
#include <grafica/gpu_shape.h>
#include <grafica/transformations.h>

namespace gr = Grafica;
namespace tr = Grafica::Transformations;

// A global variable to control the application
struct Controller
{
    bool fillPolygon = true;
} controller;

// This function will be executed whenever a key is pressed or released
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        controller.fillPolygon = not controller.fillPolygon;
    }
}

int main()
{
    // Initialize glfw
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ex_texture_boo", NULL, NULL);
    if (window == NULL)
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

    // Creating our shader program and telling OpenGL to use it
    gr::TextureTransformShaderProgram pipeline;
    glUseProgram(pipeline.shaderProgram);

    // Creating shapes on GPU memory
    gr::GPUShape gpuBoo = gr::toGPUShape(pipeline, gr::createTextureQuad());
    gpuBoo.texture = gr::textureSimpleSetup(
        gr::getPath("assets/imgs/boo.png"), GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST);

    gr::GPUShape gpuQuestionBoxes = gr::toGPUShape(pipeline, gr::createTextureQuad(10, 1));
    gpuQuestionBoxes.texture = gr::textureSimpleSetup(
        gr::getPath("assets/imgs/cg_box.png"), GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);

    std::cout << gpuBoo << std::endl;
    std::cout << gpuQuestionBoxes << std::endl;

    // Enabling transparencies
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setting up the clear screen color
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    // Computing some transformations
    float theta, tx, ty, dtx;
    gr::Matrix4f questionBoxesTransform = tr::translate(0, -0.8, 0) * tr::scale(2, 0.2, 1);

    // Application loop
    while (!glfwWindowShouldClose(window))
    {
        // Using GLFW to check and process input events
        glfwPollEvents();

        // Filling or not the shapes depending on the controller state
        glPolygonMode(GL_FRONT_AND_BACK, controller.fillPolygon ? GL_FILL : GL_LINE);

        // Getting the time difference from the previous iteration
        theta = glfwGetTime();
        tx = 0.7 * std::sin(0.5 * theta);
        ty = 0.2 * std::sin(5 * theta);
    
        // derivative of tx give us the direction
        // dtx = 0.7 * 0.5 * std::cos(0.5 * theta); // only 'cos' contributes to the sign
        dtx = std::cos(0.5 * theta);
        
        gr::Matrix4f reflex;
        if (dtx > 0)
            reflex = tr::identity();
        else
            reflex = tr::scale(-1, 1, 1);

        glClear(GL_COLOR_BUFFER_BIT);

        gr::Matrix4f booTransform = tr::translate(tx, ty, 0) * tr::scale(0.5, 0.5, 1.0) * reflex;
        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "transform"), 1, GL_FALSE, booTransform.data());
        pipeline.drawCall(gpuBoo);

        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "transform"), 1, GL_FALSE, questionBoxesTransform.data());
        pipeline.drawCall(gpuQuestionBoxes);

        // Once the drawing is rendered, buffers are swap so an uncomplete drawing is never seen.
        glfwSwapBuffers(window);
    }

    // freeing GPU memory
    gpuBoo.clear();
    gpuQuestionBoxes.clear();
    
    glfwTerminate();
    return 0;
}