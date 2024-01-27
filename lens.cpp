#include "common/include.hpp" 
#include "common/loadPNG.hpp"


#include "include/constants.hpp"
#include "include/globalVar.hpp"
#include "include/struct.hpp"
#include "include/compObj.hpp"
#include "include/player.hpp"
#include "include/draw.hpp"

#include "include/lens.hpp"


std::vector<std::pair<float, GLuint>> flareTextures;
std::array<int, 100> queryResults = {}; // to avoid occlusion queries flickering

void initLensFlare(GLuint &queryID, GLuint &VAO_Lens, GLuint &buffer){
    GLuint Lens_sun = loadPNG_LodePNG("img/Lens/sun.png");
	GLuint Lens_tex1 = loadPNG_LodePNG("img/Lens/tex1.png");
	GLuint Lens_tex2 = loadPNG_LodePNG("img/Lens/tex2.png");
	GLuint Lens_tex3 = loadPNG_LodePNG("img/Lens/tex3.png");
	GLuint Lens_tex4 = loadPNG_LodePNG("img/Lens/tex4.png");
	GLuint Lens_tex5 = loadPNG_LodePNG("img/Lens/tex5.png");
	GLuint Lens_tex7 = loadPNG_LodePNG("img/Lens/tex7.png");
	GLuint Lens_tex8 = loadPNG_LodePNG("img/Lens/tex8.png");
	GLuint Lens_tex9 = loadPNG_LodePNG("img/Lens/tex9.png");

	flareTextures = {
		{0.2, Lens_sun},
		{0.23, Lens_tex4},
		{0.1, Lens_tex2},
		{0.05, Lens_tex7},
		{0.02, Lens_tex1},
		{0.06, Lens_tex3},
		{0.12, Lens_tex9},
		{0.07, Lens_tex5},
		{0.012, Lens_tex1},
		{0.08, Lens_tex7},
		{0.1, Lens_tex9},
		{0.07, Lens_tex3},
		{0.3, Lens_tex5},
		{0.4, Lens_tex4},
		{0.6, Lens_tex8}
	};

    glGenQueries(1, &queryID);

	glGenVertexArrays(1, &VAO_Lens);
	glBindVertexArray(VAO_Lens);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

float Mean_Update(int newValue){
    std::rotate(queryResults.begin(), queryResults.begin() + 1, queryResults.end());
    queryResults.back() = newValue;

    int sum = std::accumulate(queryResults.begin(), queryResults.end(), 0);
    float average = static_cast<float>(sum) / queryResults.size();
    return average;
}

int occlusionQueries(GLuint &queryID, GLuint &VAO, glm::mat4 projView, float angleSun, glm::vec3 pos){
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE); 
		glBeginQuery(GL_SAMPLES_PASSED, queryID);
		glm::vec3 sunPosBlock = pos + glm::vec3(1.6/SCALE)*glm::vec3( glm::vec3( -sin( angleSun), cos( angleSun), -0.5*SCALE) ); // 1.6/SCALE, almost 10 chunks with 16 blocks ; it's enough to set the sun behind the map

		glUseProgram(shaderBasic);

		glBindVertexArray(VAO);
		
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, sunPosBlock);
		glm::mat4 MVP = projView * model;
		glUniformMatrix4fv(glGetUniformLocation(shaderBasic, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		drawBlockFace(0, 6);
		glBindVertexArray(0);

		glEndQuery(GL_SAMPLES_PASSED);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);

		GLuint queryResult;
		glGetQueryObjectuiv(queryID, GL_QUERY_RESULT, &queryResult);
        return queryResult;
}


void LensFlare(GLuint &queryID,GLuint &VAO_Lens, GLuint &VAO_Sky, ViewComp &viewComp, glm::vec3 pos){
    	float angleSun  = (glfwGetTime() * flowTime) * 3.1415926535 * 2.;
		glm::mat4 View = glm::lookAt(viewComp.eye, viewComp.dir + viewComp.eye, glm::vec3(0,1,0));
        glm::mat4 projView = Projection * View;

        int queryRes = occlusionQueries(queryID, VAO_Lens, projView, angleSun, pos);

		glm::vec3 sunPos = pos + glm::vec3( glm::vec3( -sin( angleSun), cos( angleSun) + .2, 0.) );
		glm::vec4 coords = glm::vec4(sunPos, 1.0f);
		coords = projView  * coords;
		glm::vec2 sunCoords;
		if (coords.w <= 0) {
			sunCoords = glm::vec2(-1, -1); // out of screen
		}else{
			float x = (coords.x / coords.w + 1) / 2.0f;
			float y = 1 - ((coords.y / coords.w + 1) / 2.0f);
			sunCoords = glm::vec2(x, y);
		}

		float spacing = 0.15;
		glm::vec2 sunToCenter = glm::vec2(0.5) - sunCoords;
		float brightness = 1 - (glm::length(sunToCenter) / 0.5f);
		if(brightness > 0){
			glUseProgram(shaderLens);
			glBindVertexArray(VAO_Sky);
			
			
			for (size_t i = 0; i <flareTextures.size(); i++) {
				glm::vec2 direction = sunToCenter * static_cast<float>(i) * spacing;
				glm::vec2 flarePos = sunCoords + direction;
				float xScale = flareTextures[i].first;
				float yScale = xScale * width / height;

				glUniform2f(glGetUniformLocation(shaderLens, "flarePos"), flarePos.x, flarePos.y);
				glUniform1f(glGetUniformLocation(shaderLens, "brightness"), brightness * Mean_Update(queryRes)/coverage);
				glUniform1f(glGetUniformLocation(shaderLens, "zLens"), i+1);
				glUniform2f(glGetUniformLocation(shaderLens, "scale"), xScale, yScale);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, flareTextures[i].second);
				glUniform1i( glGetUniformLocation(shaderLens, "texFlare"), 0);

				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces);
			}	
			glBindVertexArray(0);
		}
}

