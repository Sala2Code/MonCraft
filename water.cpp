#include "common/include.hpp"
#include "common/TextureArray.hpp"

#include "include/constants.hpp"
#include "include/struct.hpp"
#include "include/player.hpp"
#include "include/buffer.hpp"
#include "include/FBO_water.hpp"
#include "include/sky.hpp"
#include "include/globalVar.hpp"
#include "include/draw.hpp"

#include "include/water.hpp"

void setupWater(ViewComp &viewComp, GLuint &reflectionFBO, GLuint &refractionFBO, Player &player, TextureArray &textureArray, GLuint &VAO_Block, GLuint &VAO_Sky){
    if(isWater){
        activeFBO(reflectionFBO);
        float distWater = 2*(viewComp.eye.y-WATER_HEIGHT*SCALE - .6*SCALE); // water is at 0.2*SCALE from the surface
        viewComp.eye.y -= distWater; // - 0.05 is to avoid a glitch effect (only useful for reflection)

        viewComp.dir =  glm::reflect(viewComp.dir, glm::vec3(0, 1, 0));
        player.angleVertical *= -1; // TODO : il y avait pas ce bug avant ! :(
		
        renderSky(player, VAO_Sky);
        player.angleVertical *= -1;
        
        // showMapBlock(viewComp, player.worldMap, textureArray, VAO_Block, 1);
        // showMapBlock(viewComp, player.worldEnv, textureArray, VAO_Block, 1);
        showMap(viewComp, player, textureArray, VAO_Block, 1);

        // TODO : PLAYER

        activeFBO(refractionFBO);
        viewComp.eye.y += distWater;
        viewComp.dir =  glm::reflect(viewComp.dir, glm::vec3(0, 1, 0));

        // showMapBlock(viewComp, player.worldMap, textureArray, VAO_Block, -1);
        // showMapBlock(viewComp, player.worldEnv, textureArray, VAO_Block, -1);
        showMap(viewComp, player, textureArray, VAO_Block, -1);
        // ? No sky because this view is underwater
    }
}


void renderWater(WaterComp &waterComp, Player &player, ViewComp &viewComp, GLuint &VAO){
    if(isWater){
        glUseProgram(shaderWaterHigh);
        showWaterHigh(viewComp, player, waterComp, VAO);
    }else{
        glUseProgram(shaderWaterLow);
        showWaterLow(viewComp, player, VAO);
    }
}