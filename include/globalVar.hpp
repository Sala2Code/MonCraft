#ifndef GLOBAL_VAR_HPP
#define GLOBAL_VAR_HPP

#include "../common/TextureArray.hpp"


extern bool isWater;
extern bool isSky;


extern GLuint shaderBasic;
extern GLuint shaderBasicInstanced;
extern GLuint shaderWaterLow;
extern GLuint shaderWaterHigh;
extern GLuint shaderSkyLow;
extern GLuint shaderSkyHigh;
extern GLuint shaderLens;

extern bool isF3;
extern float flowTime;
extern float seed;

#endif
