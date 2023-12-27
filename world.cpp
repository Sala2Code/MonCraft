// #include <array>
#include <algorithm>
#include "common/include.hpp"
#include "include/player.hpp"
#include "include/constants.hpp"
#include "include/generation.hpp"
#include "include/struct.hpp"
#include "include/initObjects.hpp"
#include "include/world.hpp"


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
        }
        else if (4*nLvl > diff && diff >= 2*nLvl){ // Left
            x+=2*nLvl-1;
            y-=diff-2*nLvl;
        }
        else if (6*nLvl > diff && diff >= 4*nLvl){ // Bottom
            x+=2*nLvl-(diff-4*nLvl)-1;
            y-=2*nLvl;
        }
        else{ // Right
            x-=1;
            y-=2*nLvl-(diff-6*nLvl);
        }
    }
    chunkCoX = x;
    chunkCoY = y;

}

// From coordChunk to chunkNumber
int getNumChunk(int chunkCoX, int chunkCoY, int divide){
    chunkCoX /=divide;
    chunkCoY /=divide;
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
        }
        else if (chunkCoX==nLvl){ // Left
            iChunk+= 2*nLvl + nLvl-chunkCoY;
        }
        else if (chunkCoY==-nLvl){ // Bottom
            iChunk+= 4*nLvl + nLvl-chunkCoX;
        }
        else{ // Right
            iChunk+= 6*nLvl + chunkCoY + nLvl;
        }
    }
    return iChunk;
}

int getIndexChunk(int chunk, int &actualChunk, std::array<int, chunkLoadSize> &listIndex){
    // Donne la position par rapport au centre de la grille de chunk
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
        std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> &chunkHeightMap,
        std::array<BlockMap, chunkArea> &tempHeightMap,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<int, 2> &borderChunkMin,
		int coordActX
	){
	// Chunkload a maintenant comme dernière ligne tout ceux avec BorderMaxZ et qui parse les X
	int chunkCoZ = borderChunkMin[1] + shiftChunk;
	for(int x=-chunkView;x<=chunkView;x++){ // Update last n terms of chunkLoad and new blocks of chunkHeightMap
		int chunkCoX = coordActX + x; // or : borderChunkMin[0] + chunkView + x
		chunkLoad[ shiftChunk*(2*chunkView+1) + x+chunkView  ] = getNumChunk(chunkCoX, chunkCoZ, 1); // last line and parse it
		// Update chunkHeightMap
		for(int jz=0;jz<chunkWidth;jz++){
			for(int jx=0;jx<chunkWidth;jx++){
				tempHeightMap[jz*chunkWidth+jx].height = perlin2DheightMap((float)chunkCoX*chunkWidth + jx,  (float)chunkCoZ*chunkWidth + jz, heightMap);
				tempHeightMap[jz*chunkWidth+jx].biome = cellularBiome((float)chunkCoX*chunkWidth + jx,  (float)chunkCoZ*chunkWidth + jz);

			}
		}
		chunkHeightMap[ shiftChunk*(2*chunkView+1) + x+chunkView ] = tempHeightMap;
	}
}


