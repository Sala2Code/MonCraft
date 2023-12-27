#include <array>

#include "./common/include.hpp"
#include "./include/compObjects.hpp"

// 2d background
GLfloat vertices2D[] = {
    -1.5f, -1.0f, 0.0f,
    1.5f , -1.0f, 0.0f,
    1.5f , 1.0f, 0.0f,
    -1.5f, 1.0f, 0.0f
};
    
GLuint indices2D[] = {
    0, 1, 2,
    0, 2, 3
};

float quadVertices[] = { 
    -1.0f,  1.0f, // Vertex 1 (haut gauche)
    -1.0f,  0.5f, // Vertex 2
    -0.5f,  0.5f, // Vertex 3

    -1.0f,  1.0f, // Vertex 1 (haut gauche)
    -0.5f,  0.5f, // Vertex 3
    -0.5f,  1.0f, // Vertex 4
};

float quadVertices2[] = { 
    -1.0f+ (1  / 2.0),  1.0f, // Vertex 1 (haut gauche)
    -1.0f + (1 / 2.0),  0.5f, // Vertex 2
    -0.5f + (1 / 2.0),  0.5f, // Vertex 3

    -1.0f + (1 / 2.),  1.0f, // Vertex 1 (haut gauche)
    -0.5f + (1 / 2.),  0.5f, // Vertex 3
    -0.5f + (1 / 2.),  1.0f, // Vertex 4
};


float quadUVs[] = { 
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
};



std::array<std::pair<unsigned short, bool>, 6> portionName{{
    {0, false},
    {1, false},
    {2, true},
    {2, false},
    {3, true},
    {3, false}
}};
std::array<std::array<int, 2>, 6> const portionPlayer = {{
    {36,0}, // Head 36-0
    {36,72}, // Chest 36-72
    {60,144}, // LegL 60-144
    {60,264}, // LegR 60-264
    {84,384}, // ArmL 84-384
    {84,552} // ArmR 84-552
}};
