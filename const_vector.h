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
    T *_data;
    size_t _length;
    
public:
    
    typedef const T *iterator;
    
    const_vector() : 
        _data(NULL), _length(0)
    {}
    
    const_vector(T *data, size_t length) :
        _data(data), _length(length)
    { }
    
    const_vector(const const_vector<T> &rhs) :
        _data(rhs._data), _length(rhs._length)
    { }
    
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
    
};


#endif
