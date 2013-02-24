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

#ifndef VECTOR_H
#define VECTOR_H

// Vector Class - A float triplet class with several vector-operation functions
// This class can be used to simplify vector math programming
class Vector
{
 public:
    // Initializes to Vector <0, 0, 0, [1]>
    Vector(void);
    // Initializes to Vector <a, b, c, 1>
    Vector(float a, float b, float c);
    // Initializes to Vector <a, b, c, d>
    Vector(float a, float b, float c, float d);
    ~Vector();
    // Returns the Magnitude of the current Vector
    float Magnitude(void);
    // Normalizes to a Unit Vector (Scales to magnitude of 1)
    Vector Normalize(void);
    // Scales the Vector by a factor
    Vector Scale(float scale_factor);
    // Calculates the Dot-Product between this and another Vector
    float Dot(Vector vec2);
    // Returns the Cross-Product between this and another Vector
    Vector Cross(Vector vec2);
    // Allows you to simply write "vec1 = vec2"
    Vector operator = (const Vector vec2);
    // Allows to check if two vectors equal each other
    bool operator == (const Vector vec2);
    // Allows to check if two vectors are not equal to
    // each other
    bool operator != (const Vector vec2);
    // Allows to check if one vector is less than the other
    // NOTE: This operator overload has no mathematical meanning
    bool operator < (const Vector vec2);
    // NOTE: The following arithmetic operator overloads DO NOT change the value
    // of the current vector
    // Returns the sum of vectors
    Vector operator + (const Vector vec2);
    // Returns the difference of vectors
    Vector operator - (const Vector vec2);
    // Returns the vector scaled by a factor
    Vector operator * (const float scale_factor);
    // Returns the vectors scaled by a factor
    Vector operator / (const float scale_factor);
    // Returns the vector multiplied by the other
    Vector operator * (const Vector scale_vector);
    // Returns the vector divided by the other
    Vector operator / (const Vector scale_vector);
    float x, y, z, w;
};

#endif  // VECTOR_H
