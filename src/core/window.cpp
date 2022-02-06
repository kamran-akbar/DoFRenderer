#include "DoFRenderer/core/window.h"

namespace DoFRenderer {

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {

        std::cout << "y pos: " << ypos;
        std::cout << " x pos: " << xpos << std::endl;
    }

	window::window() : height(600), width(800), name("test window") {}
	
	window::window(unsigned int width, unsigned int height, std::string name) : width(width),
		height(height), name(name) { }

	void window::createWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        instance = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
        
        if (instance == NULL)
        {
            terminateWindow();
            throw "Failed to create GLFW window";    
        }

        glfwMakeContextCurrent(instance);
        glfwSetFramebufferSizeCallback(instance, framebufferSizeCallback);
        glfwSetCursorPosCallback(instance, cursorPosCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw "Failed to initialize GLAD";
        }
	}

    void window::terminateWindow() {
        glfwTerminate();
    }

    void window::processInput() {
        if (glfwGetKey(instance, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(instance, true);
    }

    void window::swapChainCall() {
        glfwSwapBuffers(instance);
        glfwPollEvents();
    }

    GLFWwindow* window::getWindowInstance() {
        return instance;
    }

    bool window::checkWindowClosed() {
        return glfwWindowShouldClose(instance);
    }

    float window::getAspectRatio() const{
        return (float)width / (float)height;
    }
    
}