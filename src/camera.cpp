#include <camera.h>
#include <glm/gtx/transform.hpp>
#include <binarystream.h>

namespace earthrendering {

Camera::Camera() {
}

Camera::~Camera() {      
}

glm::vec3 Camera::getPosition() {
    return _position;
}

glm::vec3 Camera::getSgctPosition() {
    if (_dirtySgctPosition) {
        //std::cout << "updating sgct position" << std::endl;
        updateSgctPosition();
    }
    return _sgctPosition;
}
    
glm::vec3 Camera::getDirection() {
    return _direction;
}

glm::vec3 Camera::getUp() {
    return _up;
}
    
void Camera::setPosition(glm::vec3 p) {
    _position = p;
    invalidateCameraMatrix();
}

void Camera::setFocus(glm::vec3 f) {
    _direction = glm::normalize(f - _position);
    invalidateCameraMatrix();
}

void Camera::setDirection(glm::vec3 d) {
    _direction = glm::normalize(d);
    invalidateCameraMatrix();
}

void Camera::setUp(glm::vec3 u) {
    _up = u;
    invalidateCameraMatrix();
}

void Camera::setSgctModelViewMatrix(glm::mat4 m) {
    _sgctModelViewMatrix = m;
    invalidateModelViewProjectionMatrix();
    invalidateInverseSgctModelViewMatrix();
}

void Camera::setSgctProjectionMatrix(glm::mat4 m) {
    _sgctProjectionMatrix = m;
    invalidateModelViewProjectionMatrix();
}
    
glm::mat4 Camera::getViewProjectionMatrix() {
    if (_dirtyModelViewProjectionMatrix) {
        updateModelViewProjectionMatrix();
    }
    return _modelViewProjectionMatrix;
}

    
void Camera::invalidateCameraMatrix() {
    _dirtyCameraMatrix = true;
    invalidateInverseCameraMatrix();
    invalidateModelViewProjectionMatrix();
}


void Camera::invalidateModelViewProjectionMatrix() {
    _dirtyModelViewProjectionMatrix = true;
}

void Camera::invalidateInverseCameraMatrix() {
    _dirtyInverseCameraMatrix = true;
    invalidateSgctPosition();
}
    
void Camera::invalidateInverseSgctModelViewMatrix() {
    _dirtyInverseSgctModelViewMatrix = true;
    invalidateSgctPosition();
}
    
void Camera::invalidateSgctPosition() {
    _dirtySgctPosition = true; 
}
    

void Camera::moveLocal(glm::vec3 v) {
    // x is right
    // y is up
    // z is backwards
    glm::vec3 right = glm::normalize(glm::cross(_direction, _up));
    glm::vec3 realUp = glm::cross(right, _direction);

    _position += right * v.x;
    _position += realUp * v.y;
    _position -= _direction * v.z;
    invalidateCameraMatrix();
}

    
void Camera::updateCameraMatrix() {
    glm::vec3 right = glm::normalize(glm::cross(_direction, _up));
    glm::vec3 realUp = glm::cross(right, _direction);

    _cameraMatrix = glm::lookAt(_position, _position + _direction, realUp);
    _dirtyCameraMatrix = false;
}

void Camera::updateInverseCameraMatrix() {
    if (_dirtyCameraMatrix) {
        updateCameraMatrix();
    }
    _inverseCameraMatrix = glm::inverse(_cameraMatrix);
}

void Camera::updateModelViewProjectionMatrix() {
    if (_dirtyCameraMatrix) {
        updateCameraMatrix();
    }
    _modelViewProjectionMatrix = _sgctProjectionMatrix * _sgctModelViewMatrix * _cameraMatrix;
    _dirtyModelViewProjectionMatrix = false;
}

void Camera::updateSgctPosition() {
    if (_dirtyInverseCameraMatrix) {
        updateInverseCameraMatrix();
    }
    if (_dirtyInverseSgctModelViewMatrix) {
        updateInverseSgctModelViewMatrix();
    }

    
    glm::vec4 origin = glm::vec4(0.0, 0.0, 0.0, 1.0);
    
    _sgctPosition = (_inverseCameraMatrix * _inverseSgctModelViewMatrix * origin).xyz();
    _dirtySgctPosition = false;
}

void Camera::updateInverseSgctModelViewMatrix() {
    //std::cout << "updating inverse matrix" << std::endl;
    _inverseSgctModelViewMatrix = glm::inverse(_sgctModelViewMatrix);
    _dirtyInverseSgctModelViewMatrix = false;
}
    

std::vector<char> Camera::serialize() {
    BinaryStream bs;
    bs << _position;
    bs << _direction;
    bs << _up;
    return bs.data();
}


void Camera::unserialize(std::vector<char> data) {
    BinaryStream bs(data);
    bs >> _position;
    bs >> _direction;
    bs >> _up;
    invalidateCameraMatrix();
}


    
}
