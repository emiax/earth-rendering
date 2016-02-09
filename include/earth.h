#pragma once

#include <renderable.h>
#include <sgct.h>
#include <textureset.h>
#include <sphere.h>

namespace ghoul {
    namespace opengl {
        class ProgramObject;
        class Texture;
    }
}


namespace earthrendering {

class Earth : public Renderable  {
public:
    Earth(int nSegments);
    ~Earth();
    bool initialize();
    void render(Camera& camera) override;
    void setSunPosition(glm::vec3 sunPosition);
    void invalidateGroundProgram();
    void invalidateAtmosphereProgram();
    std::vector<char> serialize();
    void unserialize(std::vector<char> data);
    void setRotation(double rotation);
    double getRotation();
    
private:
    int _time;
    void loadTextures();
    bool _groundProgramIsDirty = false;
    bool _atmosphereProgramIsDirty = false;
    ghoul::opengl::ProgramObject *_groundProgram;
    ghoul::opengl::ProgramObject *_atmosphereProgram;
    glm::vec3 _sunPosition;
    double _rotation = 0;
    void setAtmosphericUniforms(ghoul::opengl::ProgramObject* program);
    
    TextureSet _dayTextureSet;
    TextureSet _nightTextureSet;
    TextureSet _glowTextureSet;
    TextureSet _cloudTextureSet;
    TextureSet _waterTextureSet;

    Sphere _sphere;
};

}
