#pragma once
#include <chrono>
#include <ctime>

typedef std::chrono::system_clock::time_point timePoint;

namespace DoFRenderer {
	class Timer {
	public:
		Timer() {}
		~Timer() {}
		inline void tick() {
			startTime = std::chrono::system_clock::now();
		}
		inline void tock() { 
			endTime = std::chrono::system_clock::now();
		}
		inline double elapsedTime() { 
			std::chrono::duration<double> elapsed_seconds = endTime - startTime;
			return elapsed_seconds.count();
		}
		inline double fps() { 
			std::chrono::duration<double> elapsed_seconds = endTime - startTime;
			return 1 / elapsed_seconds.count();
		}

	private:
		timePoint startTime;
		timePoint endTime;
	};
}