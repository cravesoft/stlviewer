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

#ifndef AXISGLWIDGET_H
#define AXISGLWIDGET_H

#include <QtGui/QGroupBox>
#include <QtOpenGL/QGLWidget>

#include "stlfile.h"

class AxisGLWidget;

class AxisGLWidget : public QGLWidget
{

    Q_OBJECT

 public:
    AxisGLWidget(QWidget *parent = 0);
    ~AxisGLWidget();
    void reset();
    void rotateAxis();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

 public slots:
    void setXRotation(const int angle);
    void setYRotation(const int angle);
    void setZRotation(const int angle);

 signals:
    void xRotationChanged(int angle) const;
    void yRotationChanged(int angle) const;
    void zRotationChanged(int angle) const;

 protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

 private:
    void drawAxes();
    int xRot, yRot, zRot;
    QColor black, purple;
};

#endif  // AXISGLWIDGET_H
