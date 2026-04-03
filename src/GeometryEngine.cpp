// Copyright (C) 2009-2015 Olivier Crave
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

#include "GeometryEngine.hpp"

using namespace stlviewer;

GeometryEngine::GeometryEngine()
    : vertexBuf(QOpenGLBuffer::VertexBuffer)
    , normalBuf(QOpenGLBuffer::VertexBuffer)
{
    this->initializeOpenGLFunctions();

    this->vao.create();
    // Generate 2 VBOs
    this->vertexBuf.create();
    this->normalBuf.create();
}

GeometryEngine::~GeometryEngine()
{
    this->vertexBuf.destroy();
    this->normalBuf.destroy();
}

void GeometryEngine::initGeometry(StlFile &_stlfile)
{
    _stlfile.reset();
    StlFile::Stats stats = _stlfile.getStats();
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    for (int i = 0; i < stats.numFacets; ++i)
    {
        StlFile::Facet facet = _stlfile.getNextFacet();

        vertices << QVector3D(facet.vector[0].x, facet.vector[0].y, facet.vector[0].z);
        vertices << QVector3D(facet.vector[1].x, facet.vector[1].y, facet.vector[1].z);
        vertices << QVector3D(facet.vector[2].x, facet.vector[2].y, facet.vector[2].z);

        QVector3D n(facet.normal.x, facet.normal.y, facet.normal.z);
        normals << n;
        normals << n;
        normals << n;
    }

    // Transfer vertex data to VBO
    this->vertexBuf.bind();
    this->vertexBuf.allocate(vertices.constData(), vertices.length() * sizeof(QVector3D));

    // Transfer normal data to VBO
    this->normalBuf.bind();
    this->normalBuf.allocate(normals.constData(), normals.length() * sizeof(QVector3D));
}

void GeometryEngine::drawTriangleGeometry(QOpenGLShaderProgram &_program)
{
    QOpenGLVertexArrayObject::Binder vaoBinder(&this->vao);

    // 1st attribute buffer : vertices
    this->vertexBuf.bind();
    int vertexAttr = _program.attributeLocation("a_position");
    _program.enableAttributeArray(vertexAttr);
    _program.setAttributeBuffer(vertexAttr, GL_FLOAT, 0, 3, sizeof(QVector3D));

    // 2rd attribute buffer : normals
    this->normalBuf.bind();
    int normalAttr = _program.attributeLocation("a_normal");
    _program.enableAttributeArray(normalAttr);
    _program.setAttributeBuffer(normalAttr, GL_FLOAT, 0, 3, sizeof(QVector3D));

    //int vertexColor = _program.attributeLocation("a_color");
    //_program.enableAttributeArray(vertexColor);
    //_program.setAttributeValue(vertexColor, QVector3D(1.0, 0.0, 1.0));

    glDrawArrays(GL_TRIANGLES, 0, this->vertexBuf.size() / sizeof(QVector3D));
}
