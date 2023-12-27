#ifndef GENERATION_HPP
#define GENERATION_HPP

glm::vec2 random2(glm::vec2 st);
float perlin2D(glm::vec2 st);
int perlin2DheightMap(float x, float y, float height); // [0;height] becuase only for the world map
float v_gradient2(int &gi, float valX, float valY);
unsigned int cellularBiome(float x, float y);
float getBiome(glm::vec2 vec);


#endif