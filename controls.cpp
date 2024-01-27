#include "common/include.hpp"

#include "include/struct.hpp"
#include "include/globalVar.hpp"
#include "include/calcul.hpp"

#include "include/controls.hpp"

float horizontalAngleF5 = 0;
float verticalAngleF5 = 0;

bool isPressF5 = false;
bool isPressF3 = false;


const int dist_cam = 0.6+4.4*SCALE; // Be careful is you decrease SCALE, dist_cam should be > 1
float limF5 = 1.5708f;
float oldAngleChest = -1; // Different of 0
float isRoundTrip = true;

CameraDirections calculateDirection(GLFWwindow *window, bool thirdView, float deltaTime){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, width / 2, height / 2);

    float decX = mouseSpeed * deltaTime * float(width / 2 - xpos);
    float decY = mouseSpeed * deltaTime * float(height / 2 - ypos);

    horizontalAngleF5 += decX;
    verticalAngleF5 -= (-1.5708f <= verticalAngleF5 - decY && verticalAngleF5 - decY <= 1.5708f) ? decY : 0;

    // TODO : CREATE MOD && THIRDVIEW
    glm::vec3 direction = (thirdView) ? glm::vec3(-sin(horizontalAngleF5), 0, -cos(horizontalAngleF5)) : glm::vec3(cos(-verticalAngleF5) * sin(horizontalAngleF5), sin(-verticalAngleF5), cos(-verticalAngleF5) * cos(horizontalAngleF5));
    glm::vec3 directionUp = (thirdView || CREATE_MOD) ? direction : glm::vec3( sin(horizontalAngleF5), 0., cos(horizontalAngleF5));

    CameraDirections camDir;
    camDir.direction = direction;
    camDir.directionUp = directionUp;

    return camDir;
}

ViewComp controlsView(GLFWwindow *window, float &deltaTime, Player &player){
    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && !isPressF5){
        isPressF5 = true;
        player.thirdView = !player.thirdView;
    }else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE && isPressF5){
        isPressF5 = false;
    }if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && !isPressF3){
        // isF3 = !isF3; // Steve is not there yet, so we can't change the view
        isPressF3 = true;
    }else if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE && isPressF3){
        isPressF3 = false;
    }

    CameraDirections camDir = calculateDirection(window, player.thirdView, deltaTime);

    glm::vec3 posBeforeCollision = player.pos;
    movePlayer(window, deltaTime, player, camDir.directionUp);
    player.actualize(); 
    // collisionGeneral(player, chunkHeightMap, deltaTime, posBeforeCollision, listIndex);

    glm::vec3 posHead = glm::vec3(player.pos.x, player.pos.y + 2*SCALE, player.pos.z); // 2 is the steve's height
    ViewComp viewComp;
    player.angleHead = horizontalAngleF5;
    player.angleView = modFloat(horizontalAngleF5, 6.28);
    player.angleVertical = modFloat(verticalAngleF5, 6.28);

    if (!player.thirdView){
        viewComp = {
            posHead,
            camDir.direction
        };
    }
    else{
        glm::vec3 posCam(
            posHead.x + dist_cam * sin(horizontalAngleF5),
            posHead.y + dist_cam * sin(verticalAngleF5),
            posHead.z + dist_cam * cos(horizontalAngleF5));

        viewComp = {
            posCam,
            posHead - posCam 
        };
    }
    return viewComp;
}

Frustum createFrustum(ViewComp viewComp){
	float zNear = 0;
	float zFar = 100;
	float aspect = width/height;

    const float halfVSide = zFar * tanf( FoV*.5f);
    const float halfHSide = halfVSide * aspect;
	glm::vec3 dirRight = glm::normalize(glm::cross(glm::vec3(0,1,0), viewComp.dir));
	glm::vec3 dirUp = (glm::cross(viewComp.dir, dirRight));
    const glm::vec3 frontMultFar = zFar * dirUp;

    Frustum frustum;
    frustum.nearFace = {viewComp.dir};
    frustum.rightFace = {glm::cross(frontMultFar - dirRight * halfHSide, dirUp)};
    frustum.leftFace = {glm::cross(dirUp, frontMultFar + dirRight * halfHSide)};
    frustum.topFace = {glm::cross(dirRight, frontMultFar - dirUp * halfVSide)};
    frustum.bottomFace = {glm::cross(frontMultFar + dirUp * halfVSide, dirRight)};

    return frustum;
}




void movePlayer(GLFWwindow *window, float &deltaTime, Player &player, glm::vec3 &direction){
    glm::vec3 right = (player.thirdView) ? glm::vec3(sin(horizontalAngleF5 - 3.1415f / 2.0f), 0, cos(horizontalAngleF5 - 3.1415f / 2.0f)) : glm::vec3(sin(horizontalAngleF5 - 3.1415f / 2.0f), 0, cos(horizontalAngleF5 - 3.1415f / 2.0f));

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        player.pos += direction * deltaTime * speed;

        player.angleChest = player.angleHead;
        if (player.angleChest != oldAngleChest){
            float meanLim = limF5 - 1.5708f;

            limF5 += player.angleHead - meanLim;
        }
        oldAngleChest = player.angleChest;

        player.angleArm += (isRoundTrip) ? 6 * deltaTime : -6 * deltaTime;
        player.angleLeg += (isRoundTrip) ? 4 * deltaTime : -4 * deltaTime;

        if (abs(player.angleArm) > 1){
            isRoundTrip = !isRoundTrip;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE){ // reset animation
        player.angleArm = 0;
        player.angleLeg = 0;
    }if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        player.pos -= direction * deltaTime * speed;
    }

    glm::vec3 rds = right * deltaTime * speed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        player.pos += (player.thirdView) ? -rds : rds;
    }if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        player.pos -= (player.thirdView) ? -rds : rds;
    }
}
