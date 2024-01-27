#include "common/include.hpp"

#include "include/constants.hpp"

#include "include/buffer.hpp"

void genBuf(GLuint& buffer, GLfloat* compObj, GLsizei size, int e, int d) {
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, compObj, GL_STATIC_DRAW);
	glVertexAttribPointer(e, d, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(e);
}

void genBuf_Depth(GLuint &buffer){
	glGenRenderbuffers(1, &buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void genBuf_Water(GLuint &FBO, GLuint &texture, GLuint &depthbuffer){
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bindBuffer(GLuint &buffer, int loc, int size){
	glEnableVertexAttribArray(loc);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(loc, size, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void bindBuffer(GLuint &vertexbuffer, GLuint &uvbuffer, GLuint &normalbuffer){
	bindBuffer(vertexbuffer, 0, 3);
	bindBuffer(uvbuffer, 1, 2);
	bindBuffer(normalbuffer, 2, 3);
}

void unbindBuffer(unsigned short n){
	for(unsigned short i=0;i<n;i++){
		glDisableVertexAttribArray(i);
	}
}

// draw.cpp
void bindAndBufferData(GLuint bufferID, const std::vector<float>& data) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void bindAndBufferData(GLuint bufferID, const std::vector<glm::mat4>& data) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::mat4), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void processBuffers(
	GLuint idTexBuffer, const std::vector<float>& instanceTexID, 
	GLuint instanceBuffer, const std::vector<glm::mat4>& instanceMatrices,
	GLuint instanceBufferModels, const std::vector<glm::mat4>& instanceModels
){
    bindAndBufferData(idTexBuffer, instanceTexID);
    bindAndBufferData(instanceBuffer, instanceMatrices);
    bindAndBufferData(instanceBufferModels, instanceModels);
}

void processBuffers(
	GLuint instanceBuffer, const std::vector<glm::mat4>& instanceMatrices,
	GLuint instanceBufferModels, const std::vector<glm::mat4>& instanceModels
) {
    bindAndBufferData(instanceBuffer, instanceMatrices);
    bindAndBufferData(instanceBufferModels, instanceModels);
}