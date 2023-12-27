#include "common/include.hpp"
#include "include/controls.hpp"
#include "common/calcul.hpp"
#include "include/struct.hpp"

float horizontalAngleF5 = 0;
float verticalAngleF5 = 0;

bool isPressF3 = false;
bool isPressF5 = false;

const int dist_cam = 0.6+4.4*SCALE; // ! Be careful is you decrease SCALE, dist_cam should be > 1
float limF5 = 1.5708f;
float oldAngleChest = -1; // Different of 0
float isRoundTrip = true;

CameraDirections calculateDirection(GLFWwindow *window, bool thirdView, float deltaTime){
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, width / 2, height / 2);

    float decX = mouseSpeed * deltaTime * float(width / 2 - xpos);
    float decY = mouseSpeed * deltaTime * float(height / 2 - ypos);

    horizontalAngleF5 += ((limF5 - 3.14f <= horizontalAngleF5 + decX && horizontalAngleF5 + decX <= limF5) || !thirdView) ? decX : 0;
    verticalAngleF5 -= (-1.5708f <= verticalAngleF5 - decY && verticalAngleF5 - decY <= 1.5708f) ? decY : 0;

    // ! CREATE MOD && THIRDVIEW
    glm::vec3 direction = (thirdView) ? glm::vec3(-sin(horizontalAngleF5), 0, -cos(horizontalAngleF5)) : glm::vec3(cos(-verticalAngleF5) * sin(horizontalAngleF5), sin(-verticalAngleF5), cos(-verticalAngleF5) * cos(horizontalAngleF5));
    glm::vec3 directionUp = (thirdView || CREATE_MOD) ? direction : glm::vec3( sin(horizontalAngleF5), 0., cos(horizontalAngleF5));

    CameraDirections camDir;
    camDir.direction = direction;
    camDir.directionUp = directionUp;

    return camDir;
}

ViewComp controlsView(GLFWwindow *window, float &deltaTime, Player &player, std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> &chunkHeightMap, std::array<int, chunkLoadSize> &listIndex){
    // glm::vec3 up = glm::vec3(0, 1, 0);

    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && !isPressF5){
        player.thirdView = !player.thirdView;
        isPressF5 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE && isPressF5){
        isPressF5 = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && !isPressF3){
        player.showHitBox = !player.showHitBox;
        isPressF3 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE && isPressF3){
        isPressF3 = false;
    }


    CameraDirections camDir = calculateDirection(window, player.thirdView, deltaTime);

    glm::vec3 posBeforeCollision = player.pos;
    movePlayer(window, deltaTime, player, camDir.directionUp);
    collisionGeneral(player, chunkHeightMap, deltaTime, posBeforeCollision, listIndex);

    glm::vec3 posHead = glm::vec3(player.pos.x, player.pos.y + 2*SCALE, player.pos.z); // 2 is the steve's height
    ViewComp viewComp;
    player.angleHead = horizontalAngleF5;
    player.angleView = modFloat(horizontalAngleF5, 6.28);
    player.angleVertical = modFloat(verticalAngleF5, 6.28);

    if (!player.thirdView){
        // posHead -= (up==-1) ? 2*(posHead - WATER_HEIGHT): 0;  // for reflection/refraction, set camera under water
        // View = glm::lookAt(posHead, posHead + camDir.direction, upVec);
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

void movePlayer(GLFWwindow *window, float &deltaTime, Player &player, glm::vec3 &direction)
{
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
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        player.pos -= direction * deltaTime * speed;
    }

    glm::vec3 rds = right * deltaTime * speed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        player.pos += (player.thirdView) ? -rds : rds;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        player.pos -= (player.thirdView) ? -rds : rds;
    }

    // Jump // ! On peut faire de petits jump si on regarde vers le bas
    // ! VERIRIFER LE SCALE
    if(!CREATE_MOD){
        double time = glfwGetTime();
        float diffTime = time - player.takeOff;
        if (diffTime <= 0.3)
        {                                                        // 0.25s to jump
            player.pos.y += SCALE * 30 * deltaTime / (1 + 4 * diffTime); // jump
            player.isOnFloor = false;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS && player.isOnFloor)
        {
            player.takeOff = time;
        }
    }
}

