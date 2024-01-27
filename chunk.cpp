#include "common/include.hpp"

#include <algorithm>

#include "include/constants.hpp"
#include "include/player.hpp"
#include "include/constants.hpp"
#include "include/generation.hpp"
#include "include/random.hpp"
#include "include/struct.hpp"
#include "include/tree.hpp"
#include "include/calcul.hpp"
#include "include/foliage.hpp"

#include "include/chunk.hpp"



/* +1 for all values, counter-clockwise spiral
***9*
*218*
*307*
*456*
*****
Layers system
22222
21112
21012
21112
22222
*/

/*
    +1 for all values, counter-clockwise spiral
    ___ ___ ___
    | . | . | 9 |
    |___|___|___|
    | 2 | 1 | 8 |
    |___|___|___|
    | 3 | 0 | 7 |
    |___|___|___|
    | 4 | 5 | 6 |
    |___|___|___|

    Layers system :
    ___ ___ ___ ___ ___
    | 2 | 2 | 2 | 2 | 2 |
    |___|___|___|___|___|
    | 2 | 1 | 1 | 1 | 2 |
    |___|___|___|___|___|
    | 2 | 1 | 0 | 1 | 2 |
    |___|___|___|___|___|
    | 2 | 1 | 1 | 1 | 2 |
    |___|___|___|___|___|
    | 2 | 2 | 2 | 2 | 2 |
    |___|___|___|___|___|
*/

// From chunkNumber to coordChunk 
void getCoordChunk(int nbChunk, int &chunkCoX, int &chunkCoY){
    // from index, to coord
    int x=0;
    int y=0;
    int iChunk=1; // index
    int nLvl = 0; // numbers of layers

    // First chunk of the last layer
    while (nbChunk > 1+4*( (nLvl)*(nLvl+1) )){
        iChunk+=8*nLvl;
        nLvl+=1;
    }
    if (nbChunk != 1){
        int diff = nbChunk-iChunk;
        // Coord of first block chunk
        x = -nLvl+1;
        y = nLvl;

        // -1 for x because we have a shift to the right (and abscissa is inverted (+)<----0-)
        if (2*nLvl > diff && diff >=0){ // Top
            x+=diff-1;
        }else if (4*nLvl > diff && diff >= 2*nLvl){ // Left
            x+=2*nLvl-1;
            y-=diff-2*nLvl;
        }else if (6*nLvl > diff && diff >= 4*nLvl){ // Bottom
            x+=2*nLvl-(diff-4*nLvl)-1;
            y-=2*nLvl;
        }else{ // Right
            x-=1;
            y-=2*nLvl-(diff-6*nLvl);
        }
    }
    chunkCoX = x;
    chunkCoY = y;

}

// From coordChunk to chunkNumber
int getNumChunk(int chunkCoX, int chunkCoY, int divide){
    chunkCoX /= divide;
    chunkCoY /= divide;
    int iChunk=1; // index
    int nLvl = std::max( abs(chunkCoX), abs(chunkCoY)); // number of layers

    // Donne le premier chunk de la couche + nLvl est la couche où nbChunk est
    if (nLvl!=0){
        for(int i=1;i<nLvl;i++){
            iChunk+=8*i;
        }
        // -1 for x because we have a shift to the right (and abscissa is inverted (+)<----0-)
        if (chunkCoX >= -nLvl+1 && chunkCoY==nLvl){ // Top
            iChunk+= chunkCoX+nLvl;
        }else if (chunkCoX==nLvl){ // Left
            iChunk+= 2*nLvl + nLvl-chunkCoY;
        }else if (chunkCoY==-nLvl){ // Bottom
            iChunk+= 4*nLvl + nLvl-chunkCoX;
        }else{ // Right
            iChunk+= 6*nLvl + chunkCoY + nLvl;
        }
    }
    return iChunk;
}

// Donne la position par rapport au centre de la grille de chunk
int getIndexChunk(int chunk, int actualChunk, std::array<int, chunkLoadSize> &listIndex){
    int chunkCoX, chunkCoY;
    getCoordChunk(chunk, chunkCoX, chunkCoY);
    
    int decayX, decayZ;
    getCoordChunk(actualChunk, decayX, decayZ);

    // Liste avec les chunks dans le sens x croissant et z croissant
    int numChunk = getNumChunk(chunkCoX-decayX, chunkCoY-decayZ, 1);

    return std::distance(listIndex.begin(),  std::find(listIndex.begin(), listIndex.end(), numChunk));
}


