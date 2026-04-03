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

#include <QGridLayout>
#include <QLabel>
#include <QQuaternion>

#include "AxisGroupBox.hpp"
#include "AxisGLWidget.hpp"

AxisGroupBox::AxisGroupBox(QWidget *parent)
    : QGroupBox(tr("World Coordinate System"), parent)
{

    QGridLayout *labelsLayout = new QGridLayout;
    labelsLayout->addWidget(new QLabel("X:"), 0, 0);
    xRot = new QLabel("");
    xRot->setAlignment(Qt::AlignLeft);
    labelsLayout->addWidget(xRot, 0, 1);
    labelsLayout->addWidget(new QLabel("Y:"), 1, 0);
    yRot = new QLabel("");
    yRot->setAlignment(Qt::AlignLeft);
    labelsLayout->addWidget(yRot, 1, 1);
    labelsLayout->addWidget(new QLabel("Z:"), 2, 0);
    zRot = new QLabel("");
    zRot->setAlignment(Qt::AlignLeft);
    labelsLayout->addWidget(zRot, 2, 1);

    axisGLWidget = new stlviewer::AxisGLWidget(this);
    QWidget *labelsWidget = new QWidget;
    labelsWidget->setLayout(labelsLayout);
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(labelsWidget, 0, 0);
    layout->addWidget(axisGLWidget, 0, 1);
    setLayout(layout);
}

AxisGroupBox::~AxisGroupBox() {}

void AxisGroupBox::reset()
{
    xRot->setText("");
    yRot->setText("");
    zRot->setText("");
    axisGLWidget->reset();
}

void AxisGroupBox::setRotation(const QQuaternion &rot)
{
    QVector3D euler = rot.toEulerAngles();
    const QString deg = QString::fromUtf8("\u00B0");
    xRot->setText(QString::number(qRound(euler.x())) + deg);
    yRot->setText(QString::number(qRound(euler.y())) + deg);
    zRot->setText(QString::number(qRound(euler.z())) + deg);
    axisGLWidget->setRotation(rot);
}
