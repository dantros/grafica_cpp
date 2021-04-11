/**
 * @file ex_4shapes.cpp
 * @brief Drawing 4 shapes with different transformations
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <ciso646>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <grafica/shape.h>
#include <grafica/basic_shapes.h>
#include <grafica/load_shaders.h>
#include <grafica/performance_monitor.h>
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // Creating a glfw window
    constexpr unsigned int SCR_WIDTH = 600;
    constexpr unsigned int SCR_HEIGHT = 600;
    std::string title = "ex_4shapes";
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), NULL, NULL);
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
    gr::TransformShaderProgram pipeline;
    glUseProgram(pipeline.shaderProgram);

    // Setting up the clear screen color
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    // Creating shapes on GPU memory
    gr::GPUShape gpuTriangle = gr::toGPUShape(pipeline, gr::createRainbowTriangle());
    gr::GPUShape gpuQuad = gr::toGPUShape(pipeline, gr::createRainbowQuad());

    gr::PerformanceMonitor performanceMonitor(glfwGetTime(), 0.5f);
    // glfw will swap buffers as soon as possible
    glfwSwapInterval(0);

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

        // Clearing the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Using the time as the theta parameter
        float theta = glfwGetTime();

        // Triangle
        gr::Matrix4f triangleTransform = 
            tr::translate(0.5, 0.5, 0) * 
            tr::rotationZ(2 * theta) * 
            tr::uniformScale(0.5);

        // Updating the transform attribute
        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "transform"), 1, GL_FALSE, triangleTransform.data());
        
        // Drawing function
        pipeline.drawCall(gpuTriangle);

        // Another instance of the triangle
        gr::Matrix4f triangleTransform2 = 
            tr::translate(-0.5, 0.5, 0) *
            tr::scale(
                0.5 + 0.2 * std::cos(1.5 * theta),
                0.5 + 0.2 * std::sin(2 * theta),
                0);

        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "transform"), 1, GL_FALSE, triangleTransform2.data());
        pipeline.drawCall(gpuTriangle);

        //Quad
        gr::Matrix4f quadTransform = 
            tr::translate(-0.5, -0.5, 0) *
            tr::rotationZ(-theta) *
            tr::uniformScale(0.7);

        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "transform"), 1, GL_FALSE, quadTransform.data());
        pipeline.drawCall(gpuQuad);

        // Another instance of the Quad
        gr::Matrix4f quadTransform2 =
            tr::translate(0.5, -0.5, 0) *
            tr::shearing(0.3 * std::cos(theta), 0, 0, 0, 0, 0) *
            tr::uniformScale(0.7);

        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "transform"), 1, GL_FALSE, quadTransform2.data());
        pipeline.drawCall(gpuQuad);

        // Once the drawing is rendered, buffers are swap so an uncomplete drawing is never seen.
        glfwSwapBuffers(window);
    }

    // freeing GPU memory
    gpuTriangle.clear();
    gpuQuad.clear();

    glfwTerminate();
    return 0;
}