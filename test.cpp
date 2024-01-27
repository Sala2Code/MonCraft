// make a simple file 

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>

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

int main(){
    std::cout << getNumChunk(-12,-4,4) << " ";
    std::cout << getNumChunk(-13,-4,4) << " ";
    std::cout << getNumChunk(-12,-5,4) << " ";
    std::cout << getNumChunk(-11,-4,4) << " ";
    std::cout << getNumChunk(-12,-3,4) << " ";





    return 0;
}
