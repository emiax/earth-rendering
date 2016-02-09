#pragma once

#include <map>
#include <sgct.h>
#include <string>
#include <camera.h>
#include <earth.h>
#include <ghoul/misc/interpolator.h>
#include <ghoul/filesystem/file.h>

#include <iostream>

namespace earthrendering {

class CameraPath {
public: 
    CameraPath(const std::string& path);
    ~CameraPath();
    void updateCamera(Camera& camera, Earth& earth, double t);
    void printState(double t);

private:
    void load();

    ghoul::filesystem::File* _file;
    
    std::map<double, glm::dvec3> _positions;
    std::map<double, glm::dvec3> _focuses;
    std::map<double, glm::dvec3> _ups;

    std::map<double, double> _earthRotations;
    std::map<double, double> _cameraRotations;
    
    double _sequenceLength;

    template <typename T>
    T centripetalCatmullRom(double t, double t0, double t1, double t2, double t3, T p0, T p1, T p2, T p3) {
        double alpha = 0.5;


        /*    t = 
    t0 = 0;
    t1 = static_cast<double>(glm::length(p1 - p0))*alpha;
    t2 = static_cast<double>(glm::length(p2 - p1))*alpha + t1;
    t3 = static_cast<double>(glm::length(p3 - p2))*alpha + t2;*/
    
    
    T a1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
    T a2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;
    T a3 = (t3 - t) / (t3 - t2) * p2 + (t - t2) / (t3 - t2) * p3;

    T b1 = (t2 - t) / (t2 - t0) * a1 + (t - t0) / (t2 - t0) * a2;
    T b2 = (t3 - t) / (t3 - t1) * a2 + (t - t1) / (t3 - t1) * a3;

    T c = (t2 - t) / (t2 - t1) * b1 + (t - t1) / (t2 - t1) * b2;

    return c;

    }
    
    template <typename T>
    T interpolate(std::map<double, T> map, double t) {
		// No keyframe: return default element.
		if (map.size() < 1) {
			return T();
		}


        /*        std::cout << "interpolating for t = " << t << std::endl;
        for (auto i = map.begin(); i != map.end(); i++) {
            std::cout << "key: " << i->first << ". " << std::endl;
            }*/


        std::map<double, T>::iterator min = map.begin();
        std::map<double, T>::iterator max = map.end();
        max--;

        std::map<double, T>::iterator prev, next;
        
        if (t <= min->first) {
            return min->second;
        } else if (t >= max->first) {
            return max->second;
        } else {
            next = map.lower_bound(t);
            prev = next;
            prev--;

            // at least one of the iterators are ok, since there are at least 1 element.
            if (next == map.end()) {
                next = prev;
            }
            if (prev == map.end()) {
                prev = next;
            }
        }
        
        assert(prev != map.end());
        assert(next != map.end());


        T prevVal, nextVal, beforePrevVal, afterNextVal;
        double prevT, nextT, beforePrevT, afterNextT;
        nextVal = next->second;
        prevVal = prev->second;
        prevT = prev->first;
        nextT = next->first;
                
        
        // point to beforePrevious element only if one exists
        auto beforePrev = prev;
        if (prev == map.begin()) {
            beforePrevVal = prevVal;
            beforePrevT = 2 * prevT - nextT;
        } else {
            beforePrev--;
            beforePrevVal = beforePrev->second;
            beforePrevT = beforePrev->first;
        }
        
        // point to afterNext element only if one exists
        auto afterNext = next;
        afterNext++;
        if (afterNext == map.end()) {
            afterNext = next;
            afterNextVal = nextVal;
            afterNextT = 2 * nextT - prevT;
        } else {
            afterNextVal = afterNext->second;
            afterNextT = afterNext->first;
        }
        
        /*std::cout << "beforePrev: " << beforePrev->first << ". " << std::endl;
        std::cout << "prev: " << prev->first << ". " << std::endl;
        std::cout << "next: " << next->first << ". " << std::endl;
        std::cout << "afterNext: " << afterNext->first << ". " << std::endl;*/
        double fact = (t - prevT) / (nextT - prevT);

        /*auto anchor0 = prev->second;
        auto anchor1 = next->second;

        auto control0 = 0.67 * anchor0 + 0.33 * anchor1; 
        auto control1 = 0.33 * anchor0 + 0.67 * anchor1; 
        

        ghoul::Interpolator<ghoul::Interpolators::CubicBezier> interpolator;
        return interpolator.interpolate(fact, anchor0, control0, control1, anchor1);*/
        
        //ghoul::Interpolator<ghoul::Interpolators::CatmullRom> interpolator;
        //return interpolator.interpolate(fact, beforePrev->second, prev->second, next->second, afterNext->second);

        return centripetalCatmullRom(t, beforePrevT, prevT, nextT, afterNextT, beforePrevVal, prevVal, nextVal, afterNextVal);
        
        
        //ghoul::Interpolator<ghoul::Interpolators::Linear> interpolator;
        //return interpolator.interpolate(fact, prev->second, next->second);
        //return T();
    };

};

}
