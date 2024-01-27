#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <array>

#include "constants.hpp"
#include "player.hpp"

struct CameraDirections {
    glm::vec3 direction;
    glm::vec3 directionUp;
};


CameraDirections calculateDirection(GLFWwindow* window, bool thirdView, float deltaTime, bool isVertical);
ViewComp controlsView(GLFWwindow *window, float &deltaTime, Player &player);
Frustum createFrustum(ViewComp viewComp);

void movePlayer(GLFWwindow* window, float &deltaTime, Player &player, glm::vec3 &direction);


#endif