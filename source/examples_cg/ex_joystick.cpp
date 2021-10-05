/**
 * @file ex_joystick.cpp
 * @brief Using a joystick with GLFW
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <bitset>
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

/* This function will be executed whenever a key is pressed or released */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        // The joystick was connected
        std::cout << "The joystick " << jid << " was connected" << std::endl;
    }
    else if (event == GLFW_DISCONNECTED)
    {
        // The joystick was disconnected
        std::cout << "The joystick " << jid << " was disconnected" << std::endl;
    }
}

/* A simple struct handling all axes and button inputs of a joystick. */
struct Joystick
{
    std::vector<bool> buttons;
    std::vector<float> axes;

    Joystick(std::size_t const numberOfButtons, std::size_t const numberOfAxes):
        buttons(numberOfButtons, false), axes(numberOfAxes, 0.0f)
    {
    }
};

std::ostream& operator<<(std::ostream& os, const Joystick& joystick)
{
    os << "buttons: ";
    for (bool button : joystick.buttons)
    {
        os << (button ? 1 : 0);
    }
    os << " axes: ";
    for (float ax : joystick.axes)
    {
        os << std::fixed << std::setprecision(2) << std::setfill('0') << ax << " ";
    }
    return os;
}

/* A convenient type to identify specific joysticks. */
using JoystickId = std::size_t;

/* Convenient storage for all joysticks. They can be accessed by their id.
 * Do note that due to connections and disconnections, the joystick ids may not be succesive numbers.
 * This way, to use an std::vector would require additional logic. */
using JoystickContainer = std::unordered_map<JoystickId, Joystick>;

/* joysticks is not const as we detect connections and disconnections of joystics. */
void pollJoysticks(JoystickContainer& joysticks)
{
    for (int joystickId = GLFW_JOYSTICK_1; joystickId < GLFW_JOYSTICK_LAST; ++joystickId)
    {
        int const joystickConnected = glfwJoystickPresent(joystickId);
        if (joystickConnected == GLFW_FALSE)
        {
            if (joysticks.count(joystickId) != 0)
            {
                /* joystickId has been disconnected.*/
                joysticks.erase(joystickId);
            }
            continue;
        }

        // At this point we have a joystick with this id.
        if (joysticks.count(joystickId) == 0)
        {
            /* joystickId has been connected.*/

            int buttonsCount;
            glfwGetJoystickButtons(joystickId, &buttonsCount);

            int axesCount;
            glfwGetJoystickAxes(joystickId, &axesCount);

            joysticks.emplace(joystickId, Joystick(buttonsCount, axesCount));
        }

        Joystick& joystick = joysticks.at(joystickId);

        int buttonsCount;
        const unsigned char* buttons = glfwGetJoystickButtons(joystickId, &buttonsCount);
        assert(joystick.buttons.size() == buttonsCount);

        for (int buttonId = 0; buttonId < buttonsCount; ++buttonId)
        {
            joystick.buttons[buttonId] = buttons[buttonId] == GLFW_PRESS;
        }

        int axesCount;
        const float* axes = glfwGetJoystickAxes(joystickId, &axesCount);
        assert(joystick.axes.size() == axesCount);

        for (int axesId = 0; axesId < axesCount; ++axesId)
        {
            joystick.axes[axesId] = axes[axesId];
        }
    }
}

void drawJoysticks(const JoystickContainer& joysticks, const gr::GPUShape& gpuButtonOff, const gr::GPUShape& gpuButtonOn, const gr::TransformShaderProgram& pipeline)
{
    for (auto& elem : joysticks)
    {
        auto& joystickId = elem.first;
        auto& joystick = elem.second;

        float const buttonSize = 2.0f / joystick.buttons.size();

        float const verticalOffset = -1.0f + 3.0f * buttonSize * (joystickId + 1);

        for (std::size_t buttonId = 0; buttonId < joystick.buttons.size(); ++buttonId)
        {
            gr::Matrix4f transform =
                tr::translate(-1.0f + (0.5f * buttonSize) + buttonId * buttonSize, verticalOffset, 0) *
                tr::uniformScale(buttonSize);
            glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "transform"), 1, GL_FALSE, transform.data());

            if (joystick.buttons[buttonId])
            {
                pipeline.drawCall(gpuButtonOn);
            }
            else
            {
                pipeline.drawCall(gpuButtonOff);
            }

        }

        for (std::size_t axesId = 0; axesId < joystick.axes.size(); ++axesId)
        {
            gr::Matrix4f transformBackground =
                tr::translate(-1.0f + (0.5f * buttonSize) + axesId * buttonSize, verticalOffset + buttonSize, 0) *
                tr::uniformScale(buttonSize);
            glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "transform"), 1, GL_FALSE, transformBackground.data());
            pipeline.drawCall(gpuButtonOff);

            gr::Matrix4f transform =
                tr::translate(-1.0f + (0.5f * buttonSize) + axesId * buttonSize, verticalOffset + buttonSize + (joystick.axes[axesId] * buttonSize * 0.5f), 0) *
                tr::uniformScale(buttonSize * 0.5);
            glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, "transform"), 1, GL_FALSE, transform.data());
            pipeline.drawCall(gpuButtonOn);
        }
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
    std::string title = "ex_joystick";
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

    JoystickContainer joysticks;

    glfwSetJoystickCallback(joystick_callback);

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
    gr::GPUShape gpuButtonOff = gr::toGPUShape(pipeline, gr::createColorCircle(20, 0.5f, 0.5f, 0.5f));
    gr::GPUShape gpuButtonOn = gr::toGPUShape(pipeline, gr::createColorCircle(20, 0.0f, 1.0f, 0.0f));

    gr::PerformanceMonitor performanceMonitor(glfwGetTime(), 0.5f);
    // glfw will swap buffers as soon as possible
    glfwSwapInterval(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

        pollJoysticks(joysticks);

        // Uncomment to print the joysticks' status to the standard output.
        /*for (auto elem : joysticks)
        {
            auto& joystickId = elem.first;
            auto& joystick = elem.second;
            std::cout << joystickId << ") " << joystick << std::endl;
        }*/

        // Clearing the screen
        glClear(GL_COLOR_BUFFER_BIT);

        drawJoysticks(joysticks, gpuButtonOff, gpuButtonOn, pipeline);

        // Once the drawing is rendered, buffers are swap so an uncomplete drawing is never seen.
        glfwSwapBuffers(window);
    }

    // freeing GPU memory
    gpuButtonOn.clear();
    gpuButtonOff.clear();

    glfwTerminate();
    return 0;
}