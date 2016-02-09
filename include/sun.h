#include <renderable.h>
#include <sgct.h>
#include <textureset.h>
#include <quad.h>

namespace ghoul {
    namespace opengl {
        class ProgramObject;
        class Texture;
    }
}


namespace earthrendering {

class Sun : public Renderable  {
public:
    Sun();
    ~Sun();
    bool initialize();
    void render(Camera& camera) override;
    void setPosition(glm::vec3 position);
    void invalidateProgram();
    std::vector<char> serialize();
    void unserialize(std::vector<char> data);
    
private:
    void loadTextures();
    bool _programIsDirty = false;
    ghoul::opengl::ProgramObject *_program;
    glm::vec3 _position;

    Quad _billboard;
};

}
