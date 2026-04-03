#ifndef VECTOR_H
#define VECTOR_H

struct Vector {
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    Vector() = default;
    Vector(float x, float y, float z) : x(x), y(y), z(z) {}
};

#endif  // VECTOR_H
