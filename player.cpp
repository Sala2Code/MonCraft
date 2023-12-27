#include "common/include.hpp"
#include "include/constants.hpp"
#include "include/player.hpp"
#include "include/world.hpp"

void Player::actualize(){
    this->actualChunk = getNumChunk( 
        (this->pos.x>=0) ? this->pos.x/SCALE : this->pos.x/SCALE - chunkWidth, // Problem with modulo when it's negative
        (this->pos.z>=0) ? this->pos.z/SCALE : this->pos.z/SCALE - chunkWidth, // Problem with modulo when it's negative
        chunkWidth);

}