#ifndef DISPLAY_HPP
#define DISPLAY_HPP


void fps(GLFWwindow *window, double &lastTime, double &lastTimeFPS, unsigned int &n_frame, float &deltaTime);
void paused(GLFWwindow *window , bool &isPaused);


#endif
