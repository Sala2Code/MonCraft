#ifndef HITBOXF3_HPP
#define HITBOXF3_HPP

struct Player;
struct BlockMap;
struct BlockInfo;
struct BlockShader;
struct bufferComp;

void showHitBox(Player &player, glm::mat4 Projection, glm::mat4 View, GLuint MatrixID, GLuint ModelMatrixID);
void show2D(GLuint EBO);
void showPlayer(Player player, bufferComp &bufferPlayer, glm::mat4 ProjView, GLuint shader, bool isDisplay);

void showMap( bufferComp &buffer, Player player, BlockShader &blockShader, std::vector<unsigned short> &iBlock, float up, std::vector<BlockInfo> &worldMap);
void showMapBlock(std::vector<BlockInfo> &worldMap,GLuint shader,BlockShader blockShader,std::vector<unsigned short> &iBlock,Player &player, float up);
void showWater( bufferComp &buffer, Player player, BlockShader &blockShader, std::vector<unsigned short> &iBlock, float up, std::vector<BlockInfo> &worldMap);

void instancedRendering(GLuint shader, std::string attribute, float sizeType, int n, int location);

#endif