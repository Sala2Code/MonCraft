#include "common/include.hpp"
#include "common/TextureArray.hpp"

#include "include/constants.hpp"
#include "include/globalVar.hpp"
#include "include/compObj.hpp"
#include "include/player.hpp"
#include "include/buffer.hpp"
#include "include/world.hpp"
#include "include/controls.hpp"
#include "include/calcul.hpp"

#include "include/draw.hpp"


GLuint idTexBuffer, instanceBuffer, instanceBufferModels;
std::vector<glm::mat4> instanceMatrices;
std::vector<glm::mat4> instanceModels;
std::vector<float> instanceTexID;

void initInstancing(GLuint &VAO) {
    int maxInstances = chunkArea * chunkLoadSize * 2; // multiply by 2 to be sure of size
    instanceMatrices.reserve(maxInstances);
    instanceModels.reserve(maxInstances);
    instanceTexID.reserve(maxInstances);
    
    glBindVertexArray(VAO);

    glGenBuffers(1, &instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::mat4), &instanceMatrices[0], GL_STATIC_DRAW);
    instancedRendering("MVP_instance", sizeof(glm::vec4), 4, 3);

    glGenBuffers(1, &instanceBufferModels);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBufferModels);
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::mat4), &instanceModels[0], GL_STATIC_DRAW);
    instancedRendering("Models_instance", sizeof(glm::vec4), 4, 7);


    glGenBuffers(1, &idTexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, idTexBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(float), &instanceTexID[0], GL_STATIC_DRAW);
    instancedRendering("idTex_instance", sizeof(float), 1, 11); // location is useless for int/vec

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


// face 0 : up
// face 1-4 : side
// face 5 : down
void drawBlockFace(int face, int nbFace){
    for(int i=face; i<face+nbFace+1;i++){
        glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces, 4*i);
    }
}
void drawBlockFace_Instanced(int face, int nbEl, int nbFace){
    for(int i=face; i<face+nbFace+1;i++){
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces, nbEl, 4*i);
    }
}

void instancedRendering(std::string attribute, float sizeType, int n, int location){
    GLuint InstanceID = glGetAttribLocation(shaderBasicInstanced, attribute.c_str());
    glEnableVertexAttribArray(InstanceID);
    for (unsigned int i = 0; i<n; ++i) {
        glEnableVertexAttribArray(location + i);
        glVertexAttribPointer(InstanceID + i, n, GL_FLOAT, GL_FALSE,n*sizeType, reinterpret_cast<void*>(static_cast<uintptr_t>(sizeType * i)));
        glVertexAttribDivisor(InstanceID + i, 1);
    }
}

void showMap(ViewComp viewComp, Player &player, TextureArray &textureArray, GLuint &VAO, float up){
    glm::mat4 View = glm::lookAt(viewComp.eye, viewComp.dir + viewComp.eye, glm::vec3(0,1,0));
	glm::mat4 ProjView = Projection * View;

    Frustum frustum = createFrustum(viewComp);
    textureArray.ActivateShaderArray(shaderBasicInstanced);

    GLuint upID = glGetUniformLocation(shaderBasicInstanced, "up");
    glUniform1f(upID, up);
    GLuint waterHeightID = glGetUniformLocation(shaderBasicInstanced, "WATER_HEIGHT");
    glUniform1f(waterHeightID, (WATER_HEIGHT+0.8)*SCALE);

	glBindVertexArray(VAO);
    showMapBlock(player.worldMap, frustum, ProjView, viewComp.eye, VAO, up);
    showMapBlock(player.worldEnv, frustum, ProjView, viewComp.eye, VAO, up);
    showFoliage(player.worldFol, frustum, ProjView, viewComp.eye, VAO, up);
    glBindVertexArray(0);
}

void showMapBlock(std::vector<BlockInfo> &l_block, Frustum &frustum, glm::mat4 ProjView, glm::vec3 eye, GLuint &VAO, float up){
    instanceMatrices.clear();
    instanceModels.clear();
    instanceTexID.clear();
    for(auto& block : l_block){
        if(block.texture>=0){ // not water
            glm::vec3 blockToPlayer = block.pos - eye;
            if(isBlockInView(frustum, blockToPlayer)){
                glm::mat4 Model = glm::translate(glm::mat4(1.0f), block.pos);

                // z-fighting
                float maxAbs = max3(glm::abs(blockToPlayer));
                Model = (maxAbs < 3) ?  glm::scale(Model, glm::vec3(1-0.0002)) :
                        (maxAbs < 20) ? glm::scale(Model, glm::vec3(1-0.001)) : 
                                        glm::scale(Model, glm::vec3(1 - 0.005));

                Model = glm::scale(Model, glm::vec3(SCALE));

                glm::mat4 mvpMatrix = ProjView * Model;
                
                instanceModels.push_back(Model);
                instanceMatrices.push_back(mvpMatrix);
                instanceTexID.push_back(block.texture);  
            }
        }
    }

    // * pass to vertex buffer
    GLuint shift_ID = glGetUniformLocation(shaderBasicInstanced, "shiftID");
    glUniform1f(shift_ID, 0);

    processBuffers(idTexBuffer, instanceTexID, instanceBuffer, instanceMatrices, instanceBufferModels, instanceModels);

    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces, instanceMatrices.size(), 0);

    glUniform1f(shift_ID, 1);
    for(int i=1; i<5;i++){
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces, instanceMatrices.size(), 4*i);
    }

    glUniform1f(shift_ID, 2);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces, instanceMatrices.size(), 20);
}


void showWaterLow(ViewComp &viewComp, Player &player, GLuint &VAO){
    glm::mat4 View = glm::lookAt(viewComp.eye, viewComp.dir + viewComp.eye, glm::vec3(0,1,0));
	glm::mat4 ProjView = Projection * View;

    Frustum frustum = createFrustum(viewComp);

    instanceMatrices.clear();
    instanceModels.clear();
	for(auto &block : player.worldMap){
		if(block.texture < 0){  
            glm::vec3 blockToPlayer = block.pos - viewComp.eye;
            if(isBlockInView(frustum, blockToPlayer)){
                glm::mat4 Model = glm::translate(glm::mat4(1.0f), block.pos);

                Model = glm::scale(Model, glm::vec3(SCALE, SCALE*0.8, SCALE));
                Model = glm::translate(Model, glm::vec3(0,-2*SCALE, 0));
                glm::mat4 mvpMatrix = ProjView * Model;

                instanceMatrices.push_back(mvpMatrix);
                instanceModels.push_back(Model);
            }
		}
	}
    processBuffers(instanceBuffer, instanceMatrices, instanceBufferModels, instanceModels);

	glBindVertexArray(VAO);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces, instanceMatrices.size(), 0);
    glBindVertexArray(0);
}

void showWaterHigh(ViewComp viewComp, Player &player, WaterComp waterComp, GLuint &VAO){
    glm::mat4 View = glm::lookAt(viewComp.eye, viewComp.dir + viewComp.eye, glm::vec3(0,1,0));
	glm::mat4 ProjView = Projection * View;

    Frustum frustum = createFrustum(viewComp);
    
    instanceMatrices.clear();
    instanceModels.clear();
	for(auto &block : player.worldMap){
		if(block.texture < 0){
            glm::vec3 blockToPlayer = block.pos - viewComp.eye;
            if(isBlockInView(frustum, blockToPlayer)){
                glm::mat4 Model = glm::translate(glm::mat4(1.0f), block.pos);
                Model = glm::scale(Model, glm::vec3(SCALE, SCALE*0.8, SCALE));
                Model = glm::translate(Model, glm::vec3(0,-2*SCALE, 0));

                glm::mat4 mvpMatrix = ProjView * Model;

                instanceMatrices.push_back(mvpMatrix);
                instanceModels.push_back(Model);
            }

		}
	}
    processBuffers(instanceBuffer, instanceMatrices, instanceBufferModels, instanceModels);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterComp.reflection);
    glUniform1i( glGetUniformLocation(shaderWaterHigh, "reflectionShader"),0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, waterComp.refraction);
    glUniform1i( glGetUniformLocation(shaderWaterHigh, "refractionShader"),1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, waterComp.dudv);
    glUniform1i( glGetUniformLocation(shaderWaterHigh, "dudvMap"),2);

    // ? You can add normalMap here
    // I don't like the effect so I don't use it

	GLuint timeID = glGetUniformLocation(shaderWaterHigh, "u_time");
	GLuint posID = glGetUniformLocation(shaderWaterHigh, "posCam");

    glUniform1f(timeID, glfwGetTime()*flowTime);
    glUniform3f(posID, player.pos.x, player.pos.y, player.pos.z);

	glBindVertexArray(VAO);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces, instanceMatrices.size(), 0);
    glBindVertexArray(0);
}


void showFoliage(std::vector<BlockInfo> &l_block,  Frustum &frustum, glm::mat4 ProjView, glm::vec3 eye, GLuint &VAO, float up){
    instanceMatrices.clear();
    instanceModels.clear();
    instanceTexID.clear();
    for(auto& block : l_block){
        if(block.texture>=0){ // not water
            glm::vec3 blockToPlayer = block.pos - eye;
            if(isBlockInView(frustum, blockToPlayer)){
                glm::mat4 Model = glm::mat4(1.0);
                Model = glm::translate(Model, block.pos);
                Model = glm::rotate(Model, glm::radians(-45.0f), glm::vec3(0, 1, 0));
                Model = glm::scale(Model, glm::vec3(1.41));
                Model = glm::scale(Model, glm::vec3(SCALE));

                glm::mat4 mvpMatrix = ProjView * Model;
                
                instanceModels.push_back(Model);
                instanceMatrices.push_back(mvpMatrix);
                instanceTexID.push_back(block.texture);  

                Model = glm::mat4(1.0);
                Model = glm::translate(Model, block.pos + glm::vec3(0,0,SCALE));
                Model = glm::rotate(Model, glm::radians(45.0f), glm::vec3(0, 1, 0));
                Model = glm::scale(Model, glm::vec3(1.41));
                Model = glm::scale(Model, glm::vec3(SCALE));

                mvpMatrix = ProjView * Model;
                
                instanceModels.push_back(Model);
                instanceMatrices.push_back(mvpMatrix);
                instanceTexID.push_back(block.texture);  
            }
        }
    }

    // * pass to vertex buffer
    GLuint shift_ID = glGetUniformLocation(shaderBasicInstanced, "shiftID");

    processBuffers(idTexBuffer, instanceTexID, instanceBuffer, instanceMatrices, instanceBufferModels, instanceModels);

    glUniform1f(shift_ID, 0);
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indexFaces, instanceMatrices.size(), 4);
}