void loadNewChunk(
        Player &player,
        std::array<int, chunkLoadSize> &chunkLoad,
        std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> &chunkHeightMap,
        std::array<BlockMap, chunkArea> &tempHeightMap,
        std::array<std::array<BlockMap, chunkArea>, 2*chunkView+1> &tempHeight,
        std::array<int, 2*chunkView+1> &tempLoad,
        glm::vec3 &playerBeforePos,
        std::array<int, 2> &borderChunkMin
    ){

	playerBeforePos /= SCALE;
	// if negative, int(-5/chunkWidth (=16)) = 0 so have to substract 1 
	int coordBefX =  (playerBeforePos.x < 0) ? int(playerBeforePos.x/chunkWidth) - 1 : int(playerBeforePos.x/chunkWidth);
	int coordBefZ =  (playerBeforePos.z < 0) ? int(playerBeforePos.z/chunkWidth) - 1 : int(playerBeforePos.z/chunkWidth);
	int coordActX =  (player.pos.x < 0) ? int(player.pos.x/SCALE /chunkWidth) - 1 : int(player.pos.x/SCALE /chunkWidth);
	int coordActZ = (player.pos.z < 0) ? int(player.pos.z/SCALE /chunkWidth) - 1 : int(player.pos.z/SCALE /chunkWidth);

	// ! Create function to nest
	if(coordActZ > coordBefZ){ // Up
		borderChunkMin[1]++; // Update les coordonnées des chunks en bordures
		std::rotate(std::begin(chunkLoad), std::begin(chunkLoad) + 2*chunkView+1, std::end(chunkLoad));	
		std::rotate(std::begin(chunkHeightMap), std::begin(chunkHeightMap) + 2*chunkView+1, std::end(chunkHeightMap));	

		// Chunkload a maintenant comme dernière ligne tout ceux avec BorderMaxZ et qui parse les X
		updateChunkUpDown(2*chunkView, chunkHeightMap, tempHeightMap, chunkLoad, borderChunkMin, coordActX);
	
	}else if(coordActZ < coordBefZ){ // Down
		borderChunkMin[1]--; // Update les coordonnées des chunks en bordures
		// Rotate (n-1) fois, pareil que tourner 1 fois dans l'autre sens
		std::rotate(std::begin(chunkLoad), std::begin(chunkLoad) + (2*chunkView)*(2*chunkView+1), std::end(chunkLoad));	
		std::rotate(std::begin(chunkHeightMap), std::begin(chunkHeightMap) + (2*chunkView)*(2*chunkView+1), std::end(chunkHeightMap));	

		updateChunkUpDown(0, chunkHeightMap, tempHeightMap, chunkLoad, borderChunkMin, coordActX);

	}

	if(coordActX > coordBefX){ // Left
		borderChunkMin[0]++; // Update les coordonnées des chunks en bordures

		for(int i=0;i<chunkLoad.size()/(2*chunkView+1);i++){  // pour chaque ligne
			// chunkLoad
			std::copy(chunkLoad.begin()+i*(2*chunkView+1), chunkLoad.begin()+(i+1)*(2*chunkView+1), tempLoad.begin()); // copie la ligne initiale
			std::rotate(tempLoad.begin(), tempLoad.begin()+1, tempLoad.end()); // rotate la ligne
			std::copy(tempLoad.begin(), tempLoad.end(), chunkLoad.begin()+i*(2*chunkView+1)); // copie la ligne à l'original

			// chunkHeightMap 
			std::copy(chunkHeightMap.begin()+i*(2*chunkView+1), chunkHeightMap.begin()+(i+1)*(2*chunkView+1), tempHeight.begin()); // copie la ligne initiale
			std::rotate(tempHeight.begin(), tempHeight.begin()+1, tempHeight.end()); // rotate la ligne

			int chunkCoX = borderChunkMin[0]+2*chunkView;
			int chunkCoZ = borderChunkMin[1] + i;
			chunkLoad[i*(2*chunkView+1) +2*chunkView ] = getNumChunk(chunkCoX, chunkCoZ, 1); // last term of line
			// std::cout << getNumChunk(chunkCoX, chunkCoZ, 1) << " " << i*(2*chunkView+1) +2*chunkView << " " << chunkCoX << " " << chunkCoZ << std::endl;
			// Update chunkHeightMap
			for(int jz=0;jz<chunkWidth;jz++){
				for(int jx=0;jx<chunkWidth;jx++){
					tempHeightMap[jz*chunkWidth+jx].height = perlin2DheightMap((float)chunkCoX*chunkWidth + jx,  (float)chunkCoZ*chunkWidth + jz, heightMap);
					tempHeightMap[jz*chunkWidth+jx].biome = cellularBiome((float)chunkCoX*chunkWidth + jx,  (float)chunkCoZ*chunkWidth + jz);

				}
			}
			tempHeight[ 2*chunkView ] = tempHeightMap;
			std::copy(tempHeight.begin(), tempHeight.end(), chunkHeightMap.begin()+i*(2*chunkView+1)); // on applique à l'original
		
		}
	}else if(coordActX < coordBefX){ // Right
		borderChunkMin[0]--; // Update les coordonnées des chunks en bordures

		for(int i=0;i<chunkLoad.size()/(2*chunkView+1);i++){  // pour chaque ligne
			// chunkLoad
			std::copy(chunkLoad.begin()+i*(2*chunkView+1), chunkLoad.begin()+(i+1)*(2*chunkView+1), tempLoad.begin()); // copie la ligne initiale
			std::rotate(tempLoad.begin(), tempLoad.begin()+2*chunkView, tempLoad.end()); // rotate la ligne
			std::copy(tempLoad.begin(), tempLoad.end(), chunkLoad.begin()+i*(2*chunkView+1)); // copie la ligne à l'original

			// chunkHeightMap 
			std::copy(chunkHeightMap.begin()+i*(2*chunkView+1), chunkHeightMap.begin()+(i+1)*(2*chunkView+1), tempHeight.begin()); // copie la ligne initiale
			std::rotate(tempHeight.begin(), tempHeight.begin()+2*chunkView, tempHeight.end()); // rotate la ligne

			int chunkCoX = borderChunkMin[0];
			int chunkCoZ = borderChunkMin[1] + i;
			chunkLoad[i*(2*chunkView+1)] = getNumChunk(chunkCoX, chunkCoZ, 1); // last term of line
			// std::cout << getNumChunk(chunkCoX, chunkCoZ, 1) << " " << i*(2*chunkView+1) +2*chunkView << " " << chunkCoX << " " << chunkCoZ << std::endl;
			// Update chunkHeightMap
			for(int jz=0;jz<chunkWidth;jz++){
				for(int jx=0;jx<chunkWidth;jx++){
					tempHeightMap[jz*chunkWidth+jx].height = perlin2DheightMap((float)chunkCoX*chunkWidth + jx,  (float)chunkCoZ*chunkWidth + jz, heightMap);
					tempHeightMap[jz*chunkWidth+jx].biome = cellularBiome((float)chunkCoX*chunkWidth + jx,  (float)chunkCoZ*chunkWidth + jz);
				}
			}
			tempHeight[0] = tempHeightMap;
			std::copy(tempHeight.begin(), tempHeight.end(), chunkHeightMap.begin()+i*(2*chunkView+1)); // on applique à l'original
		
		}
	}

}

