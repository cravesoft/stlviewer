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

#include "AxisGLWidget.hpp"
#include <QPainter>
#include <QMatrix4x4>

using namespace stlviewer;

AxisGLWidget::AxisGLWidget(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0x4e, 0x4e, 0x7f));
    setPalette(pal);
}

void AxisGLWidget::reset()
{
    rot = QQuaternion();
    update();
}

QSize AxisGLWidget::minimumSizeHint() const { return QSize(50, 50); }
QSize AxisGLWidget::sizeHint() const { return QSize(100, 100); }

void AxisGLWidget::setRotation(const QQuaternion &rotation)
{
    rot = rotation;
    update();
}

QPointF AxisGLWidget::project(const QVector3D &v) const
{
    QMatrix4x4 m;
    m.rotate(rot);
    QVector3D p = m.map(v);
    float cx = width() / 2.0f;
    float cy = height() / 2.0f;
    float scale = qMin(width(), height()) * 0.35f;
    return QPointF(cx + p.x() * scale, cy - p.y() * scale);
}

void AxisGLWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPointF origin = project(QVector3D(0, 0, 0));
    struct Axis { QVector3D dir; QColor color; QString label; };
    Axis axes[] = {
        { QVector3D(1, 0, 0), Qt::red,   "X" },
        { QVector3D(0, 1, 0), Qt::green, "Y" },
        { QVector3D(0, 0, 1), Qt::blue,  "Z" },
    };
    for (auto &ax : axes)
    {
        QPointF tip = project(ax.dir);
        p.setPen(QPen(ax.color, 2));
        p.drawLine(origin, tip);
        p.setPen(ax.color);
        QFont f("helvetica", 9, QFont::Bold);
        p.setFont(f);
        p.drawText(tip + QPointF(3, 3), ax.label);
    }
}
