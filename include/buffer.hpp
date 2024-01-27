#ifndef BUFFER_HPP
#define BUFFER_HPP


void genBuf(GLuint& buffer, GLfloat* compObj, GLsizei size, int e, int d);
void genBuf_Depth(GLuint &buffer);
void genBuf_Water(GLuint &FBO, GLuint &texture, GLuint &depth);

void bindBuffer(GLuint &buffer, int loc, int size);
void bindBuffer(GLuint &vertexbuffer, GLuint &uvbuffer, GLuint &normalbuffer);
void unbindBuffer(unsigned short n);



void bindAndBufferData(GLuint bufferID, const std::vector<float>& data);
void bindAndBufferData(GLuint bufferID, const std::vector<glm::mat4>& data);
void processBuffers(
	GLuint idTexBuffer, const std::vector<float>& instanceTexID, 
	GLuint instanceBuffer, const std::vector<glm::mat4>& instanceMatrices,
	GLuint instanceBufferModels, const std::vector<glm::mat4>& instanceModels
);
void processBuffers(
	GLuint instanceBuffer, const std::vector<glm::mat4>& instanceMatrices,
	GLuint instanceBufferModels, const std::vector<glm::mat4>& instanceModels
);

#endif
