#include "common/include.hpp"

#include <cstdlib>
#include <ctime>

#include "include/constants.hpp"

const std::string name = "MonCraft_Sala2Code";
const std::string pathJson = "others/texture.json";
const int width = 1000;
const int height = 750;

const float SCALE = .1; // Decrease if chunkView > 5

// Player
const float speed = 18.0f * SCALE;
const float speedAngle = 50.0f;
const float mouseSpeed= 0.1f;
const float FoV = 75.0f;
const glm::mat4 Projection = glm::perspective(glm::radians(FoV), (float) width / (float)height, 0.1f, 256.f*SCALE); // chunkView*chunkWidth*SCALE
const bool CREATE_MOD = true;

// Noise
const int res = 80;
const int resBiome = 20; // biome width

// Chunk
const int heightScale = 80;
const int tolView = 30 * SCALE;
const int sizeSmooth = 3; // mean, smooth transition betweeen biome (around (sizeSmooth*2+1)^2 blocks )

const int WATER_HEIGHT = 10;

const std::unordered_map<std::string, int> textureMap = {
    {"dirt", 0},
    {"grass", 1},
    {"grass_taiga", 2},
    {"grass_swamp", 3},
    {"sand", 4},
    {"snow", 5},
    {"stone", 6}
};

const int coverage = 10*width*width/height; // occlusion queries (lens flare) // formula create like this (make sure query/coverage <=1)