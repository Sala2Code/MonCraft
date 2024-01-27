#ifndef LENS_HPP
#define LENS_HPP

void initLensFlare(GLuint &queryID, GLuint &VAO_Lens, GLuint &buffer);
int occlusionQueries(GLuint &VAO, glm::mat4 projView, float angleSun, glm::vec3 pos);
float Mean_Update(int newValue);
void LensFlare(GLuint &queryID, GLuint &VAO_Lens, GLuint &VAO_Sky, ViewComp &viewComp, glm::vec3 pos);

#endif