#include <sky.h>
#include <iostream> 
#include <ghoul/opengl/programobject.h>
#include <camera.h>
#include <glm/gtx/transform.hpp>
#include <binarystream.h>
#include <textureset.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif
namespace earthrendering {

Sky::Sky(int nSegments)
    : _sphere(nSegments)
    , _starsTextureSet(1, glm::uvec2(4, 2), "${TEXTURES}/stars/${i}.png")
{
    
}

Sky::~Sky() {
    
}


bool Sky::initialize() {
    _program = ghoul::opengl::ProgramObject::Build(
        "Sky Program",
        "${SHADERS}/sky.vert",
        "${SHADERS}/sky.frag");

    _sphere.initialize();

    loadTextures();
    
    std::cout << "Initialized sky!" << std::endl;

    return true;
}

    
void Sky::render(Camera& camera) {
    if (_programIsDirty || _program->isDirty()) {
		_program->rebuildFromFile();
        _programIsDirty = false;
	}

    GLint unit = 0;
    auto bindTextureSet = [this, &unit](TextureSet& ts, const std::string& uniformArray, const std::string& uniformNTiles) {
        glm::uvec2 n = ts.nTiles(0);
        std::vector<GLint> unitIndices;
        for (int j = 0; j < n.y; j++) {
            for (int i = 0; i < n.x; i++) {
                Texture* t = ts.getTexture(0, glm::uvec2(i, j));
                GLint glEnum = GL_TEXTURE0 + unit;
                glActiveTexture(glEnum);
                t->bind();
                unitIndices.push_back(unit);
                unit++;
            }
        }
        _program->setUniform(uniformArray, unitIndices.data(), unitIndices.size());
        _program->setUniform(uniformNTiles, n);
    };

    bindTextureSet(_starsTextureSet, "starsTextures", "nStarsTiles");
    
    glm::vec3 cameraPosition = camera.getSgctPosition();
    glm::vec3 rawCameraPosition = camera.getPosition();

    glm::mat4 modelMatrix = glm::scale(glm::vec3(1.0, 1.0, 1.0));


    /// render sky
    
    _program->activate();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
     
    //_program->setUniform("cameraPosition", cameraPosition);
    _program->setUniform("viewProjection", camera.getViewProjectionMatrix());
    _program->setUniform("model", modelMatrix);
    
    _sphere.render();

    
    _program->deactivate();


}

    
void Sky::invalidateProgram() {
    _programIsDirty = true;
}

    
std::vector<char> Sky::serialize() {
    BinaryStream bs;
    return bs.data();
}

    
void Sky::unserialize(std::vector<char> data) {
//    BinaryStream bs(data);
}


void Sky::loadTextures() {
    _starsTextureSet.loadLevel(0);
}

}
