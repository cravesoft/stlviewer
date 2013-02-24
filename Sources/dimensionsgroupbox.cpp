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
#include <QLabel>
#include <QGridLayout>

#include "dimensionsgroupbox.h"

DimensionsGroupBox::DimensionsGroupBox(QWidget *parent)
    : QGroupBox(tr("Dimensions"), parent)
{

    QGridLayout *layout = new QGridLayout;
    // Write labels and values
    QLabel *label = new QLabel("Min");
    label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(label, 0, 1);
    label = new QLabel("Max");
    label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(label, 0, 2);
    label = new QLabel("Delta");
    label->setAlignment(Qt::AlignHCenter);
    layout->addWidget(label, 0, 3);
    layout->addWidget(new QLabel("X"), 1, 0);
    xMin = new QLabel("");
    xMin->setAlignment(Qt::AlignRight);
    layout->addWidget(xMin, 1, 1);
    xMax = new QLabel("");
    xMax->setAlignment(Qt::AlignRight);
    layout->addWidget(xMax, 1, 2);
    xDelta = new QLabel("");
    xDelta->setAlignment(Qt::AlignRight);
    layout->addWidget(xDelta, 1, 3);
    layout->addWidget(new QLabel("Y"), 2, 0);
    yMin = new QLabel("");
    yMin->setAlignment(Qt::AlignRight);
    layout->addWidget(yMin, 2, 1);
    yMax = new QLabel("");
    yMax->setAlignment(Qt::AlignRight);
    layout->addWidget(yMax, 2, 2);
    yDelta = new QLabel("");
    yDelta->setAlignment(Qt::AlignRight);
    layout->addWidget(yDelta, 2, 3);
    layout->addWidget(new QLabel("Z"), 3, 0);
    zMin = new QLabel("");
    zMin->setAlignment(Qt::AlignRight);
    layout->addWidget(zMin, 3, 1);
    zMax = new QLabel("");
    zMax->setAlignment(Qt::AlignRight);
    layout->addWidget(zMax, 3, 2);
    zDelta = new QLabel("");
    zDelta->setAlignment(Qt::AlignRight);
    layout->addWidget(zDelta, 3, 3);
    // Write units of length
    layout->addWidget(new QLabel("mm"), 1, 4);
    layout->addWidget(new QLabel("mm"), 2, 4);
    layout->addWidget(new QLabel("mm"), 3, 4);
    layout->setColumnMinimumWidth(0, 20);
    layout->setColumnMinimumWidth(1, 50);
    layout->setColumnMinimumWidth(2, 50);
    layout->setColumnMinimumWidth(3, 50);
    setLayout(layout);
}

DimensionsGroupBox::~DimensionsGroupBox() {}

void DimensionsGroupBox::reset()
{
    // Reset x fields
    xMax->setText("");
    xMin->setText("");
    xDelta->setText("");
    // Reset y fields
    yMax->setText("");
    yMin->setText("");
    yDelta->setText("");
    // Reset z fields
    zMax->setText("");
    zMin->setText("");
    zDelta->setText("");
}

void DimensionsGroupBox::setValues(const StlFile::Stats stats)
{
    QString data;
    // Write x values contained in stats
    data.setNum(stats.max.x, 'f', 3);
    xMax->setText(data);
    data.setNum(stats.min.x, 'f', 3);
    xMin->setText(data);
    data.setNum(stats.max.x-stats.min.x, 'f', 3);
    xDelta->setText(data);
    // Write y values contained in stats
    data.setNum(stats.max.y, 'f', 3);
    yMax->setText(data);
    data.setNum(stats.min.y, 'f', 3);
    yMin->setText(data);
    data.setNum(stats.max.y-stats.min.y, 'f', 3);
    yDelta->setText(data);
    // Write z values contained in stats
    data.setNum(stats.max.z, 'f', 3);
    zMax->setText(data);
    data.setNum(stats.min.z, 'f', 3);
    zMin->setText(data);
    data.setNum(stats.max.z-stats.min.z, 'f', 3);
    zDelta->setText(data);
}
