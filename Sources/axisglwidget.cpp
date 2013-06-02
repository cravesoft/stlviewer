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

#include "axisglwidget.h"

AxisGLWidget::AxisGLWidget(QWidget *parent) : QGLWidget(parent)
{
    xRot = yRot = zRot = 0;
    black = QColor::fromRgbF(0.0, 0.0, 0.0);
    purple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

AxisGLWidget::~AxisGLWidget()
{
    makeCurrent();
}

void AxisGLWidget::reset()
{
    xRot = yRot = zRot = 0;
}

QSize AxisGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize AxisGLWidget::sizeHint() const
{
    return QSize(100, 100);
}

void AxisGLWidget::setXRotation(const int angle)
{
    xRot = angle;
    updateGL();
}

void AxisGLWidget::setYRotation(const int angle)
{
    yRot = angle;
    updateGL();
}

void AxisGLWidget::setZRotation(const int angle)
{
    zRot = angle;
    updateGL();
}

void AxisGLWidget::initializeGL()
{
    qglClearColor(purple.dark());
	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
    // Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void AxisGLWidget::paintGL()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
    drawAxes();
}

void AxisGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Adjust clipping box
    if(width <= height)
        glOrtho(-10.0f, 10.0f, -10.0f*height/width,
                10.0f*height/width, -10.0f, 10.0f);
    else
        glOrtho(-10.0f*width/height, 10.0f*width/height,
                -10.0f, 10.0f, -10.0f, 10.0f);
    glMatrixMode(GL_MODELVIEW);
}

void AxisGLWidget::drawAxes()
{
    glLineWidth(2.0f);
    glScalef(6.0f, 6.0f, 6.0f);
    glBegin(GL_LINES);
    qglColor(QColor::fromRgbF(1, 0, 0));  // X axis is red.
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);
    qglColor(QColor::fromRgbF(0, 1, 0));  // Y axis is green.
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
    qglColor(QColor::fromRgbF(0, 0, 1));  // Z axis is blue.
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);
    glEnd();
    // Draw labels
    qglColor(QColor::fromRgbF(1, 0, 0));
    renderText(1, 0, 0, "X", QFont("helvetica", 12, QFont::Bold, true));
    qglColor(QColor::fromRgbF(0, 1, 0));
    renderText(0, 1, 0, "Y", QFont("helvetica", 12, QFont::Bold, true));
    qglColor(QColor::fromRgbF(0, 0, 1));
    renderText(0, 0, 1, "Z", QFont("helvetica", 12, QFont::Bold, true));
}
