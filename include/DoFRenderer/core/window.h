#pragma once 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

namespace DoFRenderer {
	class window {
	public:
		//Constructors
		window();
		window(unsigned int width, unsigned int height, std::string name);

		//Methods
		void createWindow();
		void terminateWindow();
		void processInput();
		void swapChainCall();
		GLFWwindow* getWindowInstance();
		bool checkWindowClosed();
		float getAspectRatio() const;

	private:
		unsigned int width, height;
		std::string name;
		GLFWwindow* instance;
	};
}