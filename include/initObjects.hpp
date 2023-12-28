#ifndef INIT_OBJECTS_HPP
#define INIT_OBJECTS_HPP

struct Player;
struct bufferComp;

void generateBuffer(bufferComp &buffer, std::vector<unsigned short> &indices, std::vector<glm::vec3> &indexed_vertices,  std::vector<glm::vec2> &indexed_uvs, std::vector<glm::vec3> &indexed_normals);
void bindBuffer(GLuint buffer, int index, int nbComp);
void bindBuffers(bufferComp &buffer);
void unbindBuffer(unsigned short n);
void bindTexture(GLuint &Texture, GLuint &TextureID);

void generateVAO_VBO(GLuint &VAO, GLuint &VBO);
void init2D(GLuint &EBO);
void initBlock(bufferComp &buffer, std::vector<unsigned short> &iBlock);
Player initPlayer(bufferComp &buffer, std::vector<unsigned short> &iPlayer, glm::vec3 pos_player);

void genBuf_Depth(GLuint &buffer);
void genBuf_Water(GLuint &FBO, GLuint &texture, GLuint &depth);



#endif
