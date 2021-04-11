/**
 * @file ex_quad_pro.cpp
 * @brief Drawing a Quad via a EBO using some convenience infrastructure
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include <iostream>
#include <string>
#include <vector>
#include <ciso646>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <grafica/shape.h>
#include <grafica/basic_shapes.h>
#include <grafica/load_shaders.h>
#include <grafica/easy_shaders.h>
#include <grafica/gpu_shape.h>
#include <grafica/transformations.h>

namespace gr = Grafica;

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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // settings
    constexpr unsigned int SCR_WIDTH = 600;
    constexpr unsigned int SCR_HEIGHT = 600;

    // Creating a glfw window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ex_quad", NULL, NULL);
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
    gr::SimpleShaderProgram pipeline;
    glUseProgram(pipeline.shaderProgram);

    // Setting up the clear screen color
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    // Creating shapes on GPU memory
    gr::GPUShape gpuShape = gr::toGPUShape(pipeline, gr::createRainbowQuad());
    
    // Application loop
    while (!glfwWindowShouldClose(window))
    {
        // Using GLFW to check and process input events
        glfwPollEvents();

        // Filling or not the shapes depending on the controller state
        glPolygonMode(GL_FRONT_AND_BACK, controller.fillPolygon ? GL_FILL : GL_LINE);

        // Clearing the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Drawing
        pipeline.drawCall(gpuShape);
 
        // Once the drawing is rendered, buffers are swap so an uncomplete drawing is never seen.
        glfwSwapBuffers(window);
    }

    // freeing GPU memorygpuShape.clear();

    glfwTerminate();
    return 0;
}