#include "common/include.hpp"

#include "./include/player.hpp"
#include "include/struct.hpp"

#include "common/VBOindex.hpp"

#include "include/compObjects.hpp"
#include "include/initObjects.hpp"

void generateBuffer(bufferComp &buffer, std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices,  std::vector<glm::vec2> &indexed_uvs, std::vector<glm::vec3> &indexed_normals){
    
	glGenBuffers(1, &buffer.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &buffer.vertex);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.vertex);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &buffer.uv);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.uv);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
	
    glGenBuffers(1, &buffer.normal);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.normal);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

}

void bindBuffer(GLuint buffer, int index, int nbComp){
	glEnableVertexAttribArray(index);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(index, nbComp, GL_FLOAT,GL_FALSE,0,(void*)0);
}

void bindBuffers(bufferComp &buffer){

	bindBuffer(buffer.vertex, 0, 3);
	bindBuffer(buffer.uv, 1, 2);
	bindBuffer(buffer.normal, 2, 3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.EBO);
}

void unbindBuffer(unsigned short n){
	for(unsigned short i=0;i<n;i++){
		glDisableVertexAttribArray(i);
	}
}

void bindTexture(GLuint &Texture, GLuint &TextureID){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glUniform1i(TextureID, 0);
}


void generateVAO_VBO(GLuint &VAO, GLuint &VBO){
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void init2D(GLuint &EBO){
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2D), indices2D, GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2D), vertices2D, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
}

void initBlock(bufferComp &buffer, std::vector<unsigned short> &iBlock){
    std::vector<glm::vec3> verticesBlock;
	std::vector<glm::vec2> uvsBlock;
	std::vector<glm::vec3> normalsBlock;
	// block block(verticesBlock, uvsBlock, normalsBlock); //Generate block
	loadOBJ("obj/cube.obj", verticesBlock, uvsBlock, normalsBlock);

	std::vector<glm::vec3> i_verticesBlock;
	std::vector<glm::vec2> i_uvsBlock;
	std::vector<glm::vec3> i_normalsBlock;

	indexVBO(verticesBlock, uvsBlock, normalsBlock, iBlock, i_verticesBlock, i_uvsBlock, i_normalsBlock);
	generateBuffer(buffer, iBlock, i_verticesBlock, i_uvsBlock, i_normalsBlock);
}

Player initPlayer(bufferComp &buffer, std::vector<unsigned short> &iPlayer, glm::vec3 pos_player){
	std::vector<glm::vec3> verticesPlayer;
	std::vector<glm::vec2> uvsPlayer;
	std::vector<glm::vec3> normalsPlayer;
	Player player(pos_player, verticesPlayer, uvsPlayer, normalsPlayer); 

	std::vector<glm::vec3> i_verticesPlayer;
	std::vector<glm::vec2> i_uvsPlayer;
	std::vector<glm::vec3> i_normalsPlayer;
	indexVBO(verticesPlayer, uvsPlayer, normalsPlayer, iPlayer, i_verticesPlayer, i_uvsPlayer, i_normalsPlayer);
	generateBuffer(buffer, iPlayer, i_verticesPlayer, i_uvsPlayer, i_normalsPlayer);

	// * Hit box
	GLuint EBO_HitBox;
	GLuint vertexbufferHitBox;
	GLuint uvbufferHitBox;
	GLuint normalbufferHitBox;
	player.bufferHitBox = {EBO_HitBox, vertexbufferHitBox, uvbufferHitBox, normalbufferHitBox};


	indexVBO(player.verticesHitBox, player.uvsHitBox, player.normalsHitBox, player.iHitBox, player.i_verticesHitBox, player.i_uvsHitBox, player.i_normalsHitBox);
	generateBuffer(player.bufferHitBox, player.iHitBox, player.i_verticesHitBox, player.i_uvsHitBox, player.i_normalsHitBox);
	return player;
}

void genBuf_Depth(GLuint &buffer){
	glGenRenderbuffers(1, &buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer);
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
