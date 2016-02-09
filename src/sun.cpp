#include <sun.h>
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

Sun::Sun()
{
    
}

Sun::~Sun() {
}

bool Sun::initialize() {
    _program = ghoul::opengl::ProgramObject::Build(
        "Sun Program",
        "${SHADERS}/sun.vert",
        "${SHADERS}/sun.frag");

    _billboard.initialize();

    std::cout << "Initialized sun!" << std::endl;

    return true;
}

void Sun::setPosition(glm::vec3 position) {
    _position = position;
}
    
void Sun::render(Camera& camera) {
    if (_programIsDirty || _program->isDirty()) {
		_program->rebuildFromFile();
        _programIsDirty = false;
	}
    glm::vec3 cameraPosition = camera.getSgctPosition();
    glm::vec3 rawCameraPosition = camera.getPosition();

    glm::vec3 camUp = camera.getUp();
    glm::vec3 camDirection = camera.getDirection();
    glm::vec3 camRight = glm::normalize(glm::cross(camDirection, camUp));
    glm::vec3 realCamUp = glm::cross(camRight, camDirection);

    glm::vec3 normal = glm::normalize(_position - rawCameraPosition);
    glm::vec3 right = glm::normalize(glm::cross(normal, camUp));
    glm::vec3 up = glm::cross(right, normal);


    glm::mat4 rotation = glm::scale(glm::vec3(1.0, 1.0, 1.0));
    rotation[0] = glm::vec4(right, 0.0);
    rotation[1] = glm::vec4(up, 0.0);
    rotation[2] = glm::vec4(normal, 0.0);


/*    std::cout << "cam pos" << rawCameraPosition.x << ", " << rawCameraPosition.y << ", " << rawCameraPosition.z << std::endl;

    std::cout << "pos" << _position.x << ", " << _position.y << ", " << _position.z << std::endl;

    
    std::cout << "normal: " << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
    std::cout << "right: " << right.x << ", " << right.y << ", " << right.z << std::endl;
    std::cout << "up: " << up.x << ", " << up.y << ", " << up.z << std::endl;        */

    glm::mat4 translation = glm::translate(glm::scale(glm::vec3(1.0, 1.0, 1.0)), _position);
    glm::mat4 modelMatrix = translation * rotation;
    
    // Render sun

    _program->activate();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //_program->setUniform("cameraPosition", cameraPosition);
    //_program->setUniform("sunPosition", _sunPosition);
    _program->setUniform("viewProjection", camera.getViewProjectionMatrix());
    _program->setUniform("model", modelMatrix);
    
    _billboard.render();
    
    _program->deactivate();
    
}



    
void Sun::invalidateProgram() {
    _programIsDirty = true;
}

    
std::vector<char> Sun::serialize() {
    BinaryStream bs;
    bs << _position;
    return bs.data();
}


 void Sun::unserialize(std::vector<char> data) {
    BinaryStream bs(data);
    bs >> _position;
}



}
