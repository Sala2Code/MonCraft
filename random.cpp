#include "common/include.hpp"

#include "include/globalVar.hpp"

#include "include/random.hpp"



//R^2 -> [-1,1]^2
glm::vec2 random2(glm::vec2 st){
    static const glm::vec2 coeff1(100.751*seed*sin(seed), 311.713*4.0134*seed);
    static const glm::vec2 coeff2(269.541+seed, 183.398-seed);

    st = glm::vec2(glm::dot(st, coeff1), glm::dot(st, coeff2));
    return glm::vec2(-1.0f + 2.0f * glm::fract(glm::sin(st) * seed));
}

//R^2 -> [a,b]
int random2_1(const glm::vec2 &vec2, int a, int b){
    constexpr glm::vec2 constants1(13.748, 91.743);
    constexpr glm::vec2 constants2(12.714, 78.273);

    float hash1 = glm::fract(sin(glm::dot(constants1 * vec2, constants2)) * 438.385);
    float hash2 = glm::fract(sin(glm::dot(constants2 * vec2, constants1)) * 587.189);

    float finalHash = glm::fract((hash1 + hash2) * 0.5);

    return a + (b - a) * finalHash;
}

//R^3 -> {0,1}
bool randomBinary(glm::vec3 position, float p){
    float value = glm::fract(sin(glm::dot(position, glm::vec3(12.8, 78.2, 45.5))) * 43758.54);
    return value < p;
}
// R^2 -> {0,1}
bool randomBinary2(glm::vec2 position, float p) {
    float value = glm::fract(sin(glm::dot(position, glm::vec2(17.8, 71.3))) * 9748.54);
    return value < p;
}