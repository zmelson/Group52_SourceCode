#include "TextureManager.h"

// "redeclare" so the file knows it exists
unordered_map<string, sf::Texture> TextureManager::textures;

void TextureManager::LoadTexture(string fileName)
{
    // turns name of texture into the path that the texture is located
    // ASSUMPTIONS: filename of texture is "images" and texture is of type "png"
    string path = "../images/";
    path += fileName + ".png";

    textures[fileName].loadFromFile(path);
}

sf::Texture& TextureManager::GetTexture(string textureName)
{
    // if texture doesn't exist yet
    if (textures.find(textureName) == textures.end())
        LoadTexture(textureName);

    return textures[textureName];
}


void TextureManager::Clear()
{
    textures.clear();
}