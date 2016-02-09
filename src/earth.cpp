#include <earth.h>
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

Earth::Earth(int nSegments)
    : _sphere(nSegments)
    , _time(0)
    , _dayTextureSet(1, glm::uvec2(4, 2), "${TEXTURES}/lowres/${i}.png")
    , _nightTextureSet(1, glm::uvec2(4, 2), "${TEXTURES}/lights/${i}.png")
    , _glowTextureSet(1, glm::uvec2(4, 2), "${TEXTURES}/glow/${i}.png")
    , _cloudTextureSet(1, glm::uvec2(2, 1), "${TEXTURES}/clouds/${i}.png")
    , _waterTextureSet(1, glm::uvec2(1, 1), "${TEXTURES}/water_4k.png")
{
}

Earth::~Earth() {
}

bool Earth::initialize() {
    _groundProgram = ghoul::opengl::ProgramObject::Build(
        "Earth Ground Program",
        "${SHADERS}/ground.vert",
        "${SHADERS}/ground.frag");

    _atmosphereProgram = ghoul::opengl::ProgramObject::Build(
        "Earth Atmosphere Program",
        "${SHADERS}/atmosphere.vert",
        "${SHADERS}/atmosphere.frag");

    _sphere.initialize();

    loadTextures();
    
    std::cout << "Initialized earth!" << std::endl;

    return true;
}

void Earth::setSunPosition(glm::vec3 sunPosition) {
    _sunPosition = sunPosition;
}
    
void Earth::render(Camera& camera) {
    if (_groundProgramIsDirty || _groundProgram->isDirty()) {
		_groundProgram->rebuildFromFile();
        _groundProgramIsDirty = false;
	}

    if (_atmosphereProgramIsDirty || _atmosphereProgram->isDirty()) {
		_atmosphereProgram->rebuildFromFile();
        _atmosphereProgramIsDirty = false;
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
        _groundProgram->setUniform(uniformArray, unitIndices.data(), unitIndices.size());
        _groundProgram->setUniform(uniformNTiles, n);
    };

    bindTextureSet(_dayTextureSet, "dayTextures", "nDayTiles");
    bindTextureSet(_nightTextureSet, "nightTextures", "nNightTiles");
    bindTextureSet(_glowTextureSet, "glowTextures", "nGlowTiles");
    bindTextureSet(_cloudTextureSet, "cloudTextures", "nCloudTiles");
    bindTextureSet(_waterTextureSet, "waterTextures", "nWaterTiles");
    
    glm::vec3 cameraPosition = camera.getSgctPosition();
    glm::vec3 rawCameraPosition = camera.getPosition();

    glm::mat4 modelMatrix = glm::scale(glm::vec3(1.0, 1.0, 1.0));
    modelMatrix = glm::rotate(modelMatrix, static_cast<float>(_rotation), glm::vec3(0.0, 1.0, 0.0));
    //_rotation += 0.001;

    // Render atmosphere

    _atmosphereProgram->activate();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE); // Additive blending

    _atmosphereProgram->setUniform("cameraPosition", cameraPosition);
    _atmosphereProgram->setUniform("sunPosition", _sunPosition);
    _atmosphereProgram->setUniform("viewProjection", camera.getViewProjectionMatrix());
    _atmosphereProgram->setUniform("model", modelMatrix);
    setAtmosphericUniforms(_atmosphereProgram);
    
    _sphere.render();


    
    _atmosphereProgram->deactivate();
    

    /// render ground
    
    _groundProgram->activate();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
     
    _groundProgram->setUniform("cameraPosition", cameraPosition);
    _groundProgram->setUniform("sunPosition", _sunPosition);
    _groundProgram->setUniform("viewProjection", camera.getViewProjectionMatrix());
    _groundProgram->setUniform("model", modelMatrix);

    _groundProgram->setUniform("time", static_cast<float>(_time++));
    setAtmosphericUniforms(_groundProgram);
    
    _sphere.render();
    
    _groundProgram->deactivate();


}


void Earth::setAtmosphericUniforms(ghoul::opengl::ProgramObject* program) {
    glm::vec3 wavelength = glm::vec3(0.650f, 0.570, 0.475);
    // 650 nm for red
    // 570 nm for green
    // 475 nm for blue
    glm::vec3 invWavelength = glm::vec3(1.0/std::pow(wavelength[0], 4.0),
                                        1.0/std::pow(wavelength[1], 4.0),
                                        1.0/std::pow(wavelength[2], 4.0));
    
    float kr = 0.0025;
    float km = 0.0010;
    float eSun = 20.0;
    float fRayleighScaleDepth = 0.25;
    float fInnerRadius = 10.0;
    float fOuterRadius = 10.25;

    double scale = 1.0 / (fOuterRadius - fInnerRadius);
    
    program->setUniform("v3InvWavelength", invWavelength);

    program->setUniform("fOuterRadius", fOuterRadius);
    program->setUniform("fOuterRadius2", static_cast<float>(std::pow(fOuterRadius, 2.0)));
    program->setUniform("fInnerRadius", fInnerRadius);
    //program->setUniform("fInnerRadius2", static_cast<float>(std::pow(fInnerRadius, 2.0)));

    program->setUniform("fKrESun", static_cast<float>(kr * eSun));
    program->setUniform("fKr4PI", static_cast<float>(kr * 4.0 * M_PI));
    program->setUniform("fKmESun", static_cast<float>(km * eSun));			// Km * ESun
    program->setUniform("fKm4PI", static_cast<float>(km * 4.0 * M_PI));

    program->setUniform("fScale", static_cast<float>(1.0 / (fOuterRadius - fInnerRadius)));
    program->setUniform("fScaleDepth", fRayleighScaleDepth); 
    program->setUniform("fScaleOverScaleDepth", static_cast<float>(scale / fRayleighScaleDepth));	// fScale / fScaleDepth
}

    
void Earth::invalidateGroundProgram() {
    _groundProgramIsDirty = true;
}

void Earth::invalidateAtmosphereProgram() {
    _atmosphereProgramIsDirty = true;
}

    
std::vector<char> Earth::serialize() {
    BinaryStream bs;
    bs << _rotation;
    return bs.data();
}

void Earth::setRotation(double rotation) {
    _rotation = rotation;
}

double Earth::getRotation() {
    return _rotation;
}

    
void Earth::unserialize(std::vector<char> data) {
    BinaryStream bs(data);
    bs >> _rotation;
}


void Earth::loadTextures() {

    _dayTextureSet.loadLevel(0);
    _nightTextureSet.loadLevel(0);
    _glowTextureSet.loadLevel(0);
    _cloudTextureSet.loadLevel(0);
    _waterTextureSet.loadLevel(0);
    
}

}