bool isBlockInView(float playerX, float playerZ, float chunkX, float chunkZ, float jx, float jz, float playerAngleView, float tolView, bool thirdView){
	chunkX  *= SCALE;
	chunkZ  *= SCALE;
	jx *= SCALE;
	jz *= SCALE;

    float viewDirX = (!thirdView) ? std::cos(playerAngleView+1.57)  : std::cos(playerAngleView-1.57);
    float viewDirZ = (!thirdView) ?  std::sin(playerAngleView-1.57) : -std::sin(playerAngleView-1.57);
	// normalize
	float viewDirLength = std::sqrt( viewDirX*viewDirX + viewDirZ*viewDirZ );
	viewDirX /= viewDirLength;
	viewDirZ /= viewDirLength;
	// tolView is to see behind you, for instance when you look the floor
    float chunkPointX = (chunkX * chunkWidth + jx) - playerX - tolView*viewDirX;
    float chunkPointZ = (chunkZ * chunkWidth + jz) - playerZ - tolView*viewDirZ;

    float dotProduct = chunkPointX * viewDirX + chunkPointZ * viewDirZ;
    if (
		(thirdView && dotProduct > -chunkView*chunkWidth && dotProduct < tolView)
		||
		(!thirdView && dotProduct > -chunkView*chunkWidth && dotProduct < tolView)
	){
        return true;
    } 
    return false;
}


void displayMap(
		Player &player,
		std::array<int, chunkLoadSize> &chunkLoad,
		std::array<std::array<BlockMap, chunkArea>, chunkLoadSize> &chunkHeightMap,
		std::array<BlockMap, chunkArea> &tempHeightMap,
		std::array<std::array<BlockMap, chunkArea>, 2*chunkView+1> &tempHeight,
		std::array<int, 2*chunkView+1> &tempLoad,
		std::array<int, 2> &borderChunkMin,
		BlockShader &blockShader,
		std::array<int, chunkLoadSize> listIndex,
		std::vector<BlockInfo> &worldMap
){

	
	for(int i : chunkLoad){
		int chunkX, chunkZ;
		getCoordChunk(i, chunkX, chunkZ);
		unsigned int indexChunk = getIndexChunk(i, player.actualChunk, listIndex);

		for(int jz=0;jz<chunkWidth;jz++){
			for(int jx=0;jx<chunkWidth;jx++){
				// AngleView is turn by pi/2 so we got an ordinary coordinate system : only for(equa/Img)Droite
				// compare line and block to know if it's on or underneath. Then, display it if the player look in this direction
				if(isBlockInView(player.pos.x, player.pos.z, chunkX, chunkZ, jx, jz, player.angleView, tolView, player.thirdView)){
					int maxHeight = chunkHeightMap[indexChunk][jz*chunkWidth+jx].height; // Surface

					// Get the height of blocks next to the current block
					// Double ternary to check border and corner cases
					int heightLeft = (jx != chunkWidth-1) ? chunkHeightMap[indexChunk][jz*chunkWidth+(jx+1)].height : (chunkX!=borderChunkMin[0]+2*chunkView) ? chunkHeightMap[getIndexChunk( getNumChunk(chunkX+1, chunkZ, 1), player.actualChunk, listIndex)][jz*chunkWidth].height : 0;
					int heightRight = (jx != 0) ? chunkHeightMap[indexChunk][jz*chunkWidth+(jx-1)].height : (chunkX!=borderChunkMin[0]) ? chunkHeightMap[getIndexChunk( getNumChunk(chunkX-1, chunkZ, 1), player.actualChunk, listIndex)][jz*chunkWidth+(chunkWidth-1)].height : 0;
					int heightBehind = (jz != 0) ? chunkHeightMap[indexChunk][(jz-1)*chunkWidth+jx].height : (chunkZ!=borderChunkMin[1])  ? chunkHeightMap[getIndexChunk( getNumChunk(chunkX, chunkZ-1, 1), player.actualChunk, listIndex)][(chunkWidth-1)*chunkWidth+jx].height : 0;
					int heightFront =  (jz != chunkWidth-1) ? chunkHeightMap[indexChunk][(jz+1)*chunkWidth+jx].height  : (chunkZ!=borderChunkMin[1]+2*chunkView)  ? chunkHeightMap[getIndexChunk( getNumChunk(chunkX, chunkZ+1, 1), player.actualChunk, listIndex)][jx].height : 0;

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
								GLuint shaderProgram = blockShader.shaderBasicInstanced;
								glm::vec3 cubePosition{chunkX*chunkWidth+jx+0.5, k+0.5, chunkZ*chunkWidth+jz+0.5}; // +0.5 because the origin is at center
								cubePosition *= glm::vec3(SCALE);
								worldMap.push_back(BlockInfo{cubePosition, chunkHeightMap[indexChunk][jz*chunkWidth+jx].biome, shaderProgram, blockShader.Texture});

						}else{
							break;
						}
					}

					if(maxHeight < WATER_HEIGHT){
						GLuint shaderProgram = blockShader.shaderWater;
						glm::vec3 cubePosition{chunkX*chunkWidth+jx+0.5, WATER_HEIGHT-0.5, chunkZ*chunkWidth+jz+0.5}; // +0.5 because the origin is at center
						cubePosition *= glm::vec3(SCALE);
						worldMap.push_back(BlockInfo{cubePosition, chunkHeightMap[indexChunk][jz*chunkWidth+jx].biome, shaderProgram, blockShader.Texture});
					}
				}
			}
		}
	}
}
