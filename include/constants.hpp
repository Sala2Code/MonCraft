#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <array> 


extern const std::string name;
extern const std::string pathJson;
extern const int width;
extern const int height;


extern const float SCALE;

extern const int res;
extern const int resBiome;


extern const float speed;
extern const float speedAngle;
extern const float mouseSpeed;
extern const float FoV;
extern const glm::mat4 Projection;
extern const bool CREATE_MOD;

// * world
extern const int heightScale;
extern const int tolView;
extern const int sizeSmooth;

constexpr int chunkView = 2;
constexpr int chunkLoadSize  = 4*chunkView*(chunkView+1)+1; // (2*chunkView+1)^2
constexpr int chunkWidth = 16; // multiple of 4
constexpr int chunkArea = chunkWidth * chunkWidth;

extern const int WATER_HEIGHT;

extern const std::unordered_map<std::string, int> textureMap;

extern const int coverage;

#endif