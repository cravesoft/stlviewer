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

#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

#include "glwidget.h"
#include "stlfile.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    object = 0;
    xRot = yRot = zRot = 0;
    xPos= yPos= zPos= 0;
    xTrans= yTrans= zTrans= 0;
    defaultZoomFactor = zoomFactor = 1.0;
    zoomInc = 0;
    leftMouseButtonMode = INACTIVE;
    wireframeMode = false;
    grey = QColor::fromRgbF(0.6, 0.6, 0.6);
    black = QColor::fromRgbF(0.0, 0.0, 0.0);
    purple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(object, 1);
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLWidget::makeObjectFromStlFile(StlFile *stlfile)
{
    makeCurrent();
    object = glGenLists(1);
    glNewList(object, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < stlfile->getStats().numFacets; ++i)
    {
        glNormal3d(stlfile->getFacets()[i].normal.x,
                   stlfile->getFacets()[i].normal.y,
                   stlfile->getFacets()[i].normal.z);
        triangle(stlfile->getFacets()[i].vector[0].x,
                 stlfile->getFacets()[i].vector[0].y,
                 stlfile->getFacets()[i].vector[0].z,
                 stlfile->getFacets()[i].vector[1].x,
                 stlfile->getFacets()[i].vector[1].y,
                 stlfile->getFacets()[i].vector[1].z,
                 stlfile->getFacets()[i].vector[2].x,
                 stlfile->getFacets()[i].vector[2].y,
                 stlfile->getFacets()[i].vector[2].z);
    }
    glEnd();
    glEndList();
    xPos = (stlfile->getStats().max.x+stlfile->getStats().min.x)/2;
    yPos = (stlfile->getStats().max.y+stlfile->getStats().min.y)/2;
    zPos = (stlfile->getStats().max.z+stlfile->getStats().min.z)/2;
    defaultZoomFactor = qMax(qMax(
        qAbs(stlfile->getStats().max.x-stlfile->getStats().min.x),
        qAbs(stlfile->getStats().max.y-stlfile->getStats().min.y)),
        qAbs(stlfile->getStats().max.z-stlfile->getStats().min.z));
    zoomInc = defaultZoomFactor/1000;
    setDefaultView();
}

void GLWidget::deleteObject()
{
    glDeleteLists(object, 1);
    updateGL();
}

void GLWidget::updateCursor()
{
    QCursor cursor = this->cursor();
    cursor.setShape(Qt::ArrowCursor);
    if(leftMouseButtonMode == ROTATE)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    else if(leftMouseButtonMode == PANNING)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    QWidget::setCursor(cursor);
}

void GLWidget::setDefaultView()
{
    setTopFrontLeftView();
    unzoom();
    /*glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, panMatrix);
    glPopMatrix();*/
}

void GLWidget::zoom()
{
    setZoom(zoomFactor - 50*zoomInc);
}

void GLWidget::unzoom()
{
    makeCurrent();
    xTrans = yTrans = zTrans = 0;
    zoomFactor = defaultZoomFactor;
    updateGL();
}

void GLWidget::setBackView()
{
    makeCurrent();
    setXRotation(270*16);
    setYRotation(0);
    setZRotation(180*16);
}

void GLWidget::setFrontView()
{
    makeCurrent();
    setXRotation(270*16);
    setYRotation(0);
    setZRotation(0);
}

void GLWidget::setLeftView()
{
    makeCurrent();
    setXRotation(270*16);
    setYRotation(0);
    setZRotation(90*16);
}

void GLWidget::setRightView()
{
    makeCurrent();
    setXRotation(270*16);
    setYRotation(0);
    setZRotation(270*16);
}

void GLWidget::setTopView()
{
    makeCurrent();
    setXRotation(0);
    setYRotation(0);
    setZRotation(0);
}

void GLWidget::setBottomView()
{
    makeCurrent();
    setXRotation(0);
    setYRotation(180*16);
    setZRotation(0);
}

void GLWidget::setTopFrontLeftView()
{
    makeCurrent();
    setXRotation(290*16);
    setYRotation(0);
    setZRotation(30*16);
}

void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if(angle != xRot)
    {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if(angle != yRot)
    {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if(angle != zRot)
    {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setXTranslation(const float distance)
{
    if(distance != xTrans)
    {
        xTrans = distance;
        emit xTranslationChanged(distance);
        updateGL();
    }
}

void GLWidget::setYTranslation(const float distance)
{
    if(distance != yTrans)
    {
        yTrans = distance;
        emit yTranslationChanged(distance);
        updateGL();
    }
}

void GLWidget::setZoom(const float zoom)
{
    if(zoom != zoomFactor)
    {
        zoomFactor = zoom;
        if(zoom <= 0)
        {
            zoomFactor = 0.001;
        }
        zoomInc = zoomFactor/1000;
        emit zoomChanged(zoom);
        updateGL();
    }
}

void GLWidget::setLeftMouseButtonMode(const GLWidget::LeftMouseButtonMode mode)
{
    leftMouseButtonMode = mode;
    updateCursor();
}

void GLWidget::setWireframeMode(const bool state)
{
    makeCurrent();
    wireframeMode = state;
    updateGL();
}

void GLWidget::initializeGL()
{
    qglClearColor(purple.dark());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    // Enable color tracking
    glEnable(GL_COLOR_MATERIAL);
    // Set Material properties to follow glColor values
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void GLWidget::paintGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Adjust clipping box
    if(width <= height)
    {
        glOrtho(-zoomFactor, zoomFactor, -zoomFactor*height/width,
            zoomFactor*height/width, -zoomFactor*5000.0f, zoomFactor*5000.0f);
    }
    else
    {
        glOrtho(-zoomFactor*width/height, zoomFactor*width/height,
            -zoomFactor, zoomFactor, -zoomFactor*5000.0f, zoomFactor*5000.0f);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glTranslated(-xTrans, -yTrans, -zTrans);
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
    //glMultMatrixf(panMatrix);
    //glLoadMatrixd(panMatrix);
    //glTranslated(-xPos-xTrans, -yPos-yTrans, -zPos-zTrans);
    glTranslated(-xPos, -yPos, -zPos);

    if(!wireframeMode)
    {
        //glPolygonMode(GL_BACK, GL_FILL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    glCullFace(GL_BACK);
    qglColor(grey);
    glCallList(object);

    if(!wireframeMode)
    {
        glCullFace(GL_FRONT);
        qglColor(black);
        glPolygonMode(GL_BACK, GL_LINE);
        glCallList(object);
        glPolygonMode(GL_BACK, GL_FILL);
        glCullFace(GL_BACK);
    }

    drawAxes();
}

void GLWidget::resizeGL(int width, int height)
{
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Adjust clipping box
    if(width <= height)
    {
        glOrtho(-zoomFactor, zoomFactor, -zoomFactor*height/width,
            zoomFactor*height/width, -zoomFactor*5000.0f, zoomFactor*5000.0f);
    }
    else
    {
        glOrtho(-zoomFactor*width/height, zoomFactor*width/height,
            -zoomFactor, zoomFactor, -zoomFactor*5000.0f, zoomFactor*5000.0f);
    }
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    QCursor cursor = this->cursor();
    if(leftMouseButtonMode == ROTATE)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    if(leftMouseButtonMode == PANNING)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    if(event->buttons() & Qt::RightButton)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    else if(event->buttons() & Qt::MidButton)
    {
        cursor.setShape(Qt::SizeAllCursor);
    }
    QWidget::setCursor(cursor);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    updateCursor();
#if 0
    if  (
        event->button() & Qt::MidButton ||
        (event->button() & Qt::LeftButton && leftMouseButtonMode == PANNING)
        )
    {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      glTranslated(-xTrans, -yTrans, -zTrans);
      glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
      glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
      glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
      glMultMatrixf(panMatrix);
      glGetFloatv(GL_MODELVIEW_MATRIX, panMatrix);
      glPopMatrix();
      xRot = yRot = zRot = 0;
      xTrans = yTrans = zTrans = 0;
      updateGL();
    }
#endif
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if  (
        (event->buttons() & Qt::LeftButton && leftMouseButtonMode == PANNING) ||
        event->buttons() & Qt::MidButton
        )
    {
        if(width <= height)
        {
            setXTranslation(xTrans - dx*2*zoomFactor/width);
            setYTranslation(yTrans + dy*2*zoomFactor/width);
        }
        else
        {
            setXTranslation(xTrans - dx*2*zoomFactor/height);
            setYTranslation(yTrans + dy*2*zoomFactor/height);
        }
    }
    else if (
            (event->buttons() & Qt::LeftButton && leftMouseButtonMode == ROTATE) ||
            event->buttons() & Qt::RightButton
            )
    {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot - 8 * dx);
    }
    lastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    setZoom(zoomFactor - delta*zoomInc);
}

void GLWidget::triangle(GLdouble x1, GLdouble y1, GLdouble z1,
                        GLdouble x2, GLdouble y2, GLdouble z2,
                        GLdouble x3, GLdouble y3, GLdouble z3)
{
    glVertex3d(x1, y1, z1);
    glVertex3d(x2, y2, z2);
    glVertex3d(x3, y3, z3);
}

void GLWidget::normalizeAngle(int *angle)
{
    while(*angle < 0)
    {
        *angle += 360 * 16;
    }
    while(*angle > 360 * 16)
    {
        *angle -= 360 * 16;
    }
}

void GLWidget::drawAxes()
{
    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glLineWidth(2.0);
    glScalef(zoomFactor/6, zoomFactor/6, zoomFactor/6);
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
    renderText(1.0, 0.0, 0.0, "X", QFont("helvetica", 12, QFont::Bold, true));
    qglColor(QColor::fromRgbF(0, 1, 0));
    renderText(0.0, 1.0, 0.0, "Y", QFont("helvetica", 12, QFont::Bold, true));
    qglColor(QColor::fromRgbF(0, 0, 1));
    renderText(0.0, 0.0, 1.0, "Z", QFont("helvetica", 12, QFont::Bold, true));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}
