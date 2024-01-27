#include "common/include.hpp"
#include "common/TextureArray.hpp"


#include "include/globalVar.hpp"

bool isWater = true;
bool isSky = true;

// * Shaders
GLuint shaderBasic;
GLuint shaderBasicInstanced;
GLuint shaderWaterLow;
GLuint shaderWaterHigh;
GLuint shaderSkyLow;
GLuint shaderSkyHigh;
GLuint shaderLens;

bool isF3 = false;
float flowTime = 0.02; // 0.02

float seed = 0;

