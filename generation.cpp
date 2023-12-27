#include <random>

#include "./common/include.hpp"

#include "./include/constants.hpp"
#include "./include/generation.hpp"

glm::vec2 random2(glm::vec2 st){
    st = glm::vec2(glm::dot(st, glm::vec2(127.1, 311.7)), glm::dot(st, glm::vec2(269.5, 183.3)));
    return glm::vec2(-1.0f + 2.0f * glm::fract(glm::sin(st) * 43758.545f));
}
float perlin2D(glm::vec2 st){
    st /= float(res);
    glm::vec2 i = glm::floor(st);
    glm::vec2 f = glm::fract(st);
    glm::vec2 u = f * f * (glm::vec2(3.0) - glm::vec2(2.0) * f);

    return 
        glm::mix(
            glm::mix(
                    glm::dot(random2(i + glm::vec2(0.0, 0.0)), f - glm::vec2(0.0, 0.0)),
                    glm::dot(random2(i + glm::vec2(1.0, 0.0)), f - glm::vec2(1.0, 0.0)),
                    u.x),
            glm::mix(
                glm::dot(random2(i + glm::vec2(0.0, 1.0)), f - glm::vec2(0.0, 1.0)),
                glm::dot(random2(i + glm::vec2(1.0, 1.0)), f - glm::vec2(1.0, 1.0)),
                u.x), 
        u.y); 
}
// perlin noise : world map
int perlin2DheightMap(float x, float y, float height){
   float n=0;
   for(int i=1; i<8;i++){
        n += (1./i) * perlin2D(float(i)*glm::vec2(x, y));
    }
    n = (.5*(1.-n));
    n = pow(n,0.8);
    
    n = n*6. - 3.;
    n = 1./(pow(2.781,-n)+1.);
    // n = round(n*64)/64.;
   
   return  n*height+1;

}

// (mix() + .5)*0.5*height+1

unsigned int cellularBiome(float x, float y){
    glm::vec2 st = glm::vec2(x, y)/float(resBiome);
    glm::vec2 i_st = glm::floor(st);
    glm::vec2 f_st = glm::fract(st);

    float m_dist = 1000.;  // minimum distance
    glm::vec2 biomePos; // cell position
    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            glm::vec2 neighbor = glm::vec2(float(x),float(y));
            glm::vec2 point = random2(i_st + neighbor);

            float dist = length(neighbor + point - f_st);
            if(dist<m_dist){
                m_dist = dist;
                biomePos = point;
            }
        }
    }
    return getBiome(biomePos);
}

float getBiome(glm::vec2 vec) {
    std::mt19937 gen(std::hash<float>{}(vec.x) ^ (std::hash<float>{}(vec.y) << 1));
    std::uniform_int_distribution<int> dist(0, nbBiomes-1); // [[0, nbBiomes-1]]
    return dist(gen);
}
