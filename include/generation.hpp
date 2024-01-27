#ifndef GENERATION_HPP
#define GENERATION_HPP

#include "constants.hpp"
#include "struct.hpp"

float perlin2D(glm::vec2 st);
glm::vec2 turbulence2D(glm::vec2 st);
int voronoiBiome(glm::vec2 st);
float polynome_biome(float a, float b, float c, float d, float x);
float filter_map(float h_map, float smooth_h_map, float a, float b, float c, float d, float coeff);
void setTempHeightMapHeight(std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap, int chunkCoX, int chunkCoZ, std::array<int, chunkLoadSize> &chunkLoad, int i);
int getHeightMap(float x, float y, float heightScale);
int getBiome(glm::vec2 st);

#endif