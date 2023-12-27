#include "common/include.hpp"
#include "include/player.hpp"
#include "include/controls.hpp"
#include "include/initObjects.hpp"
#include "include/compObjects.hpp" // portion
#include "include/display.hpp"
#include "include/world.hpp"
#include "include/generation.hpp"
#include "include/struct.hpp"

void showHitBox(Player &player, glm::mat4 Projection, glm::mat4 View, GLuint MatrixID, GLuint ModelMatrixID){
        bindBuffers(player.bufferHitBox);
        glm::mat4 Model = ModelAnimation(player, portionName[1].first, portionName[1].second); // Like the chest
        glm::mat4 mvp =  Projection * View * Model ;
        
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        glDrawElements(GL_TRIANGLES,player.iHitBox.size(), GL_UNSIGNED_SHORT, (void*)0 );
        unbindBuffer(3);
}

void show2D(GLuint EBO){
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2D), vertices2D, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    unbindBuffer(1);
}


void showPlayer(Player player, bufferComp &bufferPlayer, glm::mat4 ProjView, GLuint shader, bool isDisplay ){
    GLuint ModelMatrixID = glGetUniformLocation(shader, "Model");
	GLuint MatrixID = glGetUniformLocation(shader, "MVP");

    bindBuffers(bufferPlayer);

    for(unsigned int i=0; i<6; i++){ // 6 is the number of steve's members
        if(isDisplay){
            glm::mat4 Model = ModelAnimation(player, portionName[i].first, portionName[i].second);
            Model = glm::scale(Model, glm::vec3(SCALE));
            glm::mat4 mvp =  ProjView * Model ;
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

            glDrawElements(GL_TRIANGLES, portionPlayer[i][0], GL_UNSIGNED_SHORT, reinterpret_cast<void*>(portionPlayer[i][1]) );
        }
    }
    unbindBuffer(3);
}

void showMap(bufferComp &buffer, Player player, BlockShader &blockShader, std::vector<unsigned short> &iBlock, float up, std::vector<BlockInfo> &worldMap){
    bindBuffers(buffer);
    
    // all terrain block
    showMapBlock(worldMap, blockShader.shaderBasicInstanced, blockShader, iBlock, player, up);


    unbindBuffer(3);
}


void showMapBlock(std::vector<BlockInfo> &worldMap, GLuint shader, BlockShader blockShader, std::vector<unsigned short> &iBlock, Player &player, float up){
    glUseProgram(shader);
	GLuint TextureBasicID  = glGetUniformLocation(shader, "myTextureSampler");

    std::vector<glm::mat4> instanceMatrices;
    std::vector<float> instanceBiomes;
    std::vector<glm::mat4> instanceModels;
	for(auto& block : worldMap){
		if(block.shader == shader){
			bindTexture(block.texture, TextureBasicID); // ? peut-être le sortir de la boucle pour le faire qu'une fois mais faut trouver le block avec la bonne texture
            glm::mat4 Model = glm::translate(glm::mat4(1.0f), block.pos);
            Model = glm::scale(Model, glm::vec3(SCALE, SCALE, SCALE));
            glm::mat4 mvpMatrix = blockShader.ProjView * Model;
            instanceMatrices.push_back(mvpMatrix);

            instanceBiomes.push_back(block.biome);

            instanceModels.push_back(Model);
		}
	}

    // * pass to vertex buffer
    GLuint biomeBuffer;
    glGenBuffers(1, &biomeBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, biomeBuffer);
    glBufferData(GL_ARRAY_BUFFER, instanceBiomes.size() * sizeof(float), &instanceBiomes[0], GL_STATIC_DRAW);
    instancedRendering(shader, "iBiome_instance", sizeof(float), 1, 3); // location is useless for int/vec


    GLuint instanceBuffer;
    glGenBuffers(1, &instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), &instanceMatrices[0], GL_STATIC_DRAW);
    instancedRendering(shader, "MVP_instance", sizeof(glm::vec4), 4, 4);


    GLuint instanceBufferModels;
    glGenBuffers(1, &instanceBufferModels);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBufferModels);
    glBufferData(GL_ARRAY_BUFFER, instanceModels.size() * sizeof(glm::mat4), &instanceModels[0], GL_STATIC_DRAW);
    instancedRendering(shader, "Models_instance", sizeof(glm::vec4), 4, 8);

    // glUniform 
	GLuint timeID  = glGetUniformLocation(shader, "u_time");
	GLuint upID  = glGetUniformLocation(shader, "up");
    glUniform1f(timeID, glfwGetTime() );
    glUniform1f(upID, up);

    glDrawElementsInstanced(GL_TRIANGLES, iBlock.size(), GL_UNSIGNED_SHORT, (void*)0, instanceMatrices.size());
}



void showWater(bufferComp &buffer, Player player, BlockShader &blockShader, std::vector<unsigned short> &iBlock, float up, std::vector<BlockInfo> &worldMap){
    bindBuffers(buffer);

    GLuint shader = blockShader.shaderWater;
    glUseProgram(shader);

    std::vector<glm::mat4> instanceMatrices;
    std::vector<glm::mat4> instanceModels;
	for(auto& block : worldMap){
		if(block.shader == shader){
            glm::mat4 Model = glm::translate(glm::mat4(1.0f), block.pos);
            Model = glm::scale(Model, glm::vec3(SCALE, SCALE, SCALE));
            glm::mat4 mvpMatrix = blockShader.ProjView * Model;
            instanceMatrices.push_back(mvpMatrix);

            instanceModels.push_back(Model);
		}
	}
    GLuint instanceBuffer;
    glGenBuffers(1, &instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), &instanceMatrices[0], GL_STATIC_DRAW);
    instancedRendering(shader, "MVP_instance", sizeof(glm::vec4), 4, 3);

    GLuint instanceBufferModels;
    glGenBuffers(1, &instanceBufferModels);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBufferModels);
    glBufferData(GL_ARRAY_BUFFER, instanceModels.size() * sizeof(glm::mat4), &instanceModels[0], GL_STATIC_DRAW);
    instancedRendering(shader, "Models_instance", sizeof(glm::vec4), 4, 7);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blockShader.shaderReflection);
    glUniform1i( glGetUniformLocation(shader, "reflectionShader"),0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, blockShader.shaderRefraction);
    glUniform1i( glGetUniformLocation(shader, "refractionShader"),1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, blockShader.dudv);
    glUniform1i( glGetUniformLocation(shader, "dudvMap"),2);


	GLuint timeID  = glGetUniformLocation(shader, "u_time");
    GLuint posID  = glGetUniformLocation(shader, "posCam");

    glUniform1f(timeID, glfwGetTime() );
    glUniform3f(posID, player.pos.x, player.pos.y, player.pos.z);


    glDrawElementsInstanced(GL_TRIANGLES, iBlock.size(), GL_UNSIGNED_SHORT, (void*)0, instanceMatrices.size()); // ? Essayer de faire juste un plan
    unbindBuffer(3);

}

void instancedRendering(GLuint shader, std::string attribute, float sizeType, int n, int location){
    GLuint InstanceID = glGetAttribLocation(shader, attribute.c_str());
    glEnableVertexAttribArray(InstanceID);
    for (unsigned int i = 0; i<n; ++i) {
        glEnableVertexAttribArray(location + i);
        glVertexAttribPointer(InstanceID + i, n, GL_FLOAT, GL_FALSE,n*sizeType, reinterpret_cast<void*>(static_cast<uintptr_t>(sizeType * i)));
        glVertexAttribDivisor(InstanceID + i, 1);
    }
}

