#include "common/include.hpp"

#include "include/player.hpp"
#include "include/buffer.hpp"
#include "include/compObj.hpp"
#include "include/constants.hpp"
#include "include/struct.hpp"

#include "include/globalVar.hpp"

#include "include/sky.hpp"

void renderSky(Player &player, GLuint &VAO){
    GLuint timeID;
    if(isSky){
        glUseProgram(shaderSkyHigh);

        GLuint angleViewID = glGetUniformLocation(shaderSkyHigh, "angleView");
        GLuint resolutionID = glGetUniformLocation(shaderSkyHigh, "u_resolution");
        timeID = glGetUniformLocation(shaderSkyHigh, "u_time");

        glUniform2f(angleViewID, player.angleView, player.angleVertical); 
        glUniform2f(resolutionID, width, height);
    }else{
        glUseProgram(shaderSkyLow);

        timeID = glGetUniformLocation(shaderSkyLow, "u_time");

    }
    glUniform1f(timeID, glfwGetTime()*flowTime); 

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces);
    glBindVertexArray(0);
}