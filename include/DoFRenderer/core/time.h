#pragma once
#include <GLFW/glfw3.h>
#include <chrono>
#include <ctime>

typedef std::chrono::system_clock::time_point timePoint;

namespace DoFRenderer {
	class Timer {
	public:
		Timer() {}
		~Timer() {}
		inline void tick() {
			startTime = glfwGetTime();
		}
		inline void tock() { 
			endTime = glfwGetTime();
		}
		inline double elapsedTime() { 
			return endTime - startTime;
		}
		inline double fps() { 
			return 1 / (endTime - startTime);
		}

	private:
		double startTime;
		double endTime;
	};
}