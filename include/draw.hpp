#ifndef DRAW_HPP
#define DRAW_HPP

void initInstancing(GLuint &VAO);

void drawBlockFace(int face, int nbFace=0);
void drawBlockFace_Instanced(int face, int nbEl, int nbFace=0);

void showMap(ViewComp viewComp, Player &player, TextureArray &textureArray, GLuint &VAO, float up=0.0f);
void showMapBlock(std::vector<BlockInfo> &l_block, Frustum &frustum, glm::mat4 ProjView, glm::vec3 eye, GLuint &VAO, float up);
void showFoliage(std::vector<BlockInfo> &l_block, Frustum &frustum, glm::mat4 ProjView, glm::vec3 eye, GLuint &VAO, float up);

void showWaterLow( ViewComp &viewComp, Player &player, GLuint &VAO);
void showWaterHigh(ViewComp viewComp, Player &player, WaterComp waterComp, GLuint &VAO);
void instancedRendering(std::string attribute, float sizeType, int n, int location);

#endif