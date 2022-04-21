#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
using std::unordered_map;
using std::string;

class TextureManager
{
    // static means only one of these in memory, ever
    static unordered_map<string, sf::Texture> textures;

    // LoadTexture is private because GetTexture calls LoadTexture
    // LoadTexture never needs to be called explicitly
    static void LoadTexture(string textureName);
public:
    static sf::Texture& GetTexture(string textureName);
    static void Clear(); // essentially destructor


};