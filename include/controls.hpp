#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <array>
#include "player.hpp"
#include "constants.hpp"
#include "world.hpp"

struct CameraDirections {
    glm::vec3 direction;
    glm::vec3 directionUp;
};


CameraDirections calculateDirection(GLFWwindow* window, bool thirdView, float deltaTime, bool isVertical);
ViewComp controlsView(GLFWwindow* window, float &deltaTime, Player &player, std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> &chunkHeightMap, std::array<int, chunkLoadSize> &listIndex);
void movePlayer(GLFWwindow* window, float &deltaTime, Player &player, glm::vec3 &direction);
glm::mat4 ModelAnimation(Player &player, unsigned short &portionName, bool &isLeft);
void collisionGeneral(Player &player, std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> chunkHeightMap, float &deltaTime, glm::vec3 posBeforeCollision, std::array<int, chunkLoadSize> &listIndex); 
#endif