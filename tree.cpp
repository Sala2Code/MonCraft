#include "common/include.hpp"

#include "include/player.hpp"
#include "include/chunk.hpp"
#include "include/world.hpp"
#include "include/generation.hpp"
#include "include/struct.hpp"
#include "include/random.hpp"
#include "include/calcul.hpp"

#include "include/tree.hpp"

int getIndexTreeWood(Tree tree, int y, int z, int x){
    return tree.sizeZ_w * tree.sizeZ_w * y + tree.sizeZ_w * z + x;
}
int getIndexTreeLeaves(Tree tree, int y, int z, int x){
    return tree.sizeZ_l * tree.sizeZ_l * y + tree.sizeZ_l * z + x;
}

std::unique_ptr<Tree> generateTree(glm::vec3 pos){
    int chunk = getNumChunk(pos.x, pos.z, chunkWidth); // à vérifier 

    // TODO : Tree class more generic : add fir tree

    auto tree = std::make_unique<Tree>(3, 5, 5, 7); 
    tree->pos = pos;
    tree->chunk = chunk;

    int lengthTrunk = random2_1(glm::vec2(pos.x, pos.z), 3, 5); // ? oak
    tree->lengthTrunk = lengthTrunk;

    
    //Once the trunk ends, four blocks up in a diagonal direction can become branches
    glm::vec3 branchPos = glm::vec3(0,lengthTrunk+1,0);
    int midIndex = (tree->sizeZ_w)/2; // midIndex for wood, for leaves it's +1

    // ? oak
    // start of branch from trunk
    int branch_1[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
    for (auto i : branch_1) {
        int cx = i[0];
        int cz = i[1];

        branchPos.x = midIndex + cx;
        branchPos.z = midIndex + cz;

        if(randomBinary(pos + branchPos, 0.75)){
            tree->wood[getIndexTreeWood(*tree, 0, midIndex + cz, midIndex + cx)] = 1;
            
            if(randomBinary(pos + branchPos + glm::vec3(0, 1, 0), 0.4)){ // if branch goes up
                tree->wood[getIndexTreeWood(*tree, 1, midIndex + cz, midIndex + cx)] = 1;
            }
            if(randomBinary(pos + branchPos + glm::vec3(2 * cx, 1, 2 * cz), 0.2)){ // if branch goes up ; bis
                tree->wood[getIndexTreeWood(*tree, 1, midIndex + 2 * cz, midIndex + 2 * cx)] = 1;

                for(int yi = -1; yi <= 1; yi++){
                    for(int zi = -1; zi <= 1; zi++){
                        for(int xi = -1; xi <= 1; xi++){
                            if(randomBinary(pos + branchPos + glm::vec3(2 * cx + xi, 1 + yi, 2 * cz + zi), 0.05)){
                                tree->wood[getIndexTreeWood(*tree, 1 + yi, midIndex + 2 * cz + zi, midIndex + 2 * cx + xi)] = 1;
                            }
                        }
                    }
                }
            }
            if(randomBinary(pos + branchPos + glm::vec3(2 * cx, 0, 2 * cz), 0.2)){ // if branch stays on the same level
                tree->wood[getIndexTreeWood(*tree, 0, midIndex + 2 * cz, midIndex + 2 * cx)] = 1;
            }
        }
    }
    // Once branches are done, leaves are around each branch
    for(int iy = 0; iy < tree->sizeY_l; iy++){
        for(int iz = 0; iz < tree->sizeZ_l; iz++){
            for(int ix = 0; ix < tree->sizeZ_l; ix++){
                if(randomBinary(pos + glm::vec3(-(midIndex + 1) + ix, iy + lengthTrunk, -(midIndex + 1) + iz), 0.85)){ // purify
                    if( // if neighbor are branch
                        (tree->wood[getIndexTreeWood(*tree, iy - 1, iz - 1, ix - 2)] && ix > 1 && iz != 0 && iz != tree->sizeZ_l - 1 && iy != 0 && iy != tree->sizeY_l - 1) ||  // x-1
                        (tree->wood[getIndexTreeWood(*tree, iy - 1, iz - 1, ix)] && ix < tree->sizeZ_l - 2 && iz != 0 && iz != tree->sizeZ_l - 1 && iy != 0 && iy != tree->sizeY_l - 1) || // x+1
                        (tree->wood[getIndexTreeWood(*tree, iy - 1, iz - 2, ix - 1)] && iz > 1 && ix != 0 && ix != tree->sizeZ_l - 1 && iy != 0 && iy != tree->sizeY_l - 1) || // z-1
                        (tree->wood[getIndexTreeWood(*tree, iy - 1, iz, ix - 1)] && iz < tree->sizeZ_l - 2 && ix != 0 && ix != tree->sizeZ_l - 1 && iy != 0 && iy != tree->sizeY_l - 1) || // z+1  
                        (tree->wood[getIndexTreeWood(*tree, iy - 2, iz - 1, ix - 1)] && iy > 1 && ix != 0 && ix != tree->sizeZ_l - 1 && iz != 0 && iz != tree->sizeZ_l - 1) || // y-1
                        (tree->wood[getIndexTreeWood(*tree, iy, iz - 1, ix - 1)] && iy < tree->sizeY_l - 2 && ix != 0 && ix != tree->sizeZ_l - 1 && iz != 0 && iz != tree->sizeZ_l - 1)  // y+1
                    ){
                        tree->leaves[iy * tree->sizeZ_l * tree->sizeZ_l + iz * tree->sizeZ_l + ix] = 1;
                    }
                }
            }
        }
    }

    for(int iy = 0; iy < tree->sizeY_w; iy++){ // Remove leaves overlapping branches
        for(int iz = 0; iz < tree->sizeZ_w; iz++){
            for(int ix = 0; ix < tree->sizeZ_w; ix++){
                if(tree->wood[getIndexTreeWood(*tree, iy, iz, ix)]){
                    tree->leaves[getIndexTreeLeaves(*tree, iy + 1, iz + 1, ix + 1)] = 0;
                }
            }
        }
    }
    return tree;
}

float rateTree(int biome){
    if(biome==3 || biome==4 || biome==7){
        return 0.2;
    }
    return 0.05;
}

void setupTreeMap(
		Player &player,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
		std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		std::array<int, chunkLoadSize> &listIndex
){
    for(int i : chunkLoad){
        int chunkCoX, chunkCoZ;
        getCoordChunk(i, chunkCoX, chunkCoZ);
        for(int z=0; z<chunkWidth;z+=4){
            for(int x=0; x<chunkWidth;x+=4){
                int shiftX = (int)(2.5*z)%4;
                glm::vec2 pos2D = glm::vec2(chunkCoX*chunkWidth + x + shiftX, chunkCoZ*chunkWidth + z);
                int biome = chunkHeightMap[getIndexChunk(i, player.actualChunk, listIndex)][z*chunkWidth+x+shiftX]->biome;
                if(biome!=1 && biome!=6){
                    if( randomBinary2(pos2D, rateTree(biome)) ){
                        // * Trunk
                        glm::vec2 shift = glm::floor(random2(pos2D)+glm::vec2(1,1));
                        if( chunkHeightMap[getIndexChunk(i, player.actualChunk, listIndex)][(z+shift.y)*chunkWidth+x+shiftX+shift.x]->height > WATER_HEIGHT ){
                            Tree* tree = chunkTreeMap[getIndexChunk(i, player.actualChunk, listIndex)][(z+shift.y)*chunkWidth + x + shiftX + shift.x].get();
                            
                            // glm::vec3 pos = glm::vec3(pos2D.x + shift.x, chunkHeightMap[getIndexChunk(i, player.actualChunk, listIndex)][z*chunkWidth+x+shiftX].height, pos2D.y + shift.y);
                            glm::vec3 pos = tree->pos;
                            for(int il=0; il<tree->lengthTrunk; il++){
                                pos.y++;
                                player.worldMap.push_back(BlockInfo{pos * glm::vec3(SCALE), biome, 21}); // TODO : set texture according to biome
                            }

                            // * Branches
                            // ? oak
                            int sizeZ_w = tree->sizeZ_w;
                            for(int jy=0; jy<tree->sizeY_w;jy++){ 
                                for(int jz=0; jz<sizeZ_w; jz++){
                                    for(int jx=0; jx<sizeZ_w; jx++){ 
                                        if(tree->wood[getIndexTreeWood(*tree, jy, jz, jx)]){

                                            glm::vec3 posBranch = glm::vec3(jx-(int)sizeZ_w/2, jy+1, jz-(int)sizeZ_w/2); // relative from center tree
                                            int posWorldBranchX = pos.x+posBranch.x;
                                            int posWorldBranchZ = pos.z+posBranch.z;

                                            int posChunkBranchX = (posWorldBranchX >= 0 || posWorldBranchX%chunkWidth==0 ) ? posWorldBranchX : posWorldBranchX - chunkWidth;
                                            int posChunkBranchZ = (posWorldBranchZ >= 0 || posWorldBranchZ%chunkWidth==0 ) ? posWorldBranchZ : posWorldBranchZ - chunkWidth;
                                            int chunkBranch = getNumChunk(posChunkBranchX, posChunkBranchZ, chunkWidth); // if tree is in another chunk not loaded
                                            if( std::find(chunkLoad.begin(), chunkLoad.end(), chunkBranch) != chunkLoad.end() ){
                                                if(pos.y+posBranch.y > chunkHeightMap[getIndexChunk(chunkBranch, player.actualChunk, listIndex)][modFloat(pos.z+posBranch.z, chunkWidth)*chunkWidth+modFloat(pos.x+posBranch.x, chunkWidth)]->height){
                                                    glm::vec3 posWorld = glm::vec3(SCALE) * (pos + posBranch);
                                                    player.worldMap.push_back(BlockInfo{posWorld, biome, 21}); //  TODO : set texture according to biome 
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            // * Leaves 
                            int sizeZ_l = tree->sizeZ_l;
                            for(int jy=0; jy<tree->sizeY_l;jy++){ 
                                for(int jz=0; jz<sizeZ_l; jz++){
                                    for(int jx=0; jx<sizeZ_l; jx++){    
                                        if(tree->leaves[getIndexTreeLeaves(*tree, jy, jz, jx)]){
                                            
                                            glm::vec3 posLeaves = glm::vec3(jx-(int)sizeZ_l/2, jy, jz-(int)sizeZ_l/2); // relative from center tree
                                            
                                            int posWorldLeavesX = pos.x+posLeaves.x;
                                            int posWorldLeavesZ = pos.z+posLeaves.z;

                                            int posChunkLeavesX = (posWorldLeavesX >= 0 || posWorldLeavesX%chunkWidth==0 ) ? posWorldLeavesX : posWorldLeavesX - chunkWidth;
                                            int posChunkLeavesZ = (posWorldLeavesZ >= 0 || posWorldLeavesZ%chunkWidth==0 ) ? posWorldLeavesZ : posWorldLeavesZ - chunkWidth;
                                            
                                            int chunkLeaves = getNumChunk(posChunkLeavesX, posChunkLeavesZ, chunkWidth); // if tree is in another chunk
                                            if(std::find(chunkLoad.begin(), chunkLoad.end(), chunkLeaves) != chunkLoad.end()){
                                                if(pos.y+posLeaves.y > chunkHeightMap[getIndexChunk(chunkLeaves, player.actualChunk, listIndex)][modFloat(pos.z+posLeaves.z, chunkWidth)*chunkWidth+modFloat(pos.x+posLeaves.x, chunkWidth)]->height){
                                                    glm::vec3 posWorld = glm::vec3(SCALE) * (pos + posLeaves);
                                                    player.worldEnv.push_back(BlockInfo{posWorld, biome, 24}); // TODO : set texture according to biome
                                                } 
                                            }

                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}