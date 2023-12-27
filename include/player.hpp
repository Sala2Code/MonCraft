
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../common/loadOBJ.hpp" 
#include "struct.hpp"
#include "constants.hpp"

struct Player
{
    // void setPosition();

    glm::vec3 pos;
    float angleHead = 0;
    float angleChest = 0; // Whole body
    float angleArm = 0;
    float angleLeg = 0;
    float angleView = 0;
    float angleVertical = 0;

    bool thirdView = false;

    int actualChunk = 1;

    // Jump
    bool isOnFloor = true;
    double takeOff = glfwGetTime(); // Time jump starting
	bool isJumpCollision = false;


    // Hit box
    bool showHitBox = false;
    std::vector<glm::vec3> verticesHitBox;
	std::vector<glm::vec2> uvsHitBox;
	std::vector<glm::vec3> normalsHitBox;

    bufferComp bufferHitBox;

	std::vector<unsigned short> iHitBox;
	std::vector<glm::vec3> i_verticesHitBox;
	std::vector<glm::vec2> i_uvsHitBox;
	std::vector<glm::vec3> i_normalsHitBox;

    Player(glm::vec3 pos_player, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals){
        loadOBJ("obj/steve.obj", vertices, uvs, normals);
        loadOBJ("obj/HitBox.obj", verticesHitBox, uvsHitBox, normalsHitBox);

        this->pos = pos_player;
    }

    void actualize();


};

#endif

