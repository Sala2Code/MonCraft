#include "common/include.hpp"

#include <algorithm>

#include "include/struct.hpp"
#include "include/constants.hpp"
#include "include/globalVar.hpp"
#include "include/random.hpp"
#include "include/player.hpp"
#include "include/chunk.hpp"


#include "include/generation.hpp"

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


const int numOctaves_biome = 4;
float amplitude_biome = 1.;
float frequency_biome = 1.5;
float persistence_biome = 1.;
glm::vec2 turbulence2D(glm::vec2 st){
    glm::vec2 noi = glm::vec2(0,0);
    for(int i = 0; i < numOctaves_biome; i++) {
        float ampPer = amplitude_biome * perlin2D(st);
        noi.x += ampPer;
        noi.y += ampPer;
        st *= frequency_biome;
        amplitude_biome *= persistence_biome;
    }
    return noi;
}

int voronoiBiome(glm::vec2 st){
    st += turbulence2D(st);
    st /= glm::vec2(res);
    glm::vec2 i_st = glm::floor(st);
    glm::vec2 f_st = glm::fract(st); 

    float m_dist = 1.;  // minimum distance
    glm::vec2 m_point = glm::vec2(0,0); // minimum point
    int m_biome = 0; // biome to minimum point
    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y));
            glm::vec2 point = random2(i_st + neighbor);
            point = glm::vec2(0.5)*glm::sin(glm::vec2(6.283184)*point);
    
            float dist = glm::length(neighbor + point - f_st);
            if( dist < m_dist ) {
                m_dist = dist;
                m_point = point;
            }
        }
    }
    return getBiome(random2(m_point));
}

float polynome_biome(float a, float b, float c, float d, float x) {
    return pow(x,4.)*a + pow(x,3.)*b + pow(x,2.)*c + x*d;
}
float filter_map(float h_map, float smooth_h_map, float a, float b, float c, float d, float coeff){
    float map = coeff * h_map + (1.0 - coeff) * smooth_h_map;
    return polynome_biome(a, b, c, d, map);
}

// chunkLoad is listIndex
void setTempHeightMapHeight(
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
        int chunkCoX,
        int chunkCoZ,
        std::array<int, chunkLoadSize> &chunkLoad,
        int i
    ){
    int getNum = getNumChunk(chunkCoX, chunkCoZ, 1);
    int getNum_X_1 = getNumChunk(chunkCoX-1, chunkCoZ, 1);
    int getNum_X1 = getNumChunk(chunkCoX+1, chunkCoZ, 1);
    int getNum_Z_1 = getNumChunk(chunkCoX, chunkCoZ-1, 1);
    int getNum_Z1 = getNumChunk(chunkCoX, chunkCoZ+1, 1);

    bool find = std::find(chunkLoad.begin(), chunkLoad.end(), getNum) != chunkLoad.end();
    bool find_X_1 = std::find(chunkLoad.begin(), chunkLoad.end(), getNum_X_1) != chunkLoad.end();
    bool find_X1 = std::find(chunkLoad.begin(), chunkLoad.end(), getNum_X1) != chunkLoad.end();
    bool find_Z_1 = std::find(chunkLoad.begin(), chunkLoad.end(), getNum_Z_1) != chunkLoad.end();
    bool find_Z1 = std::find(chunkLoad.begin(), chunkLoad.end(), getNum_Z1) != chunkLoad.end();

    int getNum_X_1_Z_1 = getNumChunk(chunkCoX-1, chunkCoZ-1, 1);
    int getNum_X1_Z_1 = getNumChunk(chunkCoX+1, chunkCoZ-1, 1);
    int getNum_X_1_Z1 = getNumChunk(chunkCoX-1, chunkCoZ+1, 1);
    int getNum_X1_Z1 = getNumChunk(chunkCoX+1, chunkCoZ+1, 1);

    for(int z=0; z<chunkWidth;z++){
        for(int x=0; x<chunkWidth;x++){
            int sum = 0; // for calculate mean
            if( (x==0 || true || (z<3 || z>12))){

                for(int xi =-sizeSmooth;xi<=sizeSmooth;xi++){
                    for(int zi =-sizeSmooth; zi<=sizeSmooth; zi++){
                        // in the chunk 
                        if((x+xi>=0 && x+xi<chunkWidth) && (z+zi>=0 && z+zi<chunkWidth) ){
                            sum+=chunkHeightMap[i][(z+zi)*chunkWidth+(x+xi)]->height_own;
                        }
                        // ! chunkCoX / chunkCoZ and xi / zi are inversed
                        else if(x+xi<0 && z+zi<0 ){ 
                            if(find_X_1 && find_Z_1){
                                sum+=chunkHeightMap[getIndexChunk(getNum_X_1_Z_1, 1, chunkLoad)][((z+zi)+chunkWidth)*chunkWidth+(x+xi)+chunkWidth]->height_own;
                            }else{
                                sum+=getHeightMap((float)chunkCoX*chunkWidth + (x+xi),  (float)chunkCoZ*chunkWidth + (z+zi), heightScale);
                            }
                        }else if(x+xi>=chunkWidth && z+zi<0){
                            if(find_X1 && find_Z_1 ){
                                sum+=chunkHeightMap[getIndexChunk(getNum_X1_Z_1, 1, chunkLoad)][((z+zi)+chunkWidth)*chunkWidth+(x+xi)-chunkWidth]->height_own;
                            }else{
                                sum+=getHeightMap((float)chunkCoX*chunkWidth + (x+xi),  (float)chunkCoZ*chunkWidth + (z+zi), heightScale);
                            }
                        }else if(x+xi<0 && z+zi>=chunkWidth ){
                            if(find_X_1 && find_Z1){
                                sum+=chunkHeightMap[getIndexChunk(getNum_X_1_Z1, 1, chunkLoad)][((z+zi)-chunkWidth)*chunkWidth+(x+xi)+chunkWidth]->height_own;
                            }else{
                                sum+=getHeightMap((float)chunkCoX*chunkWidth + (x+xi),  (float)chunkCoZ*chunkWidth + (z+zi), heightScale);
                            }
                        }else if(x+xi>=chunkWidth && z+zi>=chunkWidth){
                            if(find_X1 && find_Z1){
                                sum+=chunkHeightMap[getIndexChunk(getNum_X1_Z1, 1, chunkLoad)][((z+zi)-chunkWidth)*chunkWidth+(x+xi)-chunkWidth]->height_own;
                            }else{
                                sum+=getHeightMap((float)chunkCoX*chunkWidth + (x+xi),  (float)chunkCoZ*chunkWidth + (z+zi), heightScale);
                            }
                        }
                        // right / left / top / bottom
                        // 18 valeurs parce que 3 (zi=2 zi=2, xi = 0, 1, 2 (sinon corner)) * 2 (sizeSmooth) * 3 (chunks)
                        else if(x+xi<0 && find_X_1 ){ 
                            sum+=chunkHeightMap[getIndexChunk(getNum_X_1, 1, chunkLoad)][(z+zi)*chunkWidth+(x+xi)+chunkWidth]->height_own;
                        }else if(x+xi>=chunkWidth && find_X1 ){
                            sum+=chunkHeightMap[getIndexChunk(getNum_X1, 1, chunkLoad)][(z+zi)*chunkWidth+(x+xi)-chunkWidth]->height_own;
                        }else if(z+zi<0 && find_Z_1 ){
                            sum+=chunkHeightMap[getIndexChunk(getNum_Z_1,  1, chunkLoad)][((z+zi)+chunkWidth)*chunkWidth+(x+xi)]->height_own;
                        }else if(z+zi>=chunkWidth && find_Z1){
                            sum+=chunkHeightMap[getIndexChunk(getNum_Z1, 1, chunkLoad)][((z+zi)-chunkWidth)*chunkWidth+(x+xi)]->height_own;
                        }
                        // out of chunks loaded
                        else{
                            sum+=getHeightMap((float)chunkCoX*chunkWidth + (x+xi),  (float)chunkCoZ*chunkWidth + (z+zi), heightScale);
                        }
                    }
                }
            }
            chunkHeightMap[i][z*chunkWidth+x]->height = sum/pow(2*sizeSmooth+1,2) +1;
        }
    }
}

int getHeightMap(float x, float y, float heightScale){
    float zoomBiome = 1.;
    int biome = voronoiBiome(zoomBiome*glm::vec2(x,y));

    float zoom = 0.6; 
    const int numOctaves = 4; // expensive to compute
    float amplitude = 1.0;
    float frequency = 1.; 
    float persistence = 0.5; 
    glm::vec2 st = glm::vec2(x,y)*zoom;

    float smooth_heightMap = perlin2D(st);
    float heightMap = 0;
    for (int i = 0; i < numOctaves; i++) {
        heightMap += perlin2D(frequency * st) * amplitude;
        st *= 2.0;  
        amplitude *= persistence; 
    }
    heightMap+= .5 + perlin2D(st)/50.;

    // ? river
    float r = 0.0;
    st*=0.1; // 0.017 : fleuve
    for (int i = 0; i < numOctaves; i++) {
        r += perlin2D(frequency * st) * amplitude;
        st *= 2.0;  
        amplitude *= persistence; 
    }
    r = 0.5 - abs(r)/0.05 ; // height and breadth
    r = glm::clamp(r, 0.f, 1.f);
    heightMap -= r;


    heightMap = glm::clamp(heightMap, 0.f, 1.f);
    smooth_heightMap = glm::clamp(smooth_heightMap, 0.f, 1.f);

    float mask = (glm::clamp(heightMap, 0.5f, 1.f) - .5)*1000.; // sea haven't biome
    mask = glm::clamp(mask, 0.f, 1.f);
    if(mask!=0){
        switch (biome){
            case 0:
                heightMap = filter_map(heightMap, smooth_heightMap,-.5, 0.3, .1, .15, .1);
                break;
            case 1:
                heightMap = filter_map(heightMap, smooth_heightMap,1.55, -0.75, -3.4, 3.6, 0.75);
                break;
            case 2: // 1.5, -3., .7, 1.2, .5
                heightMap = filter_map(heightMap, smooth_heightMap,-0.15, 0.75, -1.5, 1.4, .5); // same than 5
                break;
            case 3:
                heightMap = filter_map(heightMap, smooth_heightMap,1.5, -3., .7, 1.2, .2);
                break;
            case 4:
                heightMap = filter_map(heightMap, smooth_heightMap,.2, -0.2, -0.6, 1., .75);
                break;
            case 5:
                heightMap = filter_map(heightMap, smooth_heightMap,-0.15, 0.75, -1.5, 1.4, .5); // same than 2
                break;
            case 6:
                heightMap = filter_map(heightMap, smooth_heightMap,.25, -1.0, 1., 0., .5);
                break;
            case 7:
                heightMap = filter_map(heightMap, smooth_heightMap,-1.1, 0., 1.5, 0.3, .5);
                break;
            default:
                break;
            }
    }
    return heightMap*heightScale + 1;
}


int getBiome(glm::vec2 st){
    float tem = (perlin2D(glm::vec2(2000)*st)+.5)*100.;
    float rain = (perlin2D(glm::vec2(2000)*st + glm::vec2(1000,-1000))+.5)*100.;

    if(tem<33.){
        if(rain<55.){
            return 0; // tundra
        }else{
            return 1; // taiga
        }
    }else if(tem<66.){
        if(rain<33.){
            return 2; // plains
        }else if(rain<66.){
            return 3; // forest
        }else{
            return 4; // jungle
        }
    }else{
        if(rain<40.){
            return 5; // desert
        }else if(rain<66.){
            return 6; // plains
        }else{
            return 7; // swamp
        }
    }
}
