#include "common/include.hpp"
#include "include/constants.hpp"
#include "include/player.hpp"
#include "include/display.hpp"
#include "include/struct.hpp"


void activeFBO(GLuint &FBO){
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
}

void activeSky(GLuint &shader, GLuint &EBO, float angleView, float angleVertical){
    glUseProgram(shader);

	GLuint angleViewID = glGetUniformLocation(shader, "angleView");
	GLuint resolutionID = glGetUniformLocation(shader, "u_resolution");
	GLuint timeID = glGetUniformLocation(shader, "u_time");

    glUniform2f(angleViewID, angleView, angleVertical); 
    glUniform1f(timeID, glfwGetTime()); 
    glUniform2f(resolutionID, width, height); 
    show2D(EBO);
}
