#include "common/include.hpp"

#include "include/calcul.hpp"

float modFloat(float a, float b){
    float mod;
    mod = (a>0) ? a : -a;
    b = (b>0) ? b : -b;
 
    while (mod >= b)
        mod = mod - b;
 
    return (a > 0 || mod==0) ? mod : -mod+b ; // mod==0 : if a is negative and a multiple of b
}

float max3(glm::vec3 v){
  return std::max(std::max(v.x, v.y), v.z);
}