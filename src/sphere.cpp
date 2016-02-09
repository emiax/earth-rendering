#include <sphere.h>
#include <iostream>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif


namespace earthrendering {

Sphere::Sphere(int nSegments)
    : _vaoId(0)
    , _vBufferId(0)
    , _iBufferId(0)
    , _vertices((nSegments + 1) * (nSegments + 1))
    , _indices(6 * nSegments * nSegments) {


	int index = 0;
	const float fSegments = static_cast<float>(nSegments);

	for (int i = 0; i <= nSegments; i++) {
		// define an extra vertex around the y-axis due to texture mapping
		for (int j = 0; j <= nSegments; j++) {
			const float fi = static_cast<float>(i);
			const float fj = static_cast<float>(j);
			// inclination angle (north to south)
			const float theta = fi * float(M_PI) / fSegments;  // 0 -> PI
			// azimuth angle (east to west)
			const float phi = fj * float(M_PI) * 2.0f / fSegments;  // 0 -> 2*PI

			const float x = sin(phi) * sin(theta);  //
			const float y = cos(theta);             // up
			const float z = cos(phi) * sin(theta);  //

			_vertices[index].position[0] = x;
			_vertices[index].position[1] = y;
			_vertices[index].position[2] = z;

			glm::vec3 normal = glm::vec3(x, y, z);
			if (!(x == 0.f && y == 0.f && z == 0.f))
				normal = glm::normalize(normal);

			_vertices[index].normal[0] = normal[0];
			_vertices[index].normal[1] = normal[1];
			_vertices[index].normal[2] = normal[2];

			const float t1 = fj / fSegments;
			const float t2 = fi / fSegments;

			_vertices[index].st[0] = t1;
			_vertices[index].st[1] = t2;
			++index;
		}
	}

	index = 0;
	// define indices for all triangles
	for (int i = 1; i <= nSegments; ++i) {
		for (int j = 0; j < nSegments; ++j) {
			const int t = nSegments + 1;
			_indices[index] = t * (i - 1) + j + 0; //1
			++index;
			_indices[index] = t * (i + 0) + j + 0; //2 
			++index;
			_indices[index] = t * (i + 0) + j + 1; //3
			++index;

			_indices[index] = t * (i - 1) + j + 0; //4 
			++index;
			_indices[index] = t * (i + 0) + j + 1; //5
			++index;
			_indices[index] = t * (i - 1) + j + 1; //6
			++index;
		}
	}

}

Sphere::~Sphere() {
    glDeleteBuffers(1, &_vBufferId);
    glDeleteBuffers(1, &_iBufferId);
    glDeleteVertexArrays(1, &_vaoId);
}
	
bool Sphere::initialize() {
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


void Sphere::render() {
    glBindVertexArray(_vaoId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iBufferId);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

}
