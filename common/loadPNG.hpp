#include <iostream>
#include "lodepng.hpp"

#include "include.hpp"

GLuint loadPNG_LodePNG(const char* imagepath) {
    std::vector<unsigned char> image; // raw pixels
    unsigned width, height;

    // Decode
    unsigned error = lodepng::decode(image, width, height, imagepath);

    // If there's an error, display it.
    if(error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return 0;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture: all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    return textureID;
}