glm::mat4 ModelAnimation(Player &player, unsigned short &portionName, bool &isLeft)
{
    glm::mat4 Model = glm::mat4(1.0f);
    float angleLeg;
    float angleArm;
    switch (portionName)
    {
    case 0:
        Model = glm::translate(Model, player.pos);
        Model = glm::rotate(Model, player.angleHead, glm::vec3(0, 1, 0));
        break;

    case 1:
        Model = glm::translate(Model, player.pos);
        Model = glm::rotate(Model, player.angleChest, glm::vec3(0, 1, 0));
        break;

    case 2:
        angleLeg = (isLeft) ? player.angleLeg : -player.angleLeg;
        Model = glm::translate(Model, player.pos + SCALE * glm::vec3(0, 2.5, 0));                                     // Place on shoulders
        Model = glm::rotate(Model, player.angleChest, glm::vec3(0, 1, 0));                                            // Place on the body
        Model = glm::rotate(Model, angleLeg, SCALE * glm::vec3(1, 0, 0));                                             // Rotate
        Model = glm::translate(Model, SCALE * glm::vec3(0, -1.5 + sin(angleLeg - 1.5708f), cos(angleLeg - 1.5708f))); // Pivot
        break;

    case 3:
        angleArm = (isLeft) ? player.angleArm : -player.angleArm;
        Model = glm::translate(Model, player.pos + SCALE * glm::vec3(0, 2.7, 0));                                                         // Place on shoulders
        Model = glm::rotate(Model, player.angleChest, glm::vec3(0, 1, 0));                                                        // Place on the body
        Model = glm::rotate(Model, angleArm, glm::vec3(1, 0, 0));                                                                 // Rotate
        Model = glm::translate(Model, SCALE * glm::vec3(0, -1.5 + 1.2 * sin(angleArm - 1.5708f), 1.2 * cos(angleArm - 1.5708f))); // Pivot
        break;
    }
    return Model;
}

void collisionGeneral(Player &player, std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> chunkHeightMap, float &deltaTime, glm::vec3 posBeforeCollision, std::array<int, chunkLoadSize> &listIndex){

        // * Actualize the chunk of the player
        int beforeCollisionChunk = player.actualChunk;
        player.actualize();

        // * See if the player can be on this new pos (and maybe new chunk)
        // NegMod is to decrement, negative modulo doesn't work like I would
        int beforeNegModX = (posBeforeCollision.x < 0) ? -posBeforeCollision.x / chunkWidth + 1 : 0;
        int beforeNegModZ = (posBeforeCollision.z < 0) ? -posBeforeCollision.z / chunkWidth + 1 : 0;
        int negModX = (player.pos.x < 0) ? -player.pos.x / chunkWidth + 1 : 0;
        int negModZ = (player.pos.z < 0) ? -player.pos.z / chunkWidth + 1 : 0;

        int indexX = int(player.pos.x + negModX * chunkWidth) % chunkWidth;
        int indexZ = int(player.pos.z + negModZ * chunkWidth) % chunkWidth;
        int beforeIndexX = int(posBeforeCollision.x + beforeNegModX * chunkWidth) % chunkWidth;
        int beforeIndexZ = int(posBeforeCollision.z + beforeNegModZ * chunkWidth) % chunkWidth;

        // -1 to transform chunk to index in chunkHeightMap. Then, I remove the negative part of the modulo

        int valChunkHeightMap = chunkHeightMap[getIndexChunk(player.actualChunk, player.actualChunk, listIndex)][indexZ * chunkWidth + indexX].height;
        int valBeforeChunkHeightMap = chunkHeightMap[getIndexChunk(beforeCollisionChunk, player.actualChunk, listIndex)][beforeIndexZ * chunkWidth + beforeIndexX].height;

    if(!CREATE_MOD){
        player.pos.y -= 15 * deltaTime; // * Gravity
        bool isCollision = false;
        // ? Si la nouvelle pos est au dessus de l'ancienne et que le joueur est en dessous alors il y a collision
        if (valChunkHeightMap > valBeforeChunkHeightMap && player.pos.y < valChunkHeightMap)
        {
            player.isOnFloor = true;

            player.pos = glm::vec3(posBeforeCollision.x, player.pos.y, posBeforeCollision.z);
            // player.pos.y -= 0.1; // problème à une coord
            isCollision = true;
            player.isJumpCollision = true;
        }
        else{
            player.isJumpCollision = false;
        }

        // ! 13/05
        if (!player.isJumpCollision)
        {
            // ? Si le joueur est en dessous de sa nouvelle pos et qu'il y a collision / est au sol alors on met à la bonne hauteur
            if (player.pos.y <= valChunkHeightMap && (!isCollision || player.isOnFloor))
            { // ! SI NO GRAVITY (mode créatif) il peut y avoir des sauts lors de passages de chunk
                player.pos.y = valChunkHeightMap;
                player.isOnFloor = true;
            }
        }
        else
        {
            if (player.pos.y <= valBeforeChunkHeightMap && (!isCollision || player.isOnFloor))
            {
                player.pos.y = valBeforeChunkHeightMap;
                player.isOnFloor = true;
            }
        }
    }
}