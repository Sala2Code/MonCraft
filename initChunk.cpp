#include "common/include.hpp"

#include <algorithm>
#include "include/player.hpp"
#include "include/constants.hpp"
#include "include/generation.hpp"
#include "include/struct.hpp"
#include "include/chunk.hpp"
#include "include/random.hpp"
#include "include/tree.hpp"
#include "include/foliage.hpp"

#include "include/initChunk.hpp"

void initChunkLoad(std::array<int, chunkLoadSize> &chunkLoad){
	unsigned int lengthArr = (1+2*chunkView);
	for(int z=-chunkView;z<=chunkView;z++){ // Assign in spiral order chunk load from (0;0)
		for(int x=-chunkView;x<=chunkView;x++){
			chunkLoad[(z+chunkView)*lengthArr+(x+chunkView)] = getNumChunk(x, z, 1);
		}
	} 
}

// ? listIndex = chunkLoad for initialization
void initChunkHeightMap(std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap, std::array<int, chunkLoadSize> &listIndex){
	for(int i : listIndex){ // get for each block : height and biome
		int chunkCoX, chunkCoZ;
		getCoordChunk(i, chunkCoX, chunkCoZ);
		for(int z=0; z<chunkWidth;z++){
			for(int x=0; x<chunkWidth;x++){
				std::unique_ptr<BlockMap> &block = chunkHeightMap[getIndexChunk(i, 1, listIndex)][z*chunkWidth+x];
                block = std::make_unique<BlockMap>(); // Allocation dynamique
                block->height_own = getHeightMap((float)chunkCoX*chunkWidth + x, (float)chunkCoZ*chunkWidth + z, heightScale);
                block->biome = voronoiBiome(glm::vec2(chunkCoX*chunkWidth + x, chunkCoZ*chunkWidth + z));
				// block->height = block->height_own;
			}
		}
	}
	for(int i : listIndex){ // for each block modulate height according to position, neighbors and biome
		int chunkCoX, chunkCoZ;
		getCoordChunk(i, chunkCoX, chunkCoZ);
		setTempHeightMapHeight(chunkHeightMap, chunkCoX, chunkCoZ, listIndex, getIndexChunk(i, 1, listIndex));
	}
}
void initChunkTreeMap(
		std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
		std::array<int, chunkLoadSize> &listIndex,
		std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap
	){
	// subdivising each chunk in square 2x2 altenate like chessboard
	// each corner of this 2x2 is the reference to know where is the in this square if there is a tree
	for(int i : listIndex){ // get for each block if tree or not
		int chunkCoX, chunkCoZ;
		getCoordChunk(i, chunkCoX, chunkCoZ);
		for(int z=0; z<chunkWidth;z+=4){
			for(int x=0; x<chunkWidth;x+=4){
				int shiftX = (int)(2.5*z)%4;
				glm::vec2 pos2D = glm::vec2(chunkCoX*chunkWidth + x + shiftX, chunkCoZ*chunkWidth + z);
				int biome = chunkHeightMap[getIndexChunk(i, 1, listIndex)][z*chunkWidth+x]->biome;
				if(biome!=1 && biome!=6){ // no snow and desert
					if( randomBinary2(pos2D, rateTree(biome)) ){ 
						glm::vec2 shift = glm::floor(random2(pos2D)+glm::vec2(1,1));
						int heightTree = chunkHeightMap[getIndexChunk(i, 1, listIndex)][(z+shift.y)*chunkWidth + x+shiftX+shift.x]->height;
						if(heightTree > WATER_HEIGHT){
							glm::vec3 pos = glm::vec3(pos2D.x + shift.x, heightTree, pos2D.y + shift.y);
							chunkTreeMap[getIndexChunk(i, 1, listIndex)][((z + shift.y) * chunkWidth + x + shiftX + shift.x)] = std::move(generateTree(pos));
						}
					}
				}
			}
		}
	}
}


void initChunkFolMap(
    std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,
    std::array<int, chunkLoadSize> &listIndex,
    std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap
){
    // subdivising each chunk in square 2x2 altenate like chessboard
	// each corner of this 2x2 is the reference to know where is the in this square if there is a tree
    // shift of 2, comparated to initChunkTreeMap
	for(int i : listIndex){ // get for each block if tree or not
		int chunkCoX, chunkCoZ;
		getCoordChunk(i, chunkCoX, chunkCoZ);
		for(int z=0; z<chunkWidth;z+=4){
			for(int x=0; x<chunkWidth;x+=4){
				int shiftX = (int)(2.5*z+2)%4;
				glm::vec2 pos2D = glm::vec2(chunkCoX*chunkWidth + x + shiftX, chunkCoZ*chunkWidth + z);
				int biome = chunkHeightMap[getIndexChunk(i, 1, listIndex)][z*chunkWidth+x]->biome;
				if(biome!=1){ // no snow
					if( randomBinary2(pos2D, rateFol(biome)) ){ 
						glm::vec2 shift = glm::floor(random2(pos2D)+glm::vec2(1,1));
						int heightFol = chunkHeightMap[getIndexChunk(i, 1, listIndex)][(z+shift.y)*chunkWidth + x+shiftX+shift.x]->height;
						if(heightFol > WATER_HEIGHT){
							glm::vec3 pos = glm::vec3(pos2D.x + shift.x, heightFol+1, pos2D.y + shift.y);
                            std::unique_ptr<BlockInfo> &fol = chunkFolMap[getIndexChunk(i, 1, listIndex)][((z + shift.y) * chunkWidth + x+shiftX+shift.x)];
                            fol = std::make_unique<BlockInfo>(); // Allocation dynamique
                            fol->pos = pos;
                            fol->biome = biome;
							fol->texture = (biome==6)? 30 : 27;
                        }
					}
				}
			}
		}
	}


}
