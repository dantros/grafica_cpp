/**
 * @file ex_projections.cpp
 * @brief Projections example
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
#include <grafica/shape.h>
#include <grafica/basic_shapes.h>
#include <grafica/load_shaders.h>
#include <grafica/easy_shaders.h>
#include <grafica/gpu_shape.h>
#include <grafica/transformations.h>

namespace gr = Grafica;
namespace tr = Grafica::Transformations;

enum class ProjectionType{Orthographic, Frustum, Perspective};

// A global variable to control the application
struct Controller
{
    bool fillPolygon = true;
    ProjectionType projectionType = ProjectionType::Orthographic;
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
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        controller.projectionType = ProjectionType::Orthographic;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        controller.projectionType = ProjectionType::Frustum;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        controller.projectionType = ProjectionType::Perspective;
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ex_projections", NULL, NULL);
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
    gr::ModelViewProjectionShaderProgram pipeline;
    glUseProgram(pipeline.shaderProgram);

    // Creating shapes on GPU memory
    gr::GPUShape gpuAxis        = gr::toGPUShape(pipeline, gr::createAxis(7));
    gr::GPUShape gpuRedCube     = gr::toGPUShape(pipeline, gr::createColorCube(1,0,0));
    gr::GPUShape gpuGreenCube   = gr::toGPUShape(pipeline, gr::createColorCube(0,1,0));
    gr::GPUShape gpuBlueCube    = gr::toGPUShape(pipeline, gr::createColorCube(0,0,1));
    gr::GPUShape gpuYellowCube  = gr::toGPUShape(pipeline, gr::createColorCube(1,1,0));
    gr::GPUShape gpuCyanCube    = gr::toGPUShape(pipeline, gr::createColorCube(0,1,1));
    gr::GPUShape gpuPurpleCube  = gr::toGPUShape(pipeline, gr::createColorCube(1,0,1));
    gr::GPUShape gpuRainbowCube = gr::toGPUShape(pipeline, gr::createRainbowCube());

    // Setting up the clear screen color
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    // As we work in 3D, we need to check which part is in front,
    // and which one is at the back enabling the depth testing
    glEnable(GL_DEPTH_TEST);

    // Computing some variables
    float t0 = glfwGetTime(), t1, dt;
	float cameraTheta = std::numbers::pi / 4;

    // Application loop
    while (!glfwWindowShouldClose(window))
    {
        // Using GLFW to check and process input events
        glfwPollEvents();

        // Filling or not the shapes depending on the controller state
        if (controller.fillPolygon)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Getting the time difference from the previous iteration
        t1 = glfwGetTime();
        dt = t1 - t0;
        t0 = t1;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cameraTheta -= 2 * dt;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cameraTheta += 2 * dt;

        gr::Vector3f const viewPos(
            10 * std::sin(cameraTheta),
            10 * std::cos(cameraTheta),
            10);
        gr::Vector3f const eye(0,0,0);
        gr::Vector3f const at(0,0,1);

        gr::Matrix4f view = tr::lookAt(viewPos, eye, at);

        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "view"), 1, GL_FALSE, view.data());

        gr::Matrix4f projection;
        switch (controller.projectionType)
        {
            case ProjectionType::Orthographic:
                projection = tr::ortho(-8, 8, -8, 8, 0.1, 100);
                break;
            case ProjectionType::Frustum:
                projection = tr::frustum(-5, 5, -5, 5, 9, 100);
                break;
            case ProjectionType::Perspective:
                projection = tr::perspective(60, float(SCR_WIDTH)/float(SCR_HEIGHT), 0.1, 100);
                break;
            default:
                throw;
        }

        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "projection"), 1, GL_FALSE, projection.data());

        // Clearing the screen in both, color and depth
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Drawing shapes with different model transformations
        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "model"), 1, GL_FALSE, tr::identity().data());
		pipeline.drawCall(gpuAxis, GL_LINES);

        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "model"), 1, GL_FALSE, tr::translate(5,0,0).data());
        pipeline.drawCall(gpuRedCube);
        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "model"), 1, GL_FALSE, tr::translate(-5,0,0).data());
        pipeline.drawCall(gpuGreenCube);


        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "model"), 1, GL_FALSE, tr::translate(0,5,0).data());
        pipeline.drawCall(gpuBlueCube);
        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "model"), 1, GL_FALSE, tr::translate(0,-5,0).data());
        pipeline.drawCall(gpuYellowCube);


        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "model"), 1, GL_FALSE, tr::translate(0,0,5).data());
        pipeline.drawCall(gpuCyanCube);
        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "model"), 1, GL_FALSE, tr::translate(0,0,-5).data());
        pipeline.drawCall(gpuPurpleCube);

        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "model"), 1, GL_FALSE, tr::identity().data());
        pipeline.drawCall(gpuRainbowCube);

        // Once the drawing is rendered, buffers are swap so an uncomplete drawing is never seen.
        glfwSwapBuffers(window);
    }

    // freeing GPU memory
    gpuAxis.clear();
    gpuRedCube.clear();
    gpuGreenCube.clear();
    gpuBlueCube.clear();
    gpuYellowCube.clear();
    gpuCyanCube.clear();
    gpuPurpleCube.clear();
    gpuRainbowCube.clear();

    glfwTerminate();
    return 0;
}