void updateChunkUpDown(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<int, 2> &borderChunkMin,
		Player player
	){
	int shiftRot = (shiftChunk==2*chunkView) ? 1 : 2*chunkView;
	// Rotate (n-1) fois, pareil que tourner 1 fois dans l'autre sens
	std::rotate(std::begin(chunkLoad), std::begin(chunkLoad) + shiftRot*(2*chunkView+1), std::end(chunkLoad));	
	std::rotate(std::begin(chunkHeightMap), std::begin(chunkHeightMap) + shiftRot*(2*chunkView+1), std::end(chunkHeightMap));	

	// Chunkload a maintenant comme dernière ligne tout ceux avec BorderMaxZ et qui parse les X
	int chunkCoZ = borderChunkMin[1] + shiftChunk;
	for(int x=-chunkView;x<=chunkView;x++){ // Update last n terms of chunkLoad and new blocks of chunkHeightMap
		int chunkCoX = borderChunkMin[0] + chunkView + x; // or : borderChunkMin[0] + chunkView + x
		chunkLoad[ shiftChunk*(2*chunkView+1) + x+chunkView  ] = getNumChunk(chunkCoX, chunkCoZ, 1); // last line and parse it
		// Update chunkHeightMap
		for(int jz=0; jz<chunkWidth;jz++){
			for(int jx=0; jx<chunkWidth;jx++){
				chunkHeightMap[ shiftChunk*(2*chunkView+1) + x+chunkView ][jz*chunkWidth+jx]->height_own = getHeightMap((float)chunkCoX*chunkWidth + jx,  (float)chunkCoZ*chunkWidth + jz, heightScale);
				chunkHeightMap[ shiftChunk*(2*chunkView+1) + x+chunkView ][jz*chunkWidth+jx]->biome = voronoiBiome( glm::vec2(chunkCoX*chunkWidth + jx,  chunkCoZ*chunkWidth + jz));
			}
		}
	}
	for(int xc=-chunkView;xc<=chunkView;xc++){
		int chunkCoX = borderChunkMin[0] + chunkView + xc; 
		setTempHeightMapHeight(chunkHeightMap, chunkCoX, chunkCoZ, chunkLoad, shiftChunk*(2*chunkView+1) + xc+chunkView);
	}
}

void updateChunkUpDown_Tree(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		Player player
	){
	int shiftRot = (shiftChunk==2*chunkView) ? 1 : 2*chunkView;
	std::rotate(std::begin(chunkTreeMap), std::begin(chunkTreeMap) + shiftRot*(2*chunkView+1), std::end(chunkTreeMap));	

	// Chunkload a maintenant comme dernière ligne tout ceux avec BorderMaxZ et qui parse les X
	int chunkCoZ = borderChunkMin[1] + shiftChunk;
	for(int xc=-chunkView;xc<=chunkView;xc++){  // pour chaque ligne
		int chunkCoX = borderChunkMin[0] + chunkView + xc; // or : borderChunkMin[0] + chunkView + x
		for(int z=0; z<chunkWidth;z+=4){
			for(int x=0; x<chunkWidth;x+=4){
				int shiftX = (int)(2.5*z)%4;
				glm::vec2 pos2D = glm::vec2(chunkCoX*chunkWidth + x + shiftX, chunkCoZ*chunkWidth + z);
				int biome = chunkHeightMap[shiftChunk*(2*chunkView+1) + xc+chunkView][z*chunkWidth+x+shiftX]->biome;
				if(biome!=1 && biome!=6){
					if( randomBinary2(pos2D, rateTree(biome)) ){
						glm::vec2 shift = glm::floor(random2(pos2D)+glm::vec2(1,1));
						int heightTree = chunkHeightMap[shiftChunk*(2*chunkView+1) + xc+chunkView][(z+shift.y)*chunkWidth+x+shiftX+shift.x]->height;
						if(heightTree > WATER_HEIGHT){
							glm::vec3 pos = glm::vec3(pos2D.x + shift.x, heightTree, pos2D.y + shift.y);
							chunkTreeMap[shiftChunk*(2*chunkView+1) + xc+chunkView][(z+shift.y)*chunkWidth+x+shiftX+shift.x] = std::move(generateTree(pos));
						}
					}
				}
			}
		}
	}
}

void updateChunkUpDown_Fol(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		Player player
	){
	int shiftRot = (shiftChunk==2*chunkView) ? 1 : 2*chunkView;
	std::rotate(std::begin(chunkFolMap), std::begin(chunkFolMap) + shiftRot*(2*chunkView+1), std::end(chunkFolMap));	

	// Chunkload a maintenant comme dernière ligne tout ceux avec BorderMaxZ et qui parse les X
	int chunkCoZ = borderChunkMin[1] + shiftChunk;
	for(int xc=-chunkView;xc<=chunkView;xc++){  // pour chaque ligne
		int chunkCoX = borderChunkMin[0] + chunkView + xc; // or : borderChunkMin[0] + chunkView + x
		for(int z=0; z<chunkWidth;z+=4){
			for(int x=0; x<chunkWidth;x+=4){
				int shiftX = (int)(2.5*z+2)%4;
				glm::vec2 pos2D = glm::vec2(chunkCoX*chunkWidth + x + shiftX, chunkCoZ*chunkWidth + z);
				int biome = chunkHeightMap[shiftChunk*(2*chunkView+1) + xc+chunkView][z*chunkWidth+x+shiftX]->biome;
				if(biome!=1){
					if( randomBinary2(pos2D, rateFol(biome)) ){
						glm::vec2 shift = glm::floor(random2(pos2D)+glm::vec2(1,1));
						int heightFol = chunkHeightMap[shiftChunk*(2*chunkView+1) + xc+chunkView][(z+shift.y)*chunkWidth+x+shiftX+shift.x]->height;
						if(heightFol > WATER_HEIGHT){
							glm::vec3 pos = glm::vec3(pos2D.x + shift.x, heightFol+1, pos2D.y + shift.y);

							std::unique_ptr<BlockInfo> &fol =chunkFolMap[shiftChunk*(2*chunkView+1) + xc+chunkView][(z+shift.y)*chunkWidth+x+shiftX+shift.x];
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



void updateChunkLeftRight(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<int, 2> &borderChunkMin,
		Player player
	){
		// TODO : use only indices and delete tempLoad
		std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, 2*chunkView+1> tempHeight;
		std::array<std::unique_ptr<BlockMap>, chunkArea> tempHeightMap;
		std::array<int, 2*chunkView+1> tempLoad;

		int chunkCoX = borderChunkMin[0]+shiftChunk;
		int shiftRot = (shiftChunk==2*chunkView) ? 1 : 2*chunkView;
		for(int i=0;i<(2*chunkView+1);i++){  // pour chaque ligne
			// chunkLoad

			for(int ii=0; ii<2*chunkView+1; ii++) { // copie la ligne initiale
				tempLoad[ii] = std::move(chunkLoad[i*(2*chunkView+1)+ii]);
			}
			std::rotate(tempLoad.begin(), tempLoad.begin()+shiftRot, tempLoad.end()); // rotate la ligne
			for(int ii=0; ii<2*chunkView+1; ii++) {  // copie la ligne à l'original
				chunkLoad[i*(2*chunkView+1)+ii] = std::move(tempLoad[ii]);
			}
			// chunkHeightMap 
			for(int ii=0; ii<2*chunkView+1; ii++){ // copie la ligne initiale
				tempHeight[ii] = std::move(chunkHeightMap[i*(2*chunkView+1)+ii]);
			}

			std::rotate(tempHeight.begin(), tempHeight.begin()+shiftRot, tempHeight.end()); // rotate la ligne

			int chunkCoZ = borderChunkMin[1] + i;
			chunkLoad[i*(2*chunkView+1) + shiftChunk ] = getNumChunk(chunkCoX, chunkCoZ, 1); // last term of line
			// Update chunkHeightMap
			for(int jz=0; jz<chunkWidth;jz++){
				for(int jx=0; jx<chunkWidth;jx++){
					std::unique_ptr<BlockMap> &block = tempHeightMap[jz*chunkWidth+jx];
                	block = std::make_unique<BlockMap>(); // Allocation dynamique
					block->height_own = getHeightMap((float)chunkCoX*chunkWidth + jx,  (float)chunkCoZ*chunkWidth + jz, heightScale);
					block->biome = voronoiBiome( glm::vec2(chunkCoX*chunkWidth + jx,  chunkCoZ*chunkWidth + jz));
				}
			}
			tempHeight[shiftChunk] = std::move(tempHeightMap);
			for(int ii=0; ii<2*chunkView+1; ii++) {
				chunkHeightMap[i*(2*chunkView+1)+ii] = std::move(tempHeight[ii]);
			}
		}
		for(int i=0;i<(2*chunkView+1);i++){  // pour chaque ligne
			int chunkCoZ = borderChunkMin[1] + i;
			setTempHeightMapHeight(chunkHeightMap, chunkCoX, chunkCoZ, chunkLoad, i*(2*chunkView+1)+shiftChunk);
		}
}

void updateChunkLeftRight_Tree(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		Player player
	){
		std::array<std::array<std::unique_ptr<Tree>, chunkArea>, 2*chunkView+1> tempTree;
		std::array<std::unique_ptr<Tree>, chunkArea> tempTreeMap;

		int chunkCoX = borderChunkMin[0]+shiftChunk;
		int shiftRot = (shiftChunk==2*chunkView) ? 1 : 2*chunkView;
		for(int i=0;i<(2*chunkView+1);i++){  // pour chaque ligne
			int chunkCoZ = borderChunkMin[1] + i;

			// chunkTreeMap
			for(int ii=0; ii<2*chunkView+1; ii++) {
				for(int ij=0; ij<chunkArea; ij++)
					tempTree[ii][ij] = std::move(chunkTreeMap[i*(2*chunkView+1)+ii][ij]);
			}

			std::rotate(tempTree.begin(), tempTree.begin()+shiftRot, tempTree.end()); // rotate la ligne

			for(int z=0; z<chunkWidth;z+=4){
				for(int x=0; x<chunkWidth;x+=4){
					int shiftX = (int)(2.5*z)%4;
					glm::vec2 pos2D = glm::vec2(chunkCoX*chunkWidth + x + shiftX, chunkCoZ*chunkWidth + z);
					int biome = chunkHeightMap[i*(2*chunkView+1)+shiftChunk][z*chunkWidth+x+shiftX]->biome;
					if(biome!=1 && biome!=6){
						if( randomBinary2(pos2D, rateTree(biome)) ){
							glm::vec2 shift = glm::floor(random2(pos2D)+glm::vec2(1,1));
							int heightTree = chunkHeightMap[i*(2*chunkView+1)+shiftChunk][(z+shift.y)*chunkWidth+x+shiftX+shift.x]->height;
							if(heightTree>WATER_HEIGHT){
								glm::vec3 pos = glm::vec3(pos2D.x + shift.x, heightTree, pos2D.y + shift.y);
								tempTreeMap[(z+shift.y)*chunkWidth+x+shiftX+shift.x] = std::move(generateTree(pos));
							}
						}
					}
				}
			} 
			tempTree[shiftChunk] = std::move(tempTreeMap);

			for(int ii=0; ii<2*chunkView+1; ii++) {
				for(int ij=0; ij<chunkArea; ij++)
					chunkTreeMap[i*(2*chunkView+1)+ii][ij] = std::move(tempTree[ii][ij]);
			}

		}
}

void updateChunkLeftRight_Fol(
		int shiftChunk,
        std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<int, 2> &borderChunkMin,
		Player player
	){
		std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, 2*chunkView+1> tempFol;
		std::array<std::unique_ptr<BlockInfo>, chunkArea> tempFolMap;

		int chunkCoX = borderChunkMin[0]+shiftChunk;
		int shiftRot = (shiftChunk==2*chunkView) ? 1 : 2*chunkView;
		for(int i=0;i<(2*chunkView+1);i++){  // pour chaque ligne
			int chunkCoZ = borderChunkMin[1] + i;

			// chunkFolMap
			for(int ii=0; ii<2*chunkView+1; ii++) {
				for(int ij=0; ij<chunkArea; ij++)
					tempFol[ii][ij] = std::move(chunkFolMap[i*(2*chunkView+1)+ii][ij]);
			}

			std::rotate(tempFol.begin(), tempFol.begin()+shiftRot, tempFol.end()); // rotate la ligne

			for(int z=0; z<chunkWidth;z+=4){
				for(int x=0; x<chunkWidth;x+=4){
					int shiftX = (int)(2.5*z+2)%4;
					glm::vec2 pos2D = glm::vec2(chunkCoX*chunkWidth + x + shiftX, chunkCoZ*chunkWidth + z);
					int biome = chunkHeightMap[i*(2*chunkView+1)+shiftChunk][z*chunkWidth+x+shiftX]->biome;
					if(biome!=1){
						if( randomBinary2(pos2D, rateFol(biome)) ){
							glm::vec2 shift = glm::floor(random2(pos2D)+glm::vec2(1,1));
							int heightFol = chunkHeightMap[i*(2*chunkView+1)+shiftChunk][(z+shift.y)*chunkWidth+x+shiftX+shift.x]->height;
							if(heightFol>WATER_HEIGHT){
								glm::vec3 pos = glm::vec3(pos2D.x + shift.x, heightFol+1, pos2D.y + shift.y);
								std::unique_ptr<BlockInfo> &fol = tempFolMap[(z+shift.y)*chunkWidth+x+shiftX+shift.x];
								fol = std::make_unique<BlockInfo>(); // Allocation dynamique
								fol->pos = pos;
								fol->biome = biome;
								fol->texture = (biome==6)? 30 : 27;
							
							}
						}
					}
				}
			} 
			tempFol[shiftChunk] = std::move(tempFolMap);

			for(int ii=0; ii<2*chunkView+1; ii++) {
				for(int ij=0; ij<chunkArea; ij++)
					chunkFolMap[i*(2*chunkView+1)+ii][ij] = std::move(tempFol[ii][ij]);
			}
		}
}

bool loadNewChunk(
        Player &player,
        std::array<int, chunkLoadSize> &chunkLoad,
        std::array<std::array<std::unique_ptr<BlockMap>, chunkArea>, chunkLoadSize> &chunkHeightMap,
        std::array<std::array<std::unique_ptr<Tree>, chunkArea>, chunkLoadSize> &chunkTreeMap,
        std::array<std::array<std::unique_ptr<BlockInfo>, chunkArea>, chunkLoadSize> &chunkFolMap,

        glm::vec3 &playerBeforePos,
        std::array<int, 2> &borderChunkMin
    ){
	int hasNewChunk = false;

	playerBeforePos /= SCALE;
	// if negative, int(-5/chunkWidth (=16)) = 0 so have to substract 1 
	int coordBefX =  (playerBeforePos.x < 0) ? int(playerBeforePos.x/chunkWidth) - 1 : int(playerBeforePos.x/chunkWidth);
	int coordBefZ =  (playerBeforePos.z < 0) ? int(playerBeforePos.z/chunkWidth) - 1 : int(playerBeforePos.z/chunkWidth);
	int coordActX =  (player.pos.x < 0) ? int(player.pos.x/SCALE /chunkWidth) - 1 : int(player.pos.x/SCALE /chunkWidth);
	int coordActZ = (player.pos.z < 0) ? int(player.pos.z/SCALE /chunkWidth) - 1 : int(player.pos.z/SCALE /chunkWidth);

	if(coordActZ > coordBefZ){ // Up
		hasNewChunk = true;
		borderChunkMin[1]++; // Update les coordonnées des chunks en bordures
		updateChunkUpDown(2*chunkView, chunkHeightMap, chunkLoad, borderChunkMin, player);
		updateChunkUpDown_Tree(2*chunkView, chunkTreeMap, chunkHeightMap, borderChunkMin, player);
		updateChunkUpDown_Fol(2*chunkView, chunkFolMap, chunkHeightMap, borderChunkMin, player);

	}else if(coordActZ < coordBefZ){ // Down
		hasNewChunk = true;
		borderChunkMin[1]--; // Update les coordonnées des chunks en bordures
		updateChunkUpDown(0, chunkHeightMap, chunkLoad, borderChunkMin, player);
		updateChunkUpDown_Tree(0, chunkTreeMap, chunkHeightMap, borderChunkMin, player);
		updateChunkUpDown_Fol(0, chunkFolMap, chunkHeightMap, borderChunkMin, player);

	}
	if(coordActX > coordBefX){ // Left
		hasNewChunk = true;
		borderChunkMin[0]++; // Update les coordonnées des chunks en bordures
		updateChunkLeftRight(2*chunkView, chunkHeightMap, chunkLoad, borderChunkMin, player);
		updateChunkLeftRight_Tree(2*chunkView, chunkTreeMap, chunkHeightMap, borderChunkMin, player);
		updateChunkLeftRight_Fol(2*chunkView, chunkFolMap, chunkHeightMap, borderChunkMin, player);

	}else if(coordActX < coordBefX){ // Right
		hasNewChunk = true;
		borderChunkMin[0]--; // Update les coordonnées des chunks en bordures
		updateChunkLeftRight(0, chunkHeightMap, chunkLoad, borderChunkMin, player);
		updateChunkLeftRight_Tree(0, chunkTreeMap, chunkHeightMap, borderChunkMin, player);
		updateChunkLeftRight_Fol(0, chunkFolMap, chunkHeightMap, borderChunkMin, player);
	}

	return hasNewChunk;

}