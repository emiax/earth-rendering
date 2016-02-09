#include <renderer.h>
#include <camera.h>

namespace earthrendering {

void Renderer::addChild(Renderable* renderable) {
    _children.push_back(renderable);
}

void Renderer::removeChild(Renderable* renderable) {
    auto i = std::find(_children.begin(), _children.end(), renderable);
    if (i != _children.end()) {
        _children.erase(i);
    }
}

void Renderer::render(Camera& camera) {
    for (auto i : _children) {
        i->render(camera);
    }
}

}
