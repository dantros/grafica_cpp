/**
 * @file ex_lighting.cpp
 * @brief Showing lighting effects using Phong
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

// A global variable to control the application
struct Controller
{
    bool fillPolygon = true;
    bool showAxis = true;
    int shapeIndex = 0;
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
    else if (key == GLFW_KEY_UP)
    {
        controller.shapeIndex--;
    }
    else if (key == GLFW_KEY_DOWN)
    {
        controller.shapeIndex++;
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ex_lighting", NULL, NULL);
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

    // Creating our shader programs
    gr::ModelViewProjectionShaderProgram colorPipeline;
    gr::PhongColorShaderProgram phongPipeline;

    // Creating shapes on GPU memory
    gr::GPUShape gpuAxis = gr::toGPUShape(colorPipeline, gr::createAxis(7));

    std::vector<gr::GPUShape> gpuShapes
    {
        gr::toGPUShape(phongPipeline, gr::createRainbowNormalsCube()),
        gr::toGPUShape(phongPipeline, gr::createColorNormalsCube(1,0,0)),
        gr::toGPUShape(phongPipeline, gr::createColorNormalsCube(0,1,0)),
        gr::toGPUShape(phongPipeline, gr::createColorNormalsCube(0,0,1)),
        gr::toGPUShape(phongPipeline, gr::createColorNormalsCube(1,1,0)),
        gr::toGPUShape(phongPipeline, gr::createColorNormalsCube(0,1,1)),
        gr::toGPUShape(phongPipeline, gr::createColorNormalsCube(1,0,1)),
        gr::toGPUShape(phongPipeline, gr::createColorNormalsCube(1,1,1)),
        gr::toGPUShape(phongPipeline, gr::createColorNormalsCube(0.5,0.5,0.5)),
        gr::toGPUShape(phongPipeline, gr::createColorNormalsCube(0.1,0.1,0.1)),
    };

    // Setting up the clear screen color
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    // As we work in 3D, we need to check which part is in front,
    // and which one is at the back enabling the depth testing
    glEnable(GL_DEPTH_TEST);

    // Computing some transformations
    float t0 = glfwGetTime(), t1, dt;
	float cameraTheta = std::numbers::pi / 4;

    gr::Matrix4f projection = tr::perspective(45, float(SCR_WIDTH)/float(SCR_HEIGHT), 0.1, 100);
    gr::Matrix4f model = tr::identity();

    // Application loop
    while (!glfwWindowShouldClose(window))
    {
        // Using GLFW to check and process input events
        glfwPollEvents();

        // Filling or not the shapes depending on the controller state
        glPolygonMode(GL_FRONT_AND_BACK, controller.fillPolygon ? GL_FILL : GL_LINE);

        // Getting the time difference from the previous iteration
        t1 = glfwGetTime();
        dt = t1 - t0;
        t0 = t1;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            cameraTheta -= 2 * dt;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cameraTheta += 2 * dt;

        gr::Vector3f const viewPos(
            3 * std::sin(cameraTheta),
            3 * std::cos(cameraTheta),
            2);
        gr::Vector3f const eye(0,0,0);
        gr::Vector3f const at(0,0,1);

        gr::Matrix4f view = tr::lookAt(viewPos, eye, at);

        //Clearing the screen in both, color and depth
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Drawing shapes with different uniforms and different shader programs
        glUseProgram(colorPipeline.shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(colorPipeline.shaderProgram, "view"), 1, GL_FALSE, view.data());
        glUniformMatrix4fv(glGetUniformLocation(colorPipeline.shaderProgram, "projection"), 1, GL_FALSE, projection.data());
        glUniformMatrix4fv(glGetUniformLocation(colorPipeline.shaderProgram, "model"), 1, GL_FALSE, tr::identity().data());
		colorPipeline.drawCall(gpuAxis, GL_LINES);

        // Cyclic rotation over shapes.
        if (controller.shapeIndex == gpuShapes.size())
            controller.shapeIndex = 0;
        else if (controller.shapeIndex < 0)
            controller.shapeIndex = gpuShapes.size() - 1;

        // Getting the shape to display
        const gr::GPUShape& shapeToDisplay = gpuShapes.at(controller.shapeIndex);

        glUseProgram(phongPipeline.shaderProgram);

        // Sending MVP matrices
        glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "projection"), 1, GL_FALSE, projection.data()); 
        glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "view"), 1, GL_FALSE, view.data());
        glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "model"), 1, GL_FALSE, model.data());

        // Sending phong lighting parameters

        // White light in all components: ambient, diffuse and specular.
        glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "La"), 1.0, 1.0, 1.0);
        glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Ld"), 1.0, 1.0, 1.0);
        glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Ls"), 1.0, 1.0, 1.0);

        // Object is barely visible at only ambient. Diffuse behavior is slightly red. Sparkles are white
        glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Ka"), 0.2, 0.2, 0.2);
        glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Kd"), 0.9, 0.5, 0.5);
        glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "Ks"), 1.0, 1.0, 1.0);

        // TO DO: Explore different parameter combinations to understand their effect!

        glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "lightPosition"), -5, -5, 5);
        glUniform3f(glGetUniformLocation(phongPipeline.shaderProgram, "viewPosition"), viewPos[0], viewPos[1], viewPos[2]);
        glUniform1ui(glGetUniformLocation(phongPipeline.shaderProgram, "shininess"), 100);
        
        glUniform1f(glGetUniformLocation(phongPipeline.shaderProgram, "constantAttenuation"), 0.0001);
        glUniform1f(glGetUniformLocation(phongPipeline.shaderProgram, "linearAttenuation"), 0.03);
        glUniform1f(glGetUniformLocation(phongPipeline.shaderProgram, "quadraticAttenuation"), 0.01);

        glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "projection"), 1, GL_FALSE, projection.data());
        glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "view"), 1, GL_FALSE, view.data());
        glUniformMatrix4fv(glGetUniformLocation(phongPipeline.shaderProgram, "model"), 1, GL_FALSE, model.data());

        phongPipeline.drawCall(shapeToDisplay);

        // Once the drawing is rendered, buffers are swap so an uncomplete drawing is never seen.
        glfwSwapBuffers(window);
    }

    // freeing GPU memory
    gpuAxis.clear();
    for (auto& gpuShape : gpuShapes)
        gpuShape.clear();
    
    glfwTerminate();
    return 0;
}