#ifndef LOADBMP_HPP
#define LOADBMP_HPP

#include<iostream>
#include "include.hpp"


GLuint loadBMP_custom(const char * imagepath){
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    unsigned char * data;

    FILE * file = fopen(imagepath,"rb");
    if (!file){
        return 0;
    }
    if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
        return false;
    }
    if ( header[0]!='B' || header[1]!='M' ){
        return 0;
    }
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);

    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way

    data = new unsigned char [imageSize];
    fread(data,1,imageSize,file);

    fclose(file);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    return textureID;

}

#endif