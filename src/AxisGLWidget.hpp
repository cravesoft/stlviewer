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

#ifndef _AXISGLWIDGET_HPP
#define _AXISGLWIDGET_HPP

#include <QWidget>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QPainter>

namespace stlviewer
{
    class AxisGLWidget : public QWidget
    {
        Q_OBJECT

        public: explicit AxisGLWidget(QWidget *parent = nullptr);
        public: ~AxisGLWidget() override = default;
        public: void reset();
        public: QSize minimumSizeHint() const override;
        public: QSize sizeHint() const override;

        public slots: void setRotation(const QQuaternion &rot);

        protected: void paintEvent(QPaintEvent *event) override;

        private: QPointF project(const QVector3D &v) const;

        private: QQuaternion rot;
    };
}

#endif
