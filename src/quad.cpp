#include <quad.h>
#include <iostream>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif


namespace earthrendering {

Quad::Quad()
    : _vaoId(0)
    , _vBufferId(0)
    , _iBufferId(0)
    , _vertices(4)
    , _indices(6) {

	_vertices[0].position[0] = 1;
	_vertices[0].position[1] = 1;
	_vertices[0].position[2] = 0;
	_vertices[0].st[0] = 1;
	_vertices[0].st[1] = 1;

	_vertices[1].position[0] = -1;
	_vertices[1].position[1] = 1;
	_vertices[1].position[2] = 0;
	_vertices[1].st[0] = 0;
	_vertices[1].st[1] = 1;
   
	_vertices[2].position[0] = -1;
	_vertices[2].position[1] = -1;
	_vertices[2].position[2] = 0;
	_vertices[2].st[0] = 0;
	_vertices[2].st[1] = 0;

	_vertices[3].position[0] = 1;
	_vertices[3].position[1] = -1;
	_vertices[3].position[2] = 0;
	_vertices[3].st[0] = 1;
	_vertices[3].st[1] = 0;
    
	_indices[0] = 0;
	_indices[1] = 1;
	_indices[2] = 2;
	_indices[3] = 2;
	_indices[4] = 3;
	_indices[5] = 0;


    
}

Quad::~Quad() {
    glDeleteBuffers(1, &_vBufferId);
    glDeleteBuffers(1, &_iBufferId);
    glDeleteVertexArrays(1, &_vaoId);
}
	
bool Quad::initialize() {
    // Initialize and upload to graphics card
    if (_vaoId == 0)
        glGenVertexArrays(1, &_vaoId);

    if (_vBufferId == 0) {
        glGenBuffers(1, &_vBufferId);

        if (_vBufferId == 0) {
            std::cout << "Could not create vertex buffer" << std::endl;
            return false;
        }
    }

    if (_iBufferId == 0) {
        glGenBuffers(1, &_iBufferId);

        if (_iBufferId == 0) {
            std::cout << "Could not create index buffer" << std::endl;
            return false;
        }
    }

    // First VAO setup
    glBindVertexArray(_vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, _vBufferId);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const GLvoid*>(offsetof(Vertex, position)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const GLvoid*>(offsetof(Vertex, st)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const GLvoid*>(offsetof(Vertex, normal)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(int), _indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    
}


void Quad::render() {
    glBindVertexArray(_vaoId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iBufferId);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

}
