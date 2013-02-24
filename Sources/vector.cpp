// Copyright (c) 2009 Olivier Crave
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <math.h>

#include "vector.h"

// Initializes to Vector <0, 0, 0, [1]>
Vector::Vector(void) : x (0.0f), y (0.0f), z (0.0f), w (1.0f) {}

// Initializes to Vector <a, b, c, 1>
Vector::Vector(float a, float b, float c) : x (a), y (b), z (c), w (1.0f) {}

// Initializes to Vector <a, b, c, d>
Vector::Vector(float a, float b, float c, float d) : x (a), y (b), z (c), w (d) {}

Vector::~Vector() {}

// Returns the Magnitude of the current Vector
float Vector::Magnitude(void)
{
    return static_cast<float>(sqrt(x * x + y * y + z * z));
}

// Normalizes the current Vector to a Unit Vector (Scales to magnitude of 1)
Vector Vector::Normalize(void)
{
    float currentMagnitude = Magnitude();
    if(currentMagnitude != 0.0)
    {
        x /= currentMagnitude;
        y /= currentMagnitude;
        z /= currentMagnitude;
    }
    return *this;
}

// Scales the current Vector by a factor
Vector Vector::Scale(float scale_factor)
{
    x *= scale_factor;
    y *= scale_factor;
    z *= scale_factor;
    w *= scale_factor;
    return *this;
}

// Calculates the Dot-Product between this and another Vector
float Vector::Dot(Vector vec2)
{
    return x * vec2.x + y * vec2.y + z * vec2.z;
}

// Returns the Cross-Product between this and another Vector
Vector Vector::Cross(Vector vec2)
{
    Vector cross_vec;
    cross_vec.x = y * vec2.z - vec2.y * z;
    cross_vec.y = vec2.x * z - x * vec2.z;
    cross_vec.z = x * vec2.y - vec2.x * y;
    cross_vec.w = 0.0f;
    return cross_vec;
}

// Allows you to simply write "vec1 = vec2"
Vector Vector::operator = (const Vector vec2)
{
    x = vec2.x;
    y = vec2.y;
    z = vec2.z;
    w = vec2.w;
    return *this;
}

// Allows to check if two vectors equal each other
bool Vector::operator == (const Vector vec2)
{
    return (x == vec2.x && y == vec2.y && z == vec2.z && w == vec2.w);
}

// Allows to check if two vectors don't equal to each other
bool Vector::operator != (const Vector vec2)
{
    return (x != vec2.x || y != vec2.y || z != vec2.z || w != vec2.w);
}

// Allows to check if one vector is less than the other
// NOTE: This operator overload has no mathematical meanning
bool Vector::operator < (const Vector vec2)
{
    if(x == vec2.x)
    {   // x is considered to be the most important
        // value to compare two vectors
        if(y == vec2.y)
        {   // y is considered to be second most important
            // value to compare two vectors
            if(z == vec2.z)
            {   // z is considered to be least important value
                // to compare two vectors
                return false;
            }
            else
            {
                return (z < vec2.z);
            }
        }
        else
        {
            return (y < vec2.y);
        }
    }
    else
    {
        return (x < vec2.x);
    }
}

// NOTE: The following arithmetic operator overloads DO NOT change the value of
// the current vector
// Returns the sum of vectors
Vector Vector::operator + (const Vector vec2)
{
    return Vector(x + vec2.x, y + vec2.y, z + vec2.z, w + vec2.w);
}

// Returns the difference of vectors
Vector Vector::operator - (const Vector vec2)
{
    return Vector(x - vec2.x, y - vec2.y, z - vec2.z, w - vec2.w);
}

// Returns the vector scaled by a factor
Vector Vector::operator * (const float scale_factor)
{
    return Vector(x * scale_factor, y * scale_factor, z * scale_factor,
                  w * scale_factor);
}

// Returns the vector scaled by a factor
Vector Vector::operator / (const float scale_factor)
{
    return Vector(x / scale_factor, y / scale_factor, z / scale_factor,
                  w / scale_factor);
}

// Returns the vector multiplied by the other
Vector Vector::operator * (const Vector scale_vector)
{
    return Vector(x * scale_vector.x, y * scale_vector.y, z * scale_vector.z,
                  w * scale_vector.w);
}

// Returns the vector divided by the other
Vector Vector::operator / (const Vector scale_vector)
{
    return Vector(x / scale_vector.x, y / scale_vector.y, z / scale_vector.z,
                  w / scale_vector.w);
}
