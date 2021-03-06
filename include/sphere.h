#pragma once

#include <sgct.h>


namespace earthrendering {

class Sphere {
public:
    Sphere(int nSegments);
    ~Sphere();
    bool initialize();
    void render();
    
    struct Vertex {
        GLfloat position[3];
        GLfloat st[2];
        GLfloat normal[3];
    };

private:
    GLuint _vaoId;
    GLuint _vBufferId;
    GLuint _iBufferId;
    std::vector<Vertex> _vertices;
    std::vector<int> _indices;

};

}
