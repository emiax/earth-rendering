#pragma once

#include <sgct.h>
#include <sstream>
#include <iostream>

namespace earthrendering {

class BinaryStream {
public:
    BinaryStream() {}
    
    BinaryStream(std::vector<char> v) {
        _s.write(v.data(), v.size());
    }
    
    void clear() {
        _s = std::stringstream();
    }
    
    template <typename T>
    BinaryStream& operator<<(T& rhs) {
        _s.write(reinterpret_cast<char*>(&rhs), sizeof(T));
        return *this;
    }

    template <typename T>
    BinaryStream& operator<<(std::vector<T>& rhs) {
        size_t size = rhs.size();
        _s.write(reinterpret_cast<char*>(&size), sizeof(size_t));
        _s.write(reinterpret_cast<char*>(rhs.data()), rhs.size());
        return *this;
    }

    template <typename T>
    BinaryStream& operator>>(T& rhs) {
        _s.read(reinterpret_cast<char*>(&rhs), sizeof(T));
        return *this;
    }
        
    template <typename T>
    BinaryStream& operator>>(std::vector<T>& rhs) {
        size_t size;
        _s.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        rhs.resize(size);
        
        _s.read(reinterpret_cast<char*>(rhs.data()), size);
        return *this;
    }
    
    std::vector<char> data() {
        std::string s = _s.str();
        std::vector<char> v(s.size());
        memcpy(v.data(), s.data(), s.size());
        return v;
    }
private:
    std::stringstream _s;
};
}
