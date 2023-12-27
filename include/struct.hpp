#ifndef STRUCT_HPP
#define STRUCT_HPP


struct BlockMap{
    int height;
    int biome;
};

struct BlockShader{
    GLuint shaderBasicInstanced;
    GLuint shaderWater;
    GLuint Texture;
    glm::mat4 ProjView;
    GLuint shaderReflection;
    GLuint shaderRefraction;
    GLuint dudv;
    
};

struct BlockInfo{
	glm::vec3 pos;
	int biome;
	GLuint shader;
	GLuint texture;
};

struct ViewComp{
    glm::vec3 eye;
    glm::vec3 dir;
};

struct bufferComp{
    GLuint EBO;
    GLuint vertex;
    GLuint uv;
    GLuint normal;
};

struct mvpComp{
    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 Model;
}; // ! PEUT ETRE MVP en +


#endif