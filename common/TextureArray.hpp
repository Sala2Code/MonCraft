#ifndef TEXTUREARRAY_HPP
#define TEXTUREARRAY_HPP


class TextureArray
{
public:
    GLuint textureID;

    TextureArray();

    void AddTextureToArray(const GLchar *filePath);
    void ActivateShaderArray(GLuint shaderProgramID);
    void Bind();
    void Unbind();
    void Delete();

private:
    glm::vec2 image_size = glm::vec2(16, 16);
    GLchar textureArrayName[15] = "arrTexture";
    GLuint textureUnitID;
    GLuint image_index = 0;
};

// Increments for each texture array instance that is created. This keeps track of
// which texture slots we have activated so far
inline GLuint textureSlotIndex = 0;

#endif