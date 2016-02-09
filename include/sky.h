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

class Sky : public Renderable  {
public:
    Sky(int nSegments);
    ~Sky();
    bool initialize();
    void render(Camera& camera) override;
    void invalidateProgram();
    std::vector<char> serialize();
    void unserialize(std::vector<char> data);
private:
    void loadTextures();
    bool _programIsDirty = false;
    ghoul::opengl::ProgramObject *_program;

    Sphere _sphere;
    TextureSet _starsTextureSet;
};

}
