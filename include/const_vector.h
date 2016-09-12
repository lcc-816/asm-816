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
    
    constexpr const_vector() noexcept = default;
    
    constexpr const_vector(T *data, size_t length) noexcept :
        _data(data), _length(length)
    { }
    
    constexpr const_vector(const const_vector<T> &rhs) noexcept = default;
    
    constexpr size_t size() const noexcept 
    {
        return _length;
    }
    
    constexpr T operator[](size_t index) const noexcept
    {
        return _data[index];
    }
    
    constexpr T at(size_t index) const
    {
        if (index >= _length) throw std::out_of_range("const_vector");
        return _data[index];
    }
    
    constexpr iterator begin() const noexcept 
    {
        return _data;
    }
    constexpr iterator end() const noexcept 
    {
        return _data + _length;
    }
    

    constexpr T *front() const noexcept {
        return _data[0];
    }

    constexpr T *back() const noexcept {
        return _data[_length - 1];
    }
};


#endif
