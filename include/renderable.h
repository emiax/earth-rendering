#pragma once


namespace earthrendering {

class Camera;

class Renderable {
public: 
    virtual void render(Camera& camera) = 0;
};

}
