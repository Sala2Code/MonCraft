#include "common/include.hpp"

#include "include/constants.hpp"
#include "include/struct.hpp"
#include "include/player.hpp"
#include "include/chunk.hpp"
#include "include/random.hpp"

#include "include/foliage.hpp"

float rateFol(int biome){
    if(biome==3 || biome==4 || biome==7){
        return 0.4;
    }
    return 0.2;
}



void setupFolMap(		
    Player &player,
    std::array<int, chunkLoadSize> &chunkLoad,
    std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,
    std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
    std::array<int, 2> &borderChunkMin,
    std::array<int, chunkLoadSize> &listIndex
){
    for(int i : chunkLoad){
        int chunkCoX, chunkCoZ;
        getCoordChunk(i, chunkCoX, chunkCoZ);
        for(int z=0; z<chunkWidth;z+=4){
            int shiftX = (int)(2.5*z+2)%4;
            for(int x=0; x<chunkWidth;x+=4){
                glm::vec2 pos2D = glm::vec2(chunkCoX*chunkWidth + x + shiftX, chunkCoZ*chunkWidth + z);
                int biome = chunkHeightMap[getIndexChunk(i, player.actualChunk, listIndex)][z*chunkWidth+x+shiftX]->biome;
                if(biome!=1){
                    if( randomBinary2(pos2D, rateFol(biome)) ){
                        glm::vec2 shift = glm::floor(random2(pos2D)+glm::vec2(1,1));
                        if( chunkHeightMap[getIndexChunk(i, player.actualChunk, listIndex)][(z+shift.y)*chunkWidth+x+shiftX+shift.x]->height > WATER_HEIGHT ){
                            BlockInfo* fol = chunkFolMap[getIndexChunk(i, player.actualChunk, listIndex)][(z+shift.y)*chunkWidth + x + shiftX + shift.x].get();
                            player.worldFol.push_back(BlockInfo{fol->pos*glm::vec3(SCALE), fol->biome, fol->texture});
                            
                        }
                    }
                }
            }
        }
    }
}
