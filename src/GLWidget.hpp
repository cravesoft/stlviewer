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

#ifndef _GLWIDGET_HPP
#define _GLWIDGET_HPP

#include "qt.hpp"
#include "STLFile.hpp"
#include "GeometryEngine.hpp"

namespace stlviewer
{
    class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
    {
        Q_OBJECT

        public: enum LeftMouseButtonMode
        {
            INACTIVE,
            ROTATE,
            PANNING
        };

        public: GLWidget(QWidget *_parent = 0);

        public: ~GLWidget();

        public: void makeObjectFromSTLFile(StlFile &_stlFile);

        public: void deleteObject();

        public: void setDefaultView();

        public: void zoomIn();

        public: void zoomOut();

        public: void setDefaultZoom();

        public: void setBackView();

        public: void setFrontView();

        public: void setLeftView();

        public: void setRightView();

        public: void setTopView();

        public: void setBottomView();

        public: void setTopFrontLeftView();

        public: bool isWireframeModeActivated()
                const { return this->wireframeMode; };

        public: static bool isYAxisReversed()
                { return GLWidget::yAxisReversed; };

        public: void setLeftMouseButtonMode(const GLWidget::LeftMouseButtonMode);

        public: QQuaternion getRotation() const { return this->rot; };

        public slots: void setRotation(const QQuaternion &_rot);

        public slots: void setPosition(const QVector3D &_pos);

        public slots: void setZoomFactor(double _zoomFactor);

        private: float getCameraDistance(qreal fov);

        public slots: void setWireframeMode(bool state);

        public slots: static void setYAxisMode(bool isReversed);

        signals: void rotationChanged(const QQuaternion &_angle) const;

        signals: void positionChanged(const QVector3D &_pos) const;

        signals: void zoomChanged(double zoom);

        protected: QSize minimumSizeHint() const Q_DECL_OVERRIDE;

        protected: QSize sizeHint() const Q_DECL_OVERRIDE;

        protected: void mousePressEvent(QMouseEvent *_event) Q_DECL_OVERRIDE;

        protected: void mouseReleaseEvent(QMouseEvent *_event) Q_DECL_OVERRIDE;

        protected: void mouseMoveEvent(QMouseEvent *_event) Q_DECL_OVERRIDE;

        protected: void wheelEvent(QWheelEvent *_event) Q_DECL_OVERRIDE;

        protected: void initShaders();

        protected: void initializeGL() Q_DECL_OVERRIDE;

        protected: void paintGL() Q_DECL_OVERRIDE;

        protected: void resizeGL(int _w, int _h) Q_DECL_OVERRIDE;

        private: void updateProjection();

        private: void drawAxes();

        private: void initGizmo();

        private: void drawGizmo();

        private: //GLdouble panMatrix[16];

        private: int width;

        private: int height;

        private: GLuint object;

        private: bool wireframeMode;

        private: LeftMouseButtonMode leftMouseButtonMode;

        private: QQuaternion rot;
        //private: QVector3D rot;

        private: QVector3D pos;

        private: QVector3D trans;

        private: double zoomFactor;

        private: double zoomInc;

        private: double defaultZoomFactor;

        private: static bool yAxisReversed;

        private: QPoint lastPos;

        private: QOpenGLShaderProgram program;

        private: GeometryEngine *geometries;

        private: QOpenGLVertexArrayObject gizmoVAO;

        private: QOpenGLBuffer gizmoVBO;

        private: QMatrix4x4 projection;

        private: QVector2D mousePressPosition;

        private: QVector3D rotationAxis;

        private: qreal angularSpeed;

        private: StlFile *stlfile;

        private: qreal aspect;

        private: QMatrix4x4 modelMatrix;
    };
}

#endif
