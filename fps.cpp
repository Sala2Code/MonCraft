#include "common/include.hpp"
#include "include/fps.hpp"


void fps(GLFWwindow *window, double &lastTime, double &lastTimeFPS, unsigned int &n_frame, float &deltaTime){
	    double currentTime = glfwGetTime();
		n_frame++;
		if(currentTime - lastTimeFPS >=1){
			lastTimeFPS+=1;
			std::string title = "MonCraft " + std::to_string(n_frame);
			glfwSetWindowTitle(window, title.c_str());
			n_frame = 0;
		}
        deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;
}

bool isPressP = false;
void paused(GLFWwindow *window , bool &isPaused){
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !isPressP){
		isPaused = !isPaused;
		isPressP = true;
	}else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && isPressP){
		isPressP = false;
	}
}