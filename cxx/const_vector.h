//
//  const_vector.h
//  65calc02
//
//  Created by Kelvin Sherlock on 9/11/2011.
//  Copyright 2011 Kelvin W Sherlock LLC. All rights reserved.
//

#ifndef __const_vector_h__
#define __const_vector_h__

#include <stdexcept>

template <class T>
class const_vector
{
private:
    T *_data = nullptr;
    size_t _length = 0;
    
public:
    
    typedef const T *iterator;
    
    const_vector() = default;
    
    const_vector(T *data, size_t length) :
        _data(data), _length(length)
    { }
    
    const_vector(const const_vector<T> &rhs) = default;
    
    size_t size() const 
    {
        return _length;
    }
    
    T operator[](size_t index) const
    {
        return _data[index];
    }
    
    T at(size_t index) const
    {
        if (index >= _length) throw std::out_of_range("const_vector");
        return _data[index];
    }
    
    iterator begin() const
    {
        return _data;
    }
    iterator end() const
    {
        return _data + _length;
    }
    

    T *front() const {
        return _data[0];
    }

    T *back() const {
        return _data[_length - 1];
    }
};


#endif
