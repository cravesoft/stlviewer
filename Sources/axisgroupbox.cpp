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

#include <QGLWidget>
#include <QGridLayout>
#include <QLabel>

#include "axisgroupbox.h"
#include "axisglwidget.h"

AxisGroupBox::AxisGroupBox(QWidget *parent)
    : QGroupBox(tr("World Coordinate System"), parent)
{

    QGridLayout *layout2 = new QGridLayout;
    // Write labels and values
    layout2->addWidget(new QLabel("X:"), 0, 0);
    xRot = new QLabel("");
    xRot->setAlignment(Qt::AlignLeft);
    layout2->addWidget(xRot, 0, 1);
    layout2->addWidget(new QLabel("Y:"), 1, 0);
    yRot = new QLabel("");
    yRot->setAlignment(Qt::AlignLeft);
    layout2->addWidget(yRot, 1, 1);
    layout2->addWidget(new QLabel("Z:"), 2, 0);
    zRot = new QLabel("");
    zRot->setAlignment(Qt::AlignLeft);
    layout2->addWidget(zRot, 2, 1);

    axisGLWidget = new AxisGLWidget(this);
    QGridLayout *layout1 = new QGridLayout;
    QWidget *wi = new QWidget;
    wi->setLayout(layout2);
    layout1->addWidget(wi, 0, 0);
    layout1->addWidget(axisGLWidget, 0, 1);
    setLayout(layout1);
}

AxisGroupBox::~AxisGroupBox() {}

void AxisGroupBox::reset()
{
    // Reset values
    xRot->setText("");
    yRot->setText("");
    zRot->setText("");
    axisGLWidget->reset();
}

void AxisGroupBox::setXRotation(const int angle)
{
    QString data;
    // Display angle value
    data.setNum(angle / 16);
    xRot->setText(data + "°");
    // Update angle value
    axisGLWidget->setXRotation(angle);
}

void AxisGroupBox::setYRotation(const int angle)
{
    QString data;
    // Display angle value
    data.setNum(angle / 16);
    yRot->setText(data + "°");
    // Update angle value
    axisGLWidget->setYRotation(angle);
}

void AxisGroupBox::setZRotation(const int angle)
{
    QString data;
    // Display angle value
    data.setNum(angle / 16);
    zRot->setText(data + "°");
    // Update angle value
    axisGLWidget->setZRotation(angle);
}
