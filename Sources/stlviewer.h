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

#ifndef STLVIEWER_H
#define STLVIEWER_H

#include <QMainWindow>
#include <QGLWidget>
#include <QMdiArea>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QSignalMapper>

#include "glmdichild.h"

class AxisGroupBox;
class DimensionsGroupBox;
class MeshInformationGroupBox;
class PropertiesGroupBox;

class STLViewer : public QMainWindow
{

    Q_OBJECT
 
 public:
    STLViewer(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~STLViewer();

 signals:
    void leftMouseButtonModeChanged(GLWidget::LeftMouseButtonMode mode);

 protected:
    void closeEvent(QCloseEvent *event);

 private slots:
    void newFile();  
    void open();
    void save();
    void saveAs();
    void saveImage();
    void rotate();
    void panning();
    void zoom();
    void unzoom();
    void backView();
    void frontView();
    void leftView();
    void rightView();
    void topView();
    void bottomView();
    void topFrontLeftView();
    void wireframe();
    void about();
    void updateMenus();
    void updateWindowMenu();
    void setMousePressed(Qt::MouseButtons button);
    void setMouseReleased(Qt::MouseButtons button);
    GLMdiChild *createGLMdiChild();
    void setActiveSubWindow(QWidget *window);
    void destroyGLMdiChild();

 private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    // Reads persistent platform-independent application settings
    void readSettings();
    // Writes persistent platform-independent application settings
    void writeSettings();
    GLMdiChild *activeGLMdiChild();
    QMdiSubWindow *findGLMdiChild(const QString &fileName);
    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;
    QMenu *fileMenu;
    QMenu *windowMenu;
    QMenu *viewMenu;
    QMenu *defaultViewsMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *saveImageAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *zoomAct;
    QAction *rotateAct;
    QAction *panningAct;
    QAction *unzoomAct;
    QAction *backViewAct;
    QAction *frontViewAct;
    QAction *leftViewAct;
    QAction *rightViewAct;
    QAction *topViewAct;
    QAction *bottomViewAct;
    QAction *topFrontLeftViewAct;
    QAction *wireframeAct;
    QAction *exitAct;
    QAction *aboutAct;
    QString curDir;
    GLWidget::LeftMouseButtonMode leftMouseButtonMode;
    AxisGroupBox *axisGroupBox;
    DimensionsGroupBox *dimensionsGroupBox;
    MeshInformationGroupBox *meshInformationGroupBox;
    PropertiesGroupBox *propertiesGroupBox;
};

#endif // STLVIEWER_H
