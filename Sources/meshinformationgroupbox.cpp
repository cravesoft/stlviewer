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

#include "meshinformationgroupbox.h"

MeshInformationGroupBox::MeshInformationGroupBox(QWidget *parent)
    : QGroupBox(tr("Mesh Information"), parent)
{
    QGridLayout *layout = new QGridLayout;
    // Write labels and values
    layout->addWidget(new QLabel("# Facets:"), 0, 0);
    numFacets = new QLabel("");
    numFacets->setAlignment(Qt::AlignRight);
    layout->addWidget(numFacets, 0, 1);
    layout->addWidget(new QLabel("# Points:"), 1, 0);
    numPoints = new QLabel("");
    numPoints->setAlignment(Qt::AlignRight);
    layout->addWidget(numPoints, 1, 1);
    setLayout(layout);
}

MeshInformationGroupBox::~MeshInformationGroupBox() {}

void MeshInformationGroupBox::reset()
{
    // Reset values
    numFacets->setText("");
    numPoints->setText("");
}

void MeshInformationGroupBox::setValues(const StlFile::Stats stats)
{
    QString data;
    // Write values contained in stats
    data.setNum(stats.numFacets);
    numFacets->setText(data);
    data.setNum(stats.numPoints);
    numPoints->setText(data);
}
