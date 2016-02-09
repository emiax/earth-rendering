#pragma once

#include <sgct.h>


namespace earthrendering {


    
class Camera {
public:
    Camera::Camera();
    Camera::~Camera();
    glm::vec3 getPosition(); // return the camera position in world coordinates
    glm::vec3 getSgctPosition(); // return the camera position in world coordaintes with sgct's model view transform applied (inverse model view matrix applied to camera position)
    glm::vec3 getUp(); // return the up vector
    void setPosition(glm::vec3 p);
    void setFocus(glm::vec3 f); // compute new direction given new focus and current position.
    glm::vec3 getDirection();
    void setDirection(glm::vec3 d);
    void setUp(glm::vec3 u);
    void setSgctModelViewMatrix(glm::mat4 m);
    void setSgctProjectionMatrix(glm::mat4 m);
    void moveLocal(glm::vec3 v);
    glm::mat4 getViewProjectionMatrix();

    std::vector<char> serialize();
    void unserialize(std::vector<char> data);

private:
    void invalidateCameraMatrix();
    void invalidateInverseCameraMatrix();
    void invalidateModelViewProjectionMatrix();
    void invalidateInverseSgctModelViewMatrix();
    void invalidateSgctPosition();
    
    void updateCameraMatrix();
    void updateInverseCameraMatrix();
    void updateInverseSgctModelViewMatrix();
    void updateModelViewProjectionMatrix();
    void updateSgctPosition();

    glm::vec3 _position;
    glm::vec3 _direction;
    glm::vec3 _up;
    
    glm::mat4 _cameraMatrix; // The transformation GENERATED by the camera node, generated from position, direction and up.
    glm::mat4 _inverseCameraMatrix; // GENERATED from inverting camera matrix
    glm::mat4 _sgctModelViewMatrix; // Sgct's model view matrix, SET from client
    glm::mat4 _sgctProjectionMatrix; // Sgct's projection matrix, SET from client
    glm::mat4 _modelViewProjectionMatrix; // GENERATED by taking sgctProjection * sgctModelView * camera
    glm::mat4 _inverseSgctModelViewMatrix; // GENERATED by inverting sgctModelView matrix
    
    bool _dirtyCameraMatrix;
    bool _dirtyInverseCameraMatrix;
    bool _dirtyModelViewProjectionMatrix;
    bool _dirtyInverseSgctModelViewMatrix;
    bool _dirtySgctPosition;

    glm::vec3 _sgctPosition; // GENERATED from inverseSgctModelView * position
};

}