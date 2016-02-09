#pragma once

#include <sgct.h>
#include <ghoul/opengl/texture.h>

namespace earthrendering {

using namespace ghoul::opengl;
    
class TextureSet {
public:
    TextureSet(unsigned int nLevels, glm::uvec2 nTilesRoot, std::string templatePath);
    bool hasTexture(unsigned int level, glm::uvec2 xy);
    Texture* loadTexture(unsigned int level, glm::uvec2 xy);
    void loadLevel(unsigned int level);
    void releaseTexture(unsigned int level, glm::uvec2 xy);
    Texture* getTexture(unsigned int level, glm::uvec2 xy);
    bool isValid(unsigned int level, glm::uvec2 xy);
    glm::uvec2 nTiles(unsigned int level);
    glm::uvec2 nTilesHighestLevel();
    typedef std::map<unsigned int, Texture*> Level;

private:

    std::string resolvePath(unsigned int level, glm::uvec2 xy);
    
    std::vector<Level> _levels;
    glm::uvec2 _nTilesRoot;
    unsigned int linearTileCoordinates(unsigned int level, glm::uvec2 xy);
    /*
     * Template path.
     * for example: disk:/foo/${level}/image_${i}.png
     * available variables:
     * ${x} the x coordinate
     * ${y} the y coordinate
     * ${i} the linear coordinate y * w + x
     * ${level} the level
     */
    std::string _templatePath;
    
};
    
}
