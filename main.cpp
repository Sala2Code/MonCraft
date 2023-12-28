// #include <functional>
#include <algorithm> // Rotate
#include <array>

#include "common/include.hpp"
#include "include/constants.hpp"

#include "common/loadShader.hpp"
#include "common/loadBMP.hpp"
#include "common/loadOBJ.hpp"
#include "common/VBOindex.hpp"

#include "include/initObjects.hpp"
#include "include/compObjects.hpp"
#include "include/player.hpp"
#include "include/fps.hpp"
#include "include/controls.hpp"
#include "include/generation.hpp"
#include "include/world.hpp"
#include "include/display.hpp" // F3 & drawElements
#include "include/activeElements.hpp"


/* Coordinate system
	   ^ z(+)
       |
(x+)   |
<------O---
       |
*/
// ! TLDR : It's not the real project, so somes functions are not used and the code is not clean.

// ! READ :
// This project is about the tutorial on waterEffect. So I clean the code fastly
// and I don't use all functions and delete them/rearrange. Normally, the video
// allows to understand the logic behind the effect of water.
// So sorry fro the lack of comments and the bad code.
// The map initialize chunk but don't generate to avoid confusion.
// The main branch, is a little bit different but the structure and logic is the same with
// a better code and more comments, cleaner.
// the code for waterEffect is main.cpp (init) -> activeElements.cpp (activeFBO) -> display.cpp (showWater) -> water.frag (shader)
// read the make file to config the project

int main(){
	// * Time 
	double lastTime = glfwGetTime();
	double lastTimeFPS = lastTime;
	unsigned int n_frame = 0;
	float deltaTime = 0;

	// * Options
	bool isPaused = false;
	// * World generation
	std::array<int, 2*chunkView+1> tempLoad; // temp array to new chunkLoad
	std::array<std::array<BlockMap, chunkArea>, 2*chunkView+1> tempHeight; // temp array to new value of chunkHeightMap
	std::array<int, 2> borderChunkMin = {-chunkView, -chunkView}; // Récupère les coordonnées minimum des chunks loads pour savoir quels chunks sont en bordures

	// * Init GLFW
	glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "3D to 2D", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); 
	gladLoadGL();
	glViewport(0, 0, width, height);

	// * Camera
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 View; //  = glm::lookAt(glm::vec3(4,3,3),glm::vec3(0,0,0), glm::vec3(0,1,0)) ;
	glm::mat4 Projection = glm::perspective(glm::radians(FoV), (float) width / (float)height, 0.1f, 100.0f);
	glm::mat4 mvp; // =  Projection * View * Model ;

	// * Shader
	GLuint shaderProgramBasic = LoadShaders("shader/basic.vert", "shader/basic.frag");
	GLuint shaderProgramBasicInstanced = LoadShaders("shader/basicInstanced.vert", "shader/basic.frag");
	GLuint shaderProgramHitBox = LoadShaders("shader/hitBox.vert", "shader/hitBox.frag");
	GLuint shaderProgramSky = LoadShaders("shader/sky.vert", "shader/sky.frag");
	GLuint shaderProgramPaused = LoadShaders("shader/paused.vert", "shader/paused.frag");
	GLuint shaderProgramWater = LoadShaders("shader/water.vert", "shader/water.frag");
	GLuint shaderProgramRender = LoadShaders("shader/render.vert", "shader/render.frag");

	// GLuint shaderProgramRenderTemp = LoadShaders("shader/paused.vert", "shader/renderTemp.frag");

	// * Texture
    GLuint TextureDirt = loadBMP_custom("img/Dirt.bmp");
	GLuint TextureSteve = loadBMP_custom("img/steve.bmp");

	GLuint dudvMap = loadBMP_custom("img/dudv.bmp");


	// * Uniforms
	GLuint ViewMatrixID = glGetUniformLocation(shaderProgramBasic, "View");
	GLuint LightID = glGetUniformLocation(shaderProgramBasic, "LightPosition_worldspace");
	GLuint TextureBasicID  = glGetUniformLocation(shaderProgramBasic, "myTextureSampler");
	GLuint BiomeBasicID = glGetUniformLocation(shaderProgramBasic, "iBiome");

	GLuint angleViewID = glGetUniformLocation(shaderProgramSky, "angleView");
	GLuint resolutionSkyID = glGetUniformLocation(shaderProgramSky, "u_resolution");
	GLuint timeSkyID = glGetUniformLocation(shaderProgramSky, "u_time");

	GLuint resolutionRenderID = glGetUniformLocation(shaderProgramRender, "u_resolution");

	// * Init for FBO 
	glUseProgram(shaderProgramRender);
    glUniform1i( glGetUniformLocation(shaderProgramRender, "screenTexture"),0);


	// * -------------------------------------------- *
	// * Init (array in spiral)
	// Each chunk are in [1;chunkLoadSize] (with decay's player) 
	std::array<int, chunkLoadSize> chunkLoad; // [1;+inf[
	std::array<int, chunkLoadSize> listIndex; // [1;chunkLoadSize]
	unsigned int lengthArr = (1+2*chunkView);
	for(int z=-chunkView;z<=chunkView;z++){ // Assign in spiral order chunk load from (0;0)
		for(int x=-chunkView;x<=chunkView;x++){
			chunkLoad[(z+chunkView)*lengthArr+(x+chunkView)] = getNumChunk(x, z, 1);
		}
	} 
	listIndex = chunkLoad;
	// Create chunkHeightMap
	std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> chunkHeightMap; 
	std::array<BlockMap, chunkArea> tempHeightMap;
	int initChunk = 1; // chunk where player spawn
	for(int i : chunkLoad){
		int chunkCoX, chunkCoZ;
		getCoordChunk(i, chunkCoX, chunkCoZ);
		for(int z=0; z<chunkWidth;z++){
			for(int x=0; x<chunkWidth;x++){
				tempHeightMap[z*chunkWidth+x].height = perlin2DheightMap((float)chunkCoX*chunkWidth + x,  (float)chunkCoZ*chunkWidth + z, heightMap);
				tempHeightMap[z*chunkWidth+x].biome = cellularBiome((float)chunkCoX*chunkWidth + x,  (float)chunkCoZ*chunkWidth + z);
			}
		}
		chunkHeightMap[getIndexChunk(i, initChunk, listIndex)] = tempHeightMap;
	}
	// * -------------------------------------------- *



	GLuint VAO, VBO;
	generateVAO_VBO(VAO, VBO);

	// * Sky
	GLuint EBO_2D;
	init2D(EBO_2D);

	// * Block

	GLuint EBO_Block;
	GLuint vertexbufferBlock;
	GLuint uvbufferBlock;
	GLuint normalbufferBlock;
	bufferComp bufferBlock = {EBO_Block, vertexbufferBlock, uvbufferBlock, normalbufferBlock};
	std::vector<unsigned short> iBlock;
	initBlock(bufferBlock, iBlock);
	
	// * Player
	GLuint EBO_Player;
	GLuint vertexbufferPlayer;
	GLuint uvbufferPlayer;
	GLuint normalbufferPlayer;
	bufferComp bufferPlayer = {EBO_Player, vertexbufferPlayer, uvbufferPlayer, normalbufferPlayer};
	std::vector<unsigned short> iPlayer;

	glm::vec3 pos_player = SCALE*glm::vec3(0.5, chunkHeightMap[0][0].height+1, 0.5);
	Player player = initPlayer( bufferPlayer, iPlayer, pos_player);	

    glfwSetCursorPos(window, width/2, height/2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// * Light
	glm::vec3 lightPos = glm::vec3(0,20,0);

	ViewComp viewComp;
	// init world : 
	std::vector<BlockInfo> worldMap = {};
	BlockShader blockShader = { shaderProgramBasicInstanced, shaderProgramWater, TextureDirt};
	displayMap(player, chunkLoad, chunkHeightMap, tempHeightMap, tempHeight, tempLoad, borderChunkMin, blockShader, listIndex, worldMap);


	// ! Viewport (for render screen)
	GLuint vertexbufferQuad;
	glGenBuffers(1, &vertexbufferQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	GLuint uvsbufferQuad;
	glGenBuffers(1, &uvsbufferQuad);
	glBindBuffer(GL_ARRAY_BUFFER, uvsbufferQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadUVs), quadUVs, GL_STATIC_DRAW);

	GLuint vertexbufferQuad2;
	glGenBuffers(1, &vertexbufferQuad2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferQuad2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices2), quadVertices2, GL_STATIC_DRAW);
	// ! ----

	// * FBO (water effect)
	GLuint defaultFBO = 0;
	GLuint reflectionFBO, refractionFBO;
	GLuint depthbufferDefault, depthbufferRefl, depthbufferRefr;
	GLuint waterTextureReflection, waterTextureRefraction;

	genBuf_Depth(depthbufferDefault);
    	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbufferDefault);

	genBuf_Depth(depthbufferRefl);
	genBuf_Water(reflectionFBO, waterTextureReflection, depthbufferRefl);
	
	genBuf_Depth(depthbufferRefr);
	genBuf_Water(refractionFBO, waterTextureRefraction, depthbufferRefr);

	
	while (!glfwWindowShouldClose(window))
	{
		fps(window, lastTime, lastTimeFPS, n_frame, deltaTime); // Update deltaTime and display fps
		paused(window, isPaused); // paused ?
		viewComp = controlsView(window, deltaTime, player, chunkHeightMap, listIndex); // update player and view



		glEnable(GL_CLIP_DISTANCE0); // reflection and refraction
		// * Water effect 
		activeFBO(reflectionFBO);

		float distWater = 2*(viewComp.eye.y-WATER_HEIGHT*SCALE);
		viewComp.eye.y -= distWater;
		View = glm::lookAt(viewComp.eye,  viewComp.eye + glm::reflect(viewComp.dir, glm::vec3(0, 1, 0)) , glm::vec3(0,1,0));
		blockShader.ProjView = Projection * View;
		
		float up = 1; // glClipPlane
		showMap(bufferBlock, player, blockShader, iBlock, up, worldMap);
		
		// display player, elements, ...
		activeSky(shaderProgramSky, EBO_2D, player.angleView, -player.angleVertical); // invert pitch



		activeFBO(refractionFBO);
		viewComp.eye.y += distWater;
		View = glm::lookAt(viewComp.eye, viewComp.dir + viewComp.eye, glm::vec3(0,1,0));
		blockShader.ProjView = Projection * View;

		up = -1;
		showMap(bufferBlock, player, blockShader, iBlock, up, worldMap);
		// ? No sky because this view is underwater
	
		// * end water effect

		
		activeFBO(defaultFBO);
		up = 0;
		View = glm::lookAt(viewComp.eye, viewComp.dir + viewComp.eye, glm::vec3(0,1,0));
		blockShader.ProjView = Projection * View;

		blockShader.shaderReflection = waterTextureReflection;
		blockShader.shaderRefraction = waterTextureRefraction;
		blockShader.dudv = dudvMap;

		showMap(bufferBlock, player, blockShader, iBlock, up, worldMap);
		showWater(bufferBlock, player, blockShader, iBlock, up, worldMap);

		// * Player
		glUseProgram(shaderProgramBasic);
		bindTexture(TextureSteve, TextureBasicID);
		showPlayer(player, bufferPlayer, blockShader.ProjView, shaderProgramBasic, player.thirdView);
		activeSky(shaderProgramSky, EBO_2D, player.angleView, player.angleVertical);


		// ! viewPort 
		glUseProgram(shaderProgramRender);
		glUniform2f(resolutionRenderID, width, height);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waterTextureReflection); // change to see reflection or refraction (waterTextureRefraction)
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferQuad);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvsbufferQuad);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

		unbindBuffer(2);
		// ! end


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteBuffers(1, &EBO_2D);

	glDeleteBuffers(1, &EBO_Block);
    glDeleteBuffers(1, &vertexbufferBlock);
    glDeleteBuffers(1, &uvbufferBlock);
    glDeleteBuffers(1, &normalbufferBlock);

	glDeleteBuffers(1, &EBO_Player);
	glDeleteBuffers(1, &vertexbufferPlayer);
    glDeleteBuffers(1, &uvbufferPlayer);
    glDeleteBuffers(1, &normalbufferPlayer);

	glDeleteBuffers(1, &player.bufferHitBox.EBO);
	glDeleteBuffers(1, &player.bufferHitBox.vertex);
    glDeleteBuffers(1, &player.bufferHitBox.uv);
    glDeleteBuffers(1, &player.bufferHitBox.normal);

	glDeleteBuffers(1, &vertexbufferQuad);
	glDeleteBuffers(1, &vertexbufferQuad2);
	glDeleteBuffers(1, &uvsbufferQuad);


	glDeleteProgram(shaderProgramBasic);
	glDeleteProgram(shaderProgramBasicInstanced);
	glDeleteProgram(shaderProgramHitBox);
	glDeleteProgram(shaderProgramSky);
	glDeleteProgram(shaderProgramPaused);
	glDeleteProgram(shaderProgramRender);


	glDeleteBuffers(1, &TextureDirt);
	glDeleteBuffers(1, &TextureSteve);

	glDeleteFramebuffers(1, &depthbuffer);
	glDeleteFramebuffers(1, &reflectionFBO);  
	glDeleteFramebuffers(1, &refractionFBO);  

	glDeleteBuffers(1, &waterTextureReflection);
	glDeleteBuffers(1, &waterTextureRefraction);

	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
