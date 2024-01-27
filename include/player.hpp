
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "struct.hpp"
#include "constants.hpp"
#include "../common/loadBMP.hpp"

struct Player
{
    glm::vec3 pos;
    float angleHead = 0;
    float angleChest = 0; // Whole body
    float angleArm = 0;
    float angleLeg = 0;
    float angleView = 0;
    float angleVertical = 0;

    bool thirdView = false;

    int actualChunk = 1;

    std::vector<BlockInfo> worldMap = {}; 
    std::vector<BlockInfo> worldEnv = {}; 
    std::vector<BlockInfo> worldFol = {};

    GLuint texture;


    Player(glm::vec3 pos_player){
        this->pos = pos_player;
        this->texture = loadBMP_custom("img/Entity/steve.bmp");
    }

    void actualize();


};

#endif

