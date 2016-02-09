
#include <camerapath.h>
#include <cmath>
#include <fstream>
#include <sstream>

namespace earthrendering {

    CameraPath::CameraPath(const std::string& path) {
        _file = new ghoul::filesystem::File(path);
        load();

        _file->setCallback([this](const ghoul::filesystem::File& file) {
                load();
            });
    }
    

    CameraPath::~CameraPath() {
        delete _file;
    }
    
    void CameraPath::load() {

        std::cout << "reloading camera path." << std::endl;
        
        std::ifstream in(_file->path());
        std::string parameter;

        double time = 0;

        _positions.clear();
        _focuses.clear();
        _ups.clear();
        _cameraRotations.clear();
        _earthRotations.clear();

        std::string line;
        std::stringstream lineStream;

        int lineNumber = 0;
        
        while (getline(in, line)) {
            // Handle comments
            if (line.size() > 2 && line.substr(0, 2) == "//") {
                continue;
            }
            
            // Handle keyframe lines
            lineStream = std::stringstream(line);
            while (lineStream >> parameter) {
                if (parameter == "t") {
                    lineStream >> time;
                } else {
                    
                    if (parameter == "p") {
                        double x, y, z;
                        lineStream >> x >> y >> z;
                        _positions[time] = glm::dvec3(x, y, z);
                        std::cout << "positions[" << time << "]" << ": " << x << ", " << y << ", " << z << std::endl;
                    } else if (parameter == "f") {
                        double x, y, z;
                        lineStream >> x >> y >> z;
                        _focuses[time] = glm::dvec3(x, y, z);
                        std::cout << "focuses[" << time << "]" << ": " << x << ", " << y << ", " << z << std::endl;
                    } else if (parameter == "u") {
                        double x, y, z;
                        lineStream >> x >> y >> z;
                        _ups[time] = glm::dvec3(x, y, z);
                        std::cout << "ups[" << time << "]" << ": " << x << ", " << y << ", " << z << std::endl;                        
                    } else if (parameter == "e") {
                        double e;
                        lineStream >> e;
                        _earthRotations[time] = e;
                        std::cout << "earthRotations[" << time << "]: " << e << std::endl;
                    } else if (parameter == "c") {
                        double c;
                        lineStream >> c;
                        _cameraRotations[time] = c;
                        std::cout << "cameraRotations[" << time << "]" << ": " << c << std::endl;
                    } else {
                        std::cout << "Could not parse file, line number " << lineNumber << std::endl;
                    }
                    
                }
            }
            lineNumber++;
        }
                
        

        _sequenceLength = time;
        
        // TODO: acutally load from file. 
        /*_positions[0.0] = glm::dvec3(0.0, 0.0, -30.0);
        _ups[0.0] = glm::dvec3(0.0, 1.0, 0.0);
        _focuses[0.0] = glm::dvec3(0.0, 0.0, 0.0);

        _positions[10.0] = glm::dvec3(0.0, 0.0, -18.0);
        _ups[10.0] = glm::dvec3(0.0, 1.0, 0.0);
        _focuses[10.0] = glm::dvec3(0.0, 0.0, 0.0);*/

        //_sequenceLength = 10;
    }
    
    void CameraPath::updateCamera(Camera& camera, Earth& earth, double t) {
        if (_sequenceLength > 0.0001) {
            t = fmod(t, _sequenceLength);
        } else {
            t = 0.0;
        }

        double eRotation = interpolate(_earthRotations, t);
        earth.setRotation(eRotation);
        
        double cRotation = interpolate(_cameraRotations, t);
        glm::mat4 cameraRotation = glm::rotate(glm::mat4(1.0), static_cast<float>(cRotation), glm::vec3(0.0, 1.0, 0.0));
        
        glm::vec4 position = glm::vec4(interpolate(_positions, t), 1.0);
        position = cameraRotation * position;
        //std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;

        camera.setPosition(position.xyz());
        
        glm::vec4 focus = glm::vec4(interpolate(_focuses, t), 1.0);
        focus = cameraRotation * focus;
        camera.setFocus(focus.xyz());

        //std::cout << "focus: " << focus.x << ", " << focus.y << ", " << focus.z << std::endl;
        
        glm::vec4 up = glm::vec4(interpolate(_ups, t), 1.0);
        up = cameraRotation * up;
        camera.setUp(up.xyx());
        
        //std::cout << "up: " << up.x << ", " << up.y << ", " << up.z << std::endl;
    }


    void CameraPath::printState(double t) {
        if (_sequenceLength > 0.0001) {
            t = fmod(t, _sequenceLength);
        } else {
            t = 0.0;
        }

        
        double eRotation = interpolate(_earthRotations, t);
        double cRotation = interpolate(_cameraRotations, t);
        
        glm::vec4 position = glm::vec4(interpolate(_positions, t), 1.0);
        glm::vec4 focus = glm::vec4(interpolate(_focuses, t), 1.0);
        glm::vec4 up = glm::vec4(interpolate(_ups, t), 1.0);

        std::cout << "t = " << t << std::endl; 
        
        std::cout << "p " << position.x << ", " << position.y << ", " << position.z << std::endl;
        std::cout << "f " << focus.x << ", " << focus.y << ", " << focus.z << std::endl;
        std::cout << "u " << up.x << ", " << up.y << ", " << up.z << std::endl;

        std::cout << "e " << eRotation << std::endl;
        std::cout << "c " << cRotation << std::endl;
    }
}






