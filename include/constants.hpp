#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <array> 

extern const int width;
extern const int height;

extern const float SCALE;

extern const int res;
extern const int resBiome;


extern const float speed;
extern const float speedAngle;
extern const float mouseSpeed;
extern const float FoV;
extern const bool CREATE_MOD;

// * world
extern const int heightMap;
extern const int tolView;

constexpr int chunkView = 3;
constexpr int chunkLoadSize  = 4*chunkView*(chunkView+1)+1;
constexpr int chunkWidth = 16;
constexpr int chunkArea = chunkWidth * chunkWidth;

extern const int nbBiomes;
extern const int WATER_HEIGHT;


#endif