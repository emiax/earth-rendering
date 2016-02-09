#include <textureset.h>
#include <ghoul/io/texture/texturereader.h>
#include <ghoul/filesystem/filesystem.h>
#include <iostream>

namespace {
    const std::string xString = "${x}";
    const std::string yString = "${y}";
    const std::string iString = "${i}";
    const std::string levelString = "${level}";
}

namespace earthrendering {

using namespace ghoul::opengl;
using namespace ghoul::io;

    
TextureSet::TextureSet(unsigned int nLevels, glm::uvec2 nTilesRoot, std::string templatePath)
    : _levels(std::vector<Level>(nLevels))
    , _nTilesRoot(nTilesRoot)
    , _templatePath(templatePath) {}

bool TextureSet::hasTexture(unsigned int level, glm::uvec2 xy) {
    
    if (!isValid(level, xy)) return false;

    return _levels[level].find(linearTileCoordinates(level, xy)) != _levels[level].end();
    return nullptr;
}

void TextureSet::loadLevel(unsigned int level) {
    glm::uvec2 n = nTiles(level);
    for (int j = 0; j < n.y; j++) {
        for (int i = 0; i < n.x; i++) {
            loadTexture(level, glm::uvec2(i, j));
        }
    }
}
    
Texture* TextureSet::loadTexture(unsigned int level, glm::uvec2 xy) {
    if (!isValid(level, xy)) return nullptr;
    
    std::string path = resolvePath(level, xy);
    TextureReader& reader = ghoul::io::TextureReader::ref();
    Texture* texture = reader.loadTexture(absPath(path));

    if (!texture) {
        std::cout << "Failed to load texture from path " << path << std::endl;
        std::cin.get();
        return nullptr;
    }

    texture->uploadTexture();
    texture->setWrapping(Texture::WrappingMode::ClampToEdge);
    texture->setFilter(Texture::FilterMode::AnisotropicMipMap);

    _levels[level][linearTileCoordinates(level, xy)] = texture;

    std::cout << "Loaded texture from " << path << std::endl;
    
    return texture;
}

void TextureSet::releaseTexture(unsigned int level, glm::uvec2 xy) {
    if (!hasTexture(level, xy)) return;

    unsigned int i = linearTileCoordinates(level, xy);
    delete _levels[level][i];
    _levels[level].erase(i);
}

Texture* TextureSet::getTexture(unsigned int level, glm::uvec2 xy) {
    if (hasTexture(level, xy)) {
        return _levels[level][linearTileCoordinates(level, xy)];
    } else {
        return loadTexture(level, xy);
    }
    return nullptr;
}

glm::uvec2 TextureSet::nTilesHighestLevel() {
    return nTiles(_levels.size() - 1);
}
    
glm::uvec2 TextureSet::nTiles(unsigned int level) {
    // no tiles at all if the level does not exist
    if (level >= _levels.size()) return glm::uvec2(0, 0);
    // otherwise, the tiles per dimension doubles for each level.
    return _nTilesRoot * static_cast<unsigned int>(glm::pow(2, level));
}
    
bool TextureSet::isValid(unsigned int level, glm::uvec2 xy = glm::uvec2(0, 0)) {
    glm::uvec2 n = nTiles(level);
    return xy.x < n.x && xy.y < n.y;
}

std::string TextureSet::resolvePath(unsigned int level, glm::uvec2 xy) {
    std::stringstream ss;
    ss << xy.x;
    std::string x = ss.str();

    ss.str(std::string());
    ss << xy.y;
    std::string y = ss.str();

    ss.str(std::string());

    ss << linearTileCoordinates(level, xy); 
    std::string i = ss.str();

    ss.str(std::string());
    ss << level;
    std::string l = ss.str();

    std::string s = _templatePath;

    size_t xPos = s.find(xString);
    if (xPos != std::string::npos) {
        s.replace(xPos, xString.size(), x);
    }
    size_t yPos = s.find(yString);
    if (yPos != std::string::npos) {
        s.replace(yPos, yString.size(), y);
    }
    size_t iPos = s.find(iString);
    if (iPos != std::string::npos) {
        s.replace(iPos, iString.size(), i);
    }
    size_t levelPos = s.find(levelString);
    if (levelPos != std::string::npos) {
        s.replace(levelPos, levelString.size(), l);
    }

    return s;
}

unsigned int TextureSet::linearTileCoordinates(unsigned int level, glm::uvec2 xy) {
    glm::uvec2 nTilesInLevel = nTiles(level);
    return nTilesInLevel.x * xy.y + xy.x;
}
    
}
