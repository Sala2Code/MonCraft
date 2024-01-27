//  __   __  _______  __    _  _______  ______    _______  _______  _______    
// |  |_|  ||       ||  |  | ||       ||    _ |  |   _   ||       ||       | 
// |       ||   _   ||   |_| ||       ||   | ||  |  |_|  ||    ___||_     _| 
// |       ||  | |  ||       ||       ||   |_||_ |       ||   |___   |   |   
// |       ||  |_|  ||  _    ||      _||    __  ||       ||    ___|  |   |   
// | ||_|| ||       || | |   ||     |_ |   |  | ||   _   ||   |      |   |   
// |_|   |_||_______||_|  |__||_______||___|  |_||__| |__||___|      |___|   

// _______  _______  ___      _______  _______  _______  _______  ______   _______ 
// |       ||   _   ||   |    |   _   ||       ||       ||       ||      | |       |
// |  _____||  |_|  ||   |    |  |_|  ||____   ||       ||   _   ||  _    ||    ___|
// | |_____ |       ||   |    |       | ____|  ||       ||  | |  || | |   ||   |___ 
// |_____  ||       ||   |___ |       || ______||      _||  |_|  || |_|   ||    ___|
// _____| ||   _   ||       ||   _   || |_____ |     |_ |       ||       ||   |___ 
// |_______||__| |__||_______||__| |__||_______||_______||_______||______| |_______|

// The project is still in development, so there are still some bugs and some features are not yet implemented.
// To modify some parameters, you can change the values in the file "constants.cpp".
// To use on small devices, you can change the values in the file "globalVar.cpp" and set parameters to false.
// Then, to update the chunkView (distance of the chunks loaded), you can change the value in the file "include/constants.cpp".

// I suggest to stay on 2/3 chunkViews to have a good experience with all features enabled (water, sky and lens flare).

// To compile the code, see the MakeFile to update the path of lib/include.
// If you have a problem with these files, check the repository of the project (RetroRoad), I suggest in FILE the same lib/include.

// This project is to develop my skills in C++ and OpenGL, the goal is not to make a perfect game (not even a game ahah),
// only try to do beautfiul things and learn a lot of things.
// Don't hesitate to contact me if you have any questions or suggestions.

#include "common/include.hpp"
#include "include/constants.hpp"
#include "common/loadBMP.hpp"
#include "common/loadShader.hpp"
#include "common/TextureArray.hpp"

#include <functional>

#include "include/struct.hpp"
#include "include/controls.hpp"
#include "include/compObj.hpp"
#include "include/buffer.hpp"
#include "include/fps.hpp"
#include "include/chunk.hpp"
#include "include/world.hpp"
#include "include/generation.hpp"
#include "include/draw.hpp"
#include "include/json.hpp"
#include "include/FBO_water.hpp"
#include "include/water.hpp"
#include "include/sky.hpp"
#include "include/globalVar.hpp"
#include "include/gui.hpp"
#include "include/tree.hpp"
#include "include/random.hpp"
#include "include/lens.hpp"
#include "include/foliage.hpp"
#include "include/initChunk.hpp"

// TODO : steve, shadows, tree, collisions 
int main()
{
	srand(static_cast<unsigned int>(time(0)));
	seed = 0.793*(rand()%10000);
	// * Time 
	double lastTime = glfwGetTime();
	double lastTimeFPS = lastTime;
	unsigned int n_frame = 0;
	float deltaTime = 0;

	glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); 
	gladLoadGL();
	glViewport(0, 0, width, height);

    glfwSetCursorPos(window, width/2, height/2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
	// * Shaders 
	shaderBasic = LoadShaders("shader/basic.vert", "shader/basic.frag");
	shaderBasicInstanced = LoadShaders("shader/basicInstanced.vert", "shader/basicInstanced.frag");
	shaderWaterLow = LoadShaders("shader/waterLow.vert", "shader/waterLow.frag");
	shaderWaterHigh = LoadShaders("shader/waterHigh.vert", "shader/waterHigh.frag");
	shaderSkyLow = LoadShaders("shader/sky.vert", "shader/skyLow.frag");
	shaderSkyHigh = LoadShaders("shader/sky.vert", "shader/skyHigh.frag");
	shaderLens = LoadShaders("shader/lens.vert", "shader/lens.frag");

	GLuint shaderProgramRender = LoadShaders("shader/render.vert", "shader/render.frag");
	GLuint resolutionRenderID = glGetUniformLocation(shaderProgramRender, "u_resolution");

	GLuint MVP_Basic_ID = glGetUniformLocation(shaderBasic, "MVP");
	GLuint textID = glGetUniformLocation(shaderBasic, "texture");


	GLuint TextureDudv = loadBMP_custom("img/Water/dudv.bmp");
	GLuint TextureNormal = loadBMP_custom("img/Water/normal.bmp");


	// * Buffer
	// ? Block
	GLuint VAO_Block;
	glGenVertexArrays(1, &VAO_Block);
	glBindVertexArray(VAO_Block);

 	GLuint vertexbufferBlock, uvbufferBlock, normalbufferBlock;
	genBuf(vertexbufferBlock, verticesBlock, sizeof(verticesBlock),0,3);
	genBuf(uvbufferBlock, uvBlock, sizeof(uvBlock),1,2);
	genBuf(normalbufferBlock, normalBlock, sizeof(normalBlock),2,3);
    
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	Buffer bufferBlock = {vertexbufferBlock, uvbufferBlock, normalbufferBlock};

	// * Sky
	GLuint VAO_Sky, vertexbufferSky;
	glGenVertexArrays(1, &VAO_Sky);
	glBindVertexArray(VAO_Sky);
	genBuf(vertexbufferSky, vertices2D, sizeof(vertices2D),0,3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// * Init Map (array in spiral)
	std::array<int, 2> borderChunkMin = {-chunkView, -chunkView}; // Récupère les coordonnées minimum des chunks loads pour savoir quels chunks sont en bordures
	
	std::array<int, chunkLoadSize> chunkLoad, listIndex; // [1;+inf[, [1;chunkLoadSize]
	initChunkLoad(chunkLoad);
	listIndex = chunkLoad;

	std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> chunkHeightMap; 
	initChunkHeightMap(chunkHeightMap, listIndex);

	std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> chunkTreeMap; 
	initChunkTreeMap(chunkTreeMap, listIndex, chunkHeightMap);

	std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> chunkFolMap; 
	initChunkFolMap(chunkFolMap, listIndex, chunkHeightMap);


	Player player(glm::vec3(0, (chunkHeightMap[getIndexChunk(1, 1, listIndex)][0]->height +3) *SCALE, 0)); // assimilate to the camera
	player.worldMap={};
	player.worldEnv={};
	player.worldFol={};
	setupWorldMap(player, chunkLoad, chunkHeightMap, borderChunkMin, listIndex);
	setupTreeMap(player, chunkLoad, chunkTreeMap, chunkHeightMap, borderChunkMin, listIndex);
	setupFolMap(player, chunkLoad, chunkFolMap, chunkHeightMap, borderChunkMin, listIndex);


	TextureArray textureArray;
	loadArrTexture(textureArray);


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


	GLuint queryID, VAO_Lens;
	initLensFlare(queryID, VAO_Lens, vertexbufferBlock); // lensFlare, occlusion queries

	initImGUI(window);

	initInstancing(VAO_Block);

	GLenum err;
	while (!glfwWindowShouldClose(window))
	{
		// * ------ Update ------
		fps(window, lastTime, lastTimeFPS, n_frame, deltaTime); // Update deltaTime and display fps

		glm::vec3 playerBeforePos = player.pos; // to compare with pos after move

		ViewComp viewComp = controlsView(window, deltaTime, player);

		bool hasNewChunk = loadNewChunk(player, chunkLoad, chunkHeightMap, chunkTreeMap, chunkFolMap, playerBeforePos, borderChunkMin);
    	if(hasNewChunk){
			player.worldMap={};
			player.worldEnv={};
			player.worldFol={};

			setupWorldMap(player, chunkLoad, chunkHeightMap, borderChunkMin, listIndex);
			setupTreeMap(player, chunkLoad, chunkTreeMap, chunkHeightMap, borderChunkMin, listIndex);
			setupFolMap(player, chunkLoad, chunkFolMap, chunkHeightMap, borderChunkMin, listIndex);
		}

		// * ------ Render ------
		glEnable(GL_CLIP_DISTANCE0);
		
		setupWater(viewComp, reflectionFBO, refractionFBO, player, textureArray, VAO_Block, VAO_Sky);
	
		activeFBO(defaultFBO);

		renderSky(player, VAO_Sky);

		WaterComp waterComp = {waterTextureReflection, waterTextureRefraction, TextureDudv, TextureNormal};
		renderWater(waterComp, player, viewComp, VAO_Block);

		showMap(viewComp, player, textureArray, VAO_Block);


		if(isSky){
			LensFlare(queryID, VAO_Lens, VAO_Sky, viewComp, player.pos);
		}

		// * ------ GUI (options, F3) ------
		showF3(player);

		glfwSwapBuffers(window);
		glfwPollEvents();
		
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL error: " << err << std::endl;
		}
	}
	delImGUI();

	glDeleteVertexArrays(1, &VAO_Block);
	glDeleteVertexArrays(1, &VAO_Sky);
	glDeleteVertexArrays(1, &VAO_Lens);

	glDeleteProgram(shaderBasic);
	glDeleteProgram(shaderBasicInstanced);
	glDeleteProgram(shaderWaterLow);
	glDeleteProgram(shaderWaterHigh);
	glDeleteProgram(shaderSkyLow);
	glDeleteProgram(shaderSkyHigh);
	glDeleteProgram(shaderLens);

	glDeleteBuffers(1, &vertexbufferBlock);
	glDeleteBuffers(1, &uvbufferBlock);
	glDeleteBuffers(1, &normalbufferBlock);

	glDeleteBuffers(1, &vertexbufferSky);
	
	glDeleteFramebuffers(1, &depthbufferDefault);
	glDeleteFramebuffers(1, &depthbufferRefl);
	glDeleteFramebuffers(1, &depthbufferRefr);

	glDeleteFramebuffers(1, &reflectionFBO);  
	glDeleteFramebuffers(1, &refractionFBO);  

	glDeleteBuffers(1, &waterTextureReflection);
	glDeleteBuffers(1, &waterTextureRefraction);

	glDeleteQueries(1, &queryID);


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}