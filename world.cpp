#include "common/include.hpp"

#include <algorithm>
#include "include/player.hpp"
#include "include/constants.hpp"
#include "include/generation.hpp"
#include "include/struct.hpp"
#include "include/chunk.hpp"
#include "include/random.hpp"
#include "include/tree.hpp"

#include "include/world.hpp"

bool isBlockInView(Frustum &frustum, glm::vec3 &vecObj){
    return
		(glm::dot(vecObj, frustum.nearFace) > 0) &&  
    	(glm::dot(vecObj, frustum.rightFace) > 0)&&
    	(glm::dot(vecObj, frustum.leftFace) > 0) &&
		(glm::dot(vecObj, frustum.bottomFace) < 0) &&
    	(glm::dot(vecObj, frustum.topFace) > 0);
}


void setupWorldMap(
		Player &player,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		std::array<int, chunkLoadSize> &listIndex
){	
	for(int i : chunkLoad){
		int chunkX, chunkZ;
		getCoordChunk(i, chunkX, chunkZ);
		unsigned int indexChunk = getIndexChunk(i, player.actualChunk, listIndex);

		for(int jz=0;jz<chunkWidth;jz++){
			for(int jx=0;jx<chunkWidth;jx++){
				// AngleView is turn by pi/2 so we got an ordinary coordinate system : only for(equa/Img)Droite
				// compare line and block to know if it's on or underneath. Then, display it if the player look in this direction
				
				int maxHeight = chunkHeightMap[indexChunk][jz*chunkWidth+jx]->height; // Surface

				// Get the height of blocks next to the current block
				// Double ternary to check border and corner cases
				int heightLeft = (jx != chunkWidth-1) ? chunkHeightMap[indexChunk][jz*chunkWidth+(jx+1)]->height : (chunkX!=borderChunkMin[0]+2*chunkView) ? chunkHeightMap[getIndexChunk( getNumChunk(chunkX+1, chunkZ, 1), player.actualChunk, listIndex)][jz*chunkWidth]->height : 0;
				int heightRight = (jx != 0) ? chunkHeightMap[indexChunk][jz*chunkWidth+(jx-1)]->height : (chunkX!=borderChunkMin[0]) ? chunkHeightMap[getIndexChunk( getNumChunk(chunkX-1, chunkZ, 1), player.actualChunk, listIndex)][jz*chunkWidth+(chunkWidth-1)]->height : 0;
				int heightBehind = (jz != 0) ? chunkHeightMap[indexChunk][(jz-1)*chunkWidth+jx]->height : (chunkZ!=borderChunkMin[1])  ? chunkHeightMap[getIndexChunk( getNumChunk(chunkX, chunkZ-1, 1), player.actualChunk, listIndex)][(chunkWidth-1)*chunkWidth+jx]->height : 0;
				int heightFront =  (jz != chunkWidth-1) ? chunkHeightMap[indexChunk][(jz+1)*chunkWidth+jx]->height  : (chunkZ!=borderChunkMin[1]+2*chunkView)  ? chunkHeightMap[getIndexChunk( getNumChunk(chunkX, chunkZ+1, 1), player.actualChunk, listIndex)][jx]->height : 0;

				int maxDiff = 0;

				// Get the diff with neighbors blocks (4 corners and others blocks)
				if(chunkX==borderChunkMin[0] && jx==0){
					maxDiff = std::max({ maxHeight - heightLeft, maxHeight - heightBehind, maxHeight - heightFront}); 
				}else if(chunkX==borderChunkMin[0]+2*chunkView && jx==chunkWidth-1 ){
					maxDiff = std::max({ maxHeight - heightRight, maxHeight - heightBehind, maxHeight - heightFront});
				}else if(chunkZ==borderChunkMin[1] && jz==0){
					maxDiff = std::max({ maxHeight - heightLeft, maxHeight - heightRight, maxHeight - heightFront}); // Without behind
				}else if(chunkZ==borderChunkMin[1]+2*chunkView && jz==chunkWidth-1 ){
					maxDiff = std::max({ maxHeight - heightLeft, maxHeight - heightRight, maxHeight - heightBehind}); // Without front
				}else{
					maxDiff = std::max({ maxHeight - heightLeft, maxHeight - heightRight, maxHeight - heightBehind, maxHeight - heightFront});
				}
			
				for(int k=maxHeight-1;k>0;k--){ // slighly optimisation
					if(
						((k+1==maxHeight) || (player.pos.y<=0 && k==0)) // Show all faces below (above) the player // ? Normally you can't see above
						// ? You can delete a part of this because if we are still far from the border (and keep it if you want to see the outside of the world) 
						// || // Border X
						// (chunkX==borderChunkMin[0] && player.pos.x<chunkX*chunkWidth && jx==0) || (chunkX==borderChunkMin[0]+chunkView && player.pos.x>chunkX*chunkWidth+(chunkWidth-1) && jx==chunkWidth-1) 
						// || // Border Z
						// (chunkZ==borderChunkMin[1] && player.pos.z<chunkZ*chunkWidth && jz==0) || (chunkZ==borderChunkMin[1]+chunkView && player.pos.z>chunkZ*chunkWidth+(chunkWidth-1) && jz==chunkWidth-1) 
						
						|| // Bloc visible but not their above face
						(k >= maxHeight-maxDiff)
					){
							// GLuint shader = shaderTexture[0][0];
							glm::vec3 cubePosition{chunkX*chunkWidth+jx, k+1, chunkZ*chunkWidth+jz};
							cubePosition *= glm::vec3(SCALE);
							int biome = chunkHeightMap[indexChunk][jz*chunkWidth+jx]->biome;
							int indexBlock = getTextureBlock(biome, maxHeight-1-k, cubePosition); 
							player.worldMap.push_back(BlockInfo{cubePosition, biome, indexBlock});
					}else{
						break;
					}
				}
				if(maxHeight < WATER_HEIGHT){
					// GLuint shader = shaderTexture[0][1];
					glm::vec3 cubePosition{chunkX*chunkWidth+jx, WATER_HEIGHT, chunkZ*chunkWidth+jz}; 
					cubePosition *= glm::vec3(SCALE);
					int biome = chunkHeightMap[indexChunk][jz*chunkWidth+jx]->biome;
					player.worldMap.push_back(BlockInfo{cubePosition, biome, -1}); // TODO No texture
				}
			}
		}
	}
}




int getTexture(std::string name){
	return 3 * textureMap.find(name)->second; // 3 textures per block (up, side, down)
}

int getTextureBlock(int biome, int distSurf, glm::vec3 pos){ // distSurf : difference to the surface
	glm::vec2 seedRandom = glm::vec2(pos.x, pos.z);

	int isPodzol = random2_1(pos, 0, 100);
	switch(biome){
		case 0:
			if(distSurf==0){
				if(isPodzol < 70){
					return getTexture("grass_taiga");
				}else if(isPodzol<92){
					return getTexture("dirt");
				}
			}
			return setup_grass("grass", distSurf, seedRandom);

		case 1:
			if(pos.y>(60. + random2_1(seedRandom, -6, 2))*SCALE){ // high -> snow
				if(distSurf < random2_1(seedRandom, 4, 10)){ // under the snow there is stone
					return getTexture("snow");
				}
			}else if(pos.y< (40 + random2_1(seedRandom, -8, 8)) * SCALE ){
				return setup_grass("grass", distSurf, seedRandom);
			}
			return getTexture("stone");

		case 2: case 3: case 4: case 5:
			return setup_grass("grass", distSurf, seedRandom);

		case 6:
			return getTexture("sand");

		case 7:
			return setup_grass("grass_swamp", distSurf, seedRandom);
			
		default:
			return getTexture("dirt");
	}
}

int setup_grass(std::string name, int distSurf, glm::vec2 seedRandom){
	if(distSurf==0){
		return getTexture(name);
	}else if(distSurf<random2_1(seedRandom, 3, 6)){
		return getTexture("dirt");
	}
	return getTexture("stone");
}