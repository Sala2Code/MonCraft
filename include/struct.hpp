#ifndef STRUCT_HPP
#define STRUCT_HPP

// * Buffer
struct Buffer{
    GLuint vertex;
    GLuint uv;
    GLuint normal;
};


// * Block
struct BlockMap{
    int height;
    int biome;
    int height_own; // height of the block in the chunk withoutout influence of neighbors (see generation.cpp)
};

struct BlockInfo{
	glm::vec3 pos;
	int biome;
	int texture;
};


// * View
struct ViewComp{
    glm::vec3 eye;
    glm::vec3 dir;
};
struct Frustum{
    glm::vec3 topFace;
    glm::vec3 bottomFace;
    glm::vec3 rightFace;
    glm::vec3 leftFace;
    glm::vec3 farFace;
    glm::vec3 nearFace;
};


// * Others
struct WaterComp{
    GLuint reflection;
    GLuint refraction;
    GLuint dudv;
    GLuint normal;
};


struct Tree {
    glm::vec3 pos;
    int chunk;
    int lengthTrunk;

    std::vector<bool> wood; // 3d, squared base
    std::size_t sizeY_w;
    std::size_t sizeZ_w; // odd if oak, even if jungle

    std::vector<bool> leaves; // 3d, squared base
    std::size_t sizeY_l;
    std::size_t sizeZ_l;


    Tree(std::size_t y_w=1, std::size_t z_w=1, std::size_t y_l=1, std::size_t z_l=1) : 
        sizeY_w(y_w), sizeZ_w(z_w), wood(y_w*z_w*z_w),
        sizeY_l(y_l), sizeZ_l(z_l), leaves(y_l*z_l*z_l)
        {}
};





#endif