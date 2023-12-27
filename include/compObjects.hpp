#ifndef COMP_OBJECTS_HPP
#define COMP_OBJECTS_HPP

// 2d background
extern GLfloat vertices2D[12];
extern GLuint indices2D[6];

extern float quadVertices[12];
extern float quadVertices2[12];

extern float quadUVs[12];



extern std::array<std::pair<unsigned short, bool>, 6> portionName;
extern std::array<std::array<int, 2>, 6> const portionPlayer;

#endif