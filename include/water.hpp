#ifndef WATER_HPP
#define WATER_HPP

void setupWater(ViewComp &viewComp, GLuint &reflectionFBO, GLuint &refractionFBO, Player &player, TextureArray &textureArray,GLuint &VAO_Block, GLuint &VAO_Sky);
void renderWater(WaterComp &waterComp, Player &player, ViewComp &viewComp, GLuint &VAO);

#endif