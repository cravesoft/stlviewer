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

#include "GLWidget.hpp"
#include "STLFile.hpp"

#include <QDebug>
#include <algorithm>
#include <cmath>

using namespace stlviewer;

bool GLWidget::yAxisReversed = false;

GLWidget::GLWidget(QWidget *_parent)
    : QOpenGLWidget(_parent)
    , stlfile(nullptr)
    , width(0)
    , height(0)
    , wireframeMode(false)
    , leftMouseButtonMode(INACTIVE)
    , rot()
    , pos()
    , zoomFactor(1.0)
    , zoomInc(0)
    , defaultZoomFactor(1.0)
    , geometries(0)
{
}

GLWidget::~GLWidget()
{
    // Make sure the context is current when deleting the buffers.
    this->makeCurrent();
    delete this->geometries;
    this->doneCurrent();
}

void GLWidget::makeObjectFromSTLFile(StlFile &_stlfile)
{
    this->stlfile = &_stlfile;
    // geometries is created inside initializeGL(), which Qt calls lazily on
    // first paint.  If the widget hasn't been shown yet, defer the GPU upload
    // to initializeGL(); otherwise upload now.
    if (this->geometries) {
        this->makeCurrent();
        this->geometries->initGeometry(_stlfile);
    }
    StlFile::Stats stats = _stlfile.getStats();
    qDebug() << "max:" << stats.max.x << stats.max.y << stats.max.z;
    qDebug() << "min:" << stats.min.x << stats.min.y << stats.min.z;
    QVector3D trans = QVector3D((stats.max.x + stats.min.x) / 2,
                          (stats.max.y + stats.min.y) / 2,
                          (stats.max.z + stats.min.z) / 2);
    this->pos = QVector3D(0.0, 0.0, 0.0);
    this->defaultZoomFactor = std::max({
        std::abs(stats.max.x - stats.min.x),
        std::abs(stats.max.y - stats.min.y),
        std::abs(stats.max.z - stats.min.z)});
    this->zoomInc = this->defaultZoomFactor / 500;
    this->setDefaultView();
    this->modelMatrix.setToIdentity();
    this->modelMatrix.translate(-trans);
    this->update();
}

void GLWidget::setDefaultView()
{
    setTopFrontLeftView();
    setDefaultZoom();
}

void GLWidget::zoomIn()
{
    this->setZoomFactor(this->zoomFactor - 50 * this->zoomInc);
}

void GLWidget::zoomOut()
{
    this->setZoomFactor(this->zoomFactor + 50 * this->zoomInc);
}

void GLWidget::setDefaultZoom()
{
    this->makeCurrent();
    this->zoomFactor = this->defaultZoomFactor;
    this->update();
}

void GLWidget::setBackView()
{
    this->rot = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, 180.0f);
    emit rotationChanged(this->rot);
    this->update();
}

void GLWidget::setFrontView()
{
    this->rot = QQuaternion();
    emit rotationChanged(this->rot);
    this->update();
}

void GLWidget::setLeftView()
{
    this->rot = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, -90.0f);
    emit rotationChanged(this->rot);
    this->update();
}

void GLWidget::setRightView()
{
    this->rot = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, 90.0f);
    emit rotationChanged(this->rot);
    this->update();
}

void GLWidget::setTopView()
{
    this->rot = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -90.0f);
    emit rotationChanged(this->rot);
    this->update();
}

void GLWidget::setBottomView()
{
    this->rot = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f);
    emit rotationChanged(this->rot);
    this->update();
}

void GLWidget::setTopFrontLeftView()
{
    QQuaternion q1 = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -30.0f);
    QQuaternion q2 = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, 45.0f);
    this->rot = q1 * q2;
    emit rotationChanged(this->rot);
    this->update();
}

void GLWidget::setLeftMouseButtonMode(const GLWidget::LeftMouseButtonMode _mode)
{
    this->leftMouseButtonMode = _mode;
}

void GLWidget::setRotation(const QQuaternion &_rot)
{
    this->rot = _rot;
    emit rotationChanged(_rot);
    this->update();
}

void GLWidget::setPosition(const QVector3D &_pos)
{
    this->pos = _pos;
    emit positionChanged(_pos);
    this->update();
}

void GLWidget::setZoomFactor(double _zoomFactor)
{
    if (_zoomFactor <= 0)
        this->zoomFactor = 1e-3;
    else
        this->zoomFactor = _zoomFactor;
    this->zoomInc = this->zoomFactor / 500;
    emit zoomChanged(this->zoomFactor);
    this->update();
}

void GLWidget::setWireframeMode(bool _state)
{
    this->makeCurrent();
    this->wireframeMode = _state;
    this->update();
}

void GLWidget::setYAxisMode(bool _isReversed)
{
    GLWidget::yAxisReversed = _isReversed;
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLWidget::mousePressEvent(QMouseEvent *_event)
{
    this->lastPos = _event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *)
{
}

void GLWidget::mouseMoveEvent(QMouseEvent *_event)
{
    int dx = _event->x() - this->lastPos.x();
    int dy = _event->y() - this->lastPos.y();

    // Reverse Y-Axis if needed
    if (GLWidget::yAxisReversed)
    {
        dy = -dy;
    }

    if (_event->buttons() & Qt::LeftButton &&
        this->leftMouseButtonMode == PANNING)
    {
        qreal step;
        if (this->aspect < 0)
        {
            step = this->zoomFactor / this->width;
        }
        else
        {
            step = this->zoomFactor / this->height;
        }
        this->setPosition(QVector3D(dx * step, -dy * step, 0.0) + this->pos);
    }
    else if ((_event->buttons() & Qt::LeftButton &&
             this->leftMouseButtonMode == ROTATE) ||
             _event->buttons() & Qt::MiddleButton)
    {
        QQuaternion rot;
        rot = QQuaternion::fromAxisAndAngle(1.0, 0.0, 0.0, dy);
        rot *= QQuaternion::fromAxisAndAngle(0.0, 0.0, 1.0, dx);
        this->setRotation(rot * this->rot);
    }
    else if (_event->buttons() & Qt::RightButton)
    {
        setZoomFactor(this->zoomFactor + dy * this->zoomInc);
    }
    this->lastPos = _event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *_event)
{
    int delta = _event->angleDelta().y();

    setZoomFactor(this->zoomFactor - delta * this->zoomInc);
}

void GLWidget::initShaders()
{
    // Compile vertex shader
    if (!this->program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                               ":/vshader.glsl"))
        qFatal("Failed to compile vertex shader");

    // Compile fragment shader
    if (!this->program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                               ":/fshader.glsl"))
        qFatal("Failed to compile fragment shader");

    // Link shader pipeline
    if (!this->program.link())
        qFatal("Failed to link shader program");

    // Bind shader pipeline for use
    if (!this->program.bind())
        qFatal("Failed to bind shader program");
}

void GLWidget::initializeGL()
{
    this->initializeOpenGLFunctions();

    glClearColor(78.f/255.f, 78.f/255.f, 127.f/255.f, 1.0f);

    this->initShaders();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    this->geometries = new GeometryEngine;
    if (this->stlfile)
        this->geometries->initGeometry(*this->stlfile);

    this->initGizmo();
}

void GLWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->program.bind();

    this->updateProjection();

    // Compute the MVP matrix from mouse input
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(this->pos);
    viewMatrix.rotate(this->rot);
    QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;

    // Send our matrices to the currently bound shader
    this->program.setUniformValue("normalMatrix", modelViewMatrix.normalMatrix());
    this->program.setUniformValue("modelViewMatrix", modelViewMatrix);
    this->program.setUniformValue("projectionMatrix", this->projection);

    if (!this->wireframeMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    glCullFace(GL_BACK);

    this->geometries->drawTriangleGeometry(this->program);

    if (!this->wireframeMode)
    {
        glCullFace(GL_FRONT);
        glPolygonMode(GL_BACK, GL_LINE);
        this->geometries->drawTriangleGeometry(this->program);
        glPolygonMode(GL_BACK, GL_FILL);
        glCullFace(GL_BACK);
    }

    // Draw the world-origin gizmo on top of everything.
    this->drawGizmo();
}

void GLWidget::initGizmo()
{
    // Unit-length axes: origin → (1,0,0), (0,1,0), (0,0,1)
    const QVector3D verts[6] = {
        {0, 0, 0}, {1, 0, 0},   // X
        {0, 0, 0}, {0, 1, 0},   // Y
        {0, 0, 0}, {0, 0, 1},   // Z
    };

    this->gizmoVAO.create();
    QOpenGLVertexArrayObject::Binder binder(&this->gizmoVAO);

    this->gizmoVBO = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    this->gizmoVBO.create();
    this->gizmoVBO.bind();
    this->gizmoVBO.allocate(verts, sizeof(verts));

    int posAttr = this->program.attributeLocation("a_position");
    this->program.enableAttributeArray(posAttr);
    this->program.setAttributeBuffer(posAttr, GL_FLOAT, 0, 3, sizeof(QVector3D));
}

void GLWidget::drawGizmo()
{
    // Scale the unit axes to ~20 % of the model bounding box.
    const float len = static_cast<float>(this->defaultZoomFactor) * 0.2f;

    QMatrix4x4 viewMatrix;
    viewMatrix.translate(this->pos);
    viewMatrix.rotate(this->rot);

    QMatrix4x4 gizmoModel;
    gizmoModel.scale(len);

    this->program.setUniformValue("modelViewMatrix", viewMatrix * gizmoModel);
    this->program.setUniformValue("normalMatrix",
        (viewMatrix * gizmoModel).normalMatrix());
    this->program.setUniformValue("u_flatMode", 1);

    glDisable(GL_DEPTH_TEST);
    glLineWidth(2.0f);

    QOpenGLVertexArrayObject::Binder binder(&this->gizmoVAO);

    const struct { QVector3D color; int first; } axes[3] = {
        { {1.0f, 0.0f, 0.0f}, 0 },   // X  red
        { {0.0f, 0.8f, 0.0f}, 2 },   // Y  green
        { {0.3f, 0.3f, 1.0f}, 4 },   // Z  blue
    };
    for (const auto &ax : axes)
    {
        this->program.setUniformValue("u_flatColor", ax.color);
        glDrawArrays(GL_LINES, ax.first, 2);
    }

    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0f);
    this->program.setUniformValue("u_flatMode", 0);
}

void GLWidget::resizeGL(int _w, int _h)
{
    this->width = _w;
    this->height = _h;
    glViewport(0, 0, this->width, this->height);

    // Calculate aspect ratio
    this->aspect = qreal(this->width) / qreal(this->height ? this->height : 1);

    this->updateProjection();
}

void GLWidget::updateProjection()
{
    // Reset projection
    this->projection.setToIdentity();

    const qreal zNear = -7000.0;
    const qreal zFar = 7000.0;

    // Set orthographic projection
    this->projection.ortho(-this->zoomFactor * aspect, this->zoomFactor * aspect,
                           -this->zoomFactor,  this->zoomFactor,
                           zNear, zFar);
}
