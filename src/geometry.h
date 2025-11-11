#pragma once
#include <cassert>
#include <cmath>
#include <iostream>

#if DEBUG_BUILD
#define RNDR_ASSERT(x) do { assert(iss.eof()); } while(0);
#else
#define RNDR_ASSERT(x)
#endif

template <int n> struct vec {
    double data[n] = {0};
    double &operator[](const int i) {
        assert(i >= 0 && i < n);
        return data[i];
    }
    double operator[](const int i) const {
        assert(i >= 0 && i < n);
        return data[i];
    }
};

template <int n> std::ostream &operator<<(std::ostream &out, const vec<n> &v) {
    for (int i = 0; i < n; i++)
        out << v[i] << " ";
    return out;
}

template <> struct vec<3> {
    double x = 0, y = 0, z = 0;
    double &operator[](const int i) {
        assert(i >= 0 && i < 3);
        return i ? (1 == i ? y : z) : x;
    }
    double operator[](const int i) const {
        assert(i >= 0 && i < 3);
        return i ? (1 == i ? y : z) : x;
    }
};

using vec3 = vec<3>;