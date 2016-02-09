#include <renderable.h>
#include <vector>

namespace earthrendering {

class Camera;
    
class Renderer : public Renderable  {
public:
    virtual void addChild(Renderable* child);
    virtual void removeChild(Renderable* child);
    void render(Camera& camera) override;
private:
    std::vector<Renderable*> _children;
};

}
