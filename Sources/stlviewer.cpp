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

#include <QSignalMapper>
#include <QCloseEvent>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QSettings>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QAction>

#include <iostream>
#include <fstream>

#include "stlviewer.h"
#include "axisgroupbox.h"
#include "dimensionsgroupbox.h"
#include "meshinformationgroupbox.h"
#include "propertiesgroupbox.h"
#include "settingsdialog.h"

STLViewer::STLViewer(QWidget *parent, Qt::WindowFlags flags)
    :   DocumentWindow(parent, flags)
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)),
        this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)),
        this, SLOT(setActiveSubWindow(QWidget *)));
    settingsDialog = new SettingsDialog(this);
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();
    updateMenus();
    // Read persistent application settings
    readSettings();
    // Deactivate the left mouse button when manipulating objects
    leftMouseButtonMode = GLWidget::INACTIVE;
    setWindowTitle(tr("STLViewer"));
    setUnifiedTitleAndToolBarOnMac(true);
}

STLViewer::~STLViewer() {}

void STLViewer::openFile(const QString& path)
{
    QMdiSubWindow *existing = findGLMdiChild(path);
    if(existing)
    {
        mdiArea->setActiveSubWindow(existing);
    }
    else
    {
        GLMdiChild *child = createGLMdiChild();
        if(child->loadFile(path))
        {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        }
        else
        {
            setActiveSubWindow(child);
            mdiArea->closeActiveSubWindow();
            //child->close();
        }
    }
}

void STLViewer::initialize()
{
    QStringList pathList;
    for(int i = 1; i < QCoreApplication::arguments().size(); i++)
    {
        pathList.append(QCoreApplication::arguments().at(i));
    }
    openFiles(pathList);
}

bool STLViewer::openFiles(const QStringList& pathList)
{
    bool success = true;
    for(int i = 0; i < pathList.size() && i < 32; ++i)
    {
        openFile(pathList.at(i));
    }
    return success;
}

void STLViewer::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if(activeGLMdiChild())
    {
        event->ignore();
    }
    else 
    {
        writeSettings();
        event->accept();
    }
}

void STLViewer::newFile()
{
    GLMdiChild *child = createGLMdiChild();
    child->newFile();
    child->show();
    // Reset all informations
    axisGroupBox->reset();
    dimensionsGroupBox->reset();
    meshInformationGroupBox->reset();
    propertiesGroupBox->reset();
}

void STLViewer::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a file"),
        curDir, tr("STL Files (*.stl);;All Files (*.*)"));
    if(!fileName.isEmpty())
    {
        curDir = QFileInfo(fileName).filePath();
        openFile(fileName);
    }
}

void STLViewer::save()
{
    if(activeGLMdiChild() && activeGLMdiChild()->save())
    {
        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void STLViewer::saveAs()
{
    if(activeGLMdiChild() && activeGLMdiChild()->saveAs())
    {
        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void STLViewer::saveImage()
{
    if(activeGLMdiChild() && activeGLMdiChild()->saveImage())
    {
        statusBar()->showMessage(tr("Image saved"), 2000);
    }
}

void STLViewer::showSettingsDialog()
{
    settingsDialog->exec(GLWidget::isYAxisReversed());
    if(settingsDialog->result() == QDialog::Accepted)
    {
        GLWidget::setYAxisMode(settingsDialog->isYAxisReversed());
    }
}

void STLViewer::rotate()
{
    if(rotateAct->isChecked())
    {
        panningAct->setChecked(false);
        leftMouseButtonMode = GLWidget::ROTATE;
    }
    else
    {
        leftMouseButtonMode = GLWidget::INACTIVE;
    }
    emit leftMouseButtonModeChanged(leftMouseButtonMode);
}

void STLViewer::panning()
{
    if(panningAct->isChecked())
    {
        rotateAct->setChecked(false);
        leftMouseButtonMode = GLWidget::PANNING;
    }
    else
    {
        leftMouseButtonMode = GLWidget::INACTIVE;
    }
    emit leftMouseButtonModeChanged(leftMouseButtonMode);
}

void STLViewer::wireframe()
{
    activeGLMdiChild()->setWireframeMode(wireframeAct->isChecked());
    //emit wireframeStatusChanged(wireframeAct->isChecked());
}

void STLViewer::zoomIn()
{
    activeGLMdiChild()->zoomIn();
}

void STLViewer::zoomOut()
{
    activeGLMdiChild()->zoomOut();
}

void STLViewer::zoomDefault()
{
    activeGLMdiChild()->setDefaultZoom();
}

void STLViewer::backView()
{
    activeGLMdiChild()->setBackView();
}

void STLViewer::frontView()
{
    activeGLMdiChild()->setFrontView();
}

void STLViewer::leftView()
{
    activeGLMdiChild()->setLeftView();
}

void STLViewer::rightView()
{
    activeGLMdiChild()->setRightView();
}

void STLViewer::topView()
{
    activeGLMdiChild()->setTopView();
}

void STLViewer::bottomView()
{
    activeGLMdiChild()->setBottomView();
}

void STLViewer::topFrontLeftView()
{
    activeGLMdiChild()->setTopFrontLeftView();
}

void STLViewer::about()
{
    QMessageBox::about(this, tr("About STLViewer"), tr(
        "<p align=\"center\">STLViewer</p>"
        "<p align=\"center\">v%1</p>"
        "<p align=\"center\">Copyright (c) 2009 Olivier Crave</p>"
        "<p align=\"center\">Web: <a href=\"http://www.cravesoft.com\">"
        "www.cravesoft.com</a></p>"
        "<p align=\"center\">Mail: <a href=\"mailto:cravesoft@gmail.com\">"
        "cravesoft@gmail.com</a></p>"
        ).arg(QCoreApplication::applicationVersion()));
}

void STLViewer::updateMenus() {
    bool hasGLMdiChild = (activeGLMdiChild() != 0);
    if(hasGLMdiChild && !activeGLMdiChild()->isUntitled)
    {
        saveAct->setEnabled(true);
        saveAsAct->setEnabled(true);
    }
    else
    {
        saveAct->setEnabled(false);
        saveAsAct->setEnabled(false);
    }
    saveImageAct->setEnabled(hasGLMdiChild);
    closeAct->setEnabled(hasGLMdiChild);
    closeAllAct->setEnabled(hasGLMdiChild);
    zoomInAct->setEnabled(hasGLMdiChild);
    rotateAct->setEnabled(hasGLMdiChild);
    panningAct->setEnabled(hasGLMdiChild);
    zoomOutAct->setEnabled(hasGLMdiChild);
    zoomDefaultAct->setEnabled(hasGLMdiChild);
    wireframeAct->setEnabled(hasGLMdiChild);
    if(hasGLMdiChild)
    {
        wireframeAct->setChecked(activeGLMdiChild()->isWireframeModeActivated());
    }
    else
    {
        wireframeAct->setChecked(false);
    }
    backViewAct->setEnabled(hasGLMdiChild);
    frontViewAct->setEnabled(hasGLMdiChild);
    leftViewAct->setEnabled(hasGLMdiChild);
    rightViewAct->setEnabled(hasGLMdiChild);
    topViewAct->setEnabled(hasGLMdiChild);
    bottomViewAct->setEnabled(hasGLMdiChild);
    topFrontLeftViewAct->setEnabled(hasGLMdiChild);
    tileAct->setEnabled(hasGLMdiChild);
    cascadeAct->setEnabled(hasGLMdiChild);
    nextAct->setEnabled(hasGLMdiChild);
    previousAct->setEnabled(hasGLMdiChild);
    separatorAct->setVisible(hasGLMdiChild);
    if(hasGLMdiChild && !activeGLMdiChild()->isUntitled)
    {
        axisGroupBox->setXRotation(activeGLMdiChild()->getXRot());
        axisGroupBox->setYRotation(activeGLMdiChild()->getYRot());
        axisGroupBox->setZRotation(activeGLMdiChild()->getZRot());
        dimensionsGroupBox->setValues(activeGLMdiChild()->getStats());
        meshInformationGroupBox->setValues(activeGLMdiChild()->getStats());
        propertiesGroupBox->setValues(activeGLMdiChild()->getStats());
    }
    else
    {
        axisGroupBox->reset();
        dimensionsGroupBox->reset();
        meshInformationGroupBox->reset();
        propertiesGroupBox->reset();
    }
}

void STLViewer::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);
    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());
    for(int i = 0; i < windows.size(); ++i)
    {
        GLMdiChild *child = qobject_cast<GLMdiChild *>(windows.at(i)->widget());
        QString text;
        if(i < 9)
        {
            text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        }
        else
        {
            text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeGLMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

void STLViewer::setMousePressed(Qt::MouseButtons button)
{
    if(button & Qt::RightButton)
    {
        rotateAct->setChecked(true);
    }
    else if(button & Qt::MidButton)
    {
        panningAct->setChecked(true);
    }
}

void STLViewer::setMouseReleased(Qt::MouseButtons button)
{
    if(button & Qt::RightButton)
    {
        if(leftMouseButtonMode != GLWidget::ROTATE)
        {
            rotateAct->setChecked(false);
        }
    }
    else if(button & Qt::MidButton)
    {
        if(leftMouseButtonMode != GLWidget::PANNING)
        {
            panningAct->setChecked(false);
        }
    }
}

GLMdiChild *STLViewer::createGLMdiChild()
{
    GLMdiChild *child = new GLMdiChild;
    mdiArea->addSubWindow(child);
    child->setLeftMouseButtonMode(leftMouseButtonMode);
    connect(child, SIGNAL(mouseButtonPressed(Qt::MouseButtons)), this,
        SLOT(setMousePressed(Qt::MouseButtons)));
    connect(child, SIGNAL(mouseButtonReleased(Qt::MouseButtons)), this,
        SLOT(setMouseReleased(Qt::MouseButtons)));
    connect(this,
        SIGNAL(leftMouseButtonModeChanged(GLWidget::LeftMouseButtonMode)), child,
        SLOT(setLeftMouseButtonMode(GLWidget::LeftMouseButtonMode)));
    connect(child, SIGNAL(destroyed()), this, SLOT(destroyGLMdiChild()));
    connect(child, SIGNAL(xRotationChanged(const int)), axisGroupBox,
        SLOT(setXRotation(const int)));
    connect(child, SIGNAL(yRotationChanged(const int)), axisGroupBox,
        SLOT(setYRotation(const int)));
    connect(child, SIGNAL(zRotationChanged(const int)), axisGroupBox,
        SLOT(setZRotation(const int)));
    return child;
}

void STLViewer::setActiveSubWindow(QWidget *window)
{
    if(!window)
    {
        return;
    }
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void STLViewer::destroyGLMdiChild()
{
    if(activeGLMdiChild() == 0)
    {
        panningAct->setChecked(false);
        rotateAct->setChecked(false);
        leftMouseButtonMode = GLWidget::INACTIVE;
        emit leftMouseButtonModeChanged(leftMouseButtonMode);
    }
}

void STLViewer::createActions()
{
    newAct = new QAction(QIcon(":STLViewer/Images/page_white.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":STLViewer/Images/folder.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":STLViewer/Images/disk.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    saveImageAct = new QAction(tr("Save Image..."), this);
    saveImageAct->setShortcut(tr("Ctrl+I"));
    saveImageAct->setStatusTip(tr("Save the current view to disk"));
    connect(saveImageAct, SIGNAL(triggered()), this, SLOT(saveImage()));

    showSettingsDialogAct = new QAction(tr("&Settings"), this);
    showSettingsDialogAct->setShortcut(tr("Ctrl+P"));
    showSettingsDialogAct->setStatusTip(tr("Show settings"));
    connect(showSettingsDialogAct, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));

    closeAct = new QAction(tr("Cl&ose"), this);
    //closeAct->setShortcut(tr("Ctrl+W"));
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous window"));
    connect(previousAct, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    rotateAct = new QAction(QIcon(":STLViewer/Images/arrow_rotate_clockwise.png"), tr("&Rotate"), this);
    rotateAct->setShortcut(tr("R"));
    rotateAct->setStatusTip(tr("Rotate the object"));
    rotateAct->setCheckable(true);
    connect(rotateAct, SIGNAL(triggered()), this, SLOT(rotate()));
    rotateAct->setChecked(false);
      
    panningAct = new QAction(QIcon(":STLViewer/Images/arrow_out.png"), tr("&Pan"), this);
    panningAct->setShortcut(tr("P"));
    panningAct->setStatusTip(tr("Drag the object around"));
    panningAct->setCheckable(true);
    connect(panningAct, SIGNAL(triggered()), this, SLOT(panning()));
    panningAct->setChecked(false);

    zoomInAct = new QAction(QIcon(":STLViewer/Images/magnifier_zoom_in.png"), tr("&Zoom In"), this);
    zoomInAct->setShortcut(tr("+"));
    zoomInAct->setStatusTip(tr("Zoom in"));
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(QIcon(":STLViewer/Images/magnifier_zoom_out.png"),
        tr("&Zoom Out"), this);
    zoomOutAct->setShortcut(tr("-"));
    zoomOutAct->setStatusTip(tr("Zoom out"));
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    zoomDefaultAct = new QAction(QIcon(":STLViewer/Images/magnifier_zoom_default.png"),
        tr("&Default Zoom"), this);
    zoomDefaultAct->setShortcut(tr("0"));
    zoomDefaultAct->setStatusTip(tr("Set default zoom"));
    connect(zoomDefaultAct, SIGNAL(triggered()), this, SLOT(zoomDefault()));

    backViewAct = new QAction(QIcon(":STLViewer/Images/back_view.png"),
        tr("&Back View"), this);
    backViewAct->setStatusTip(tr("Back view"));
    connect(backViewAct, SIGNAL(triggered()), this, SLOT(backView()));

    frontViewAct = new QAction(QIcon(":STLViewer/Images/front_view.png"),
        tr("&Front View"), this);
    frontViewAct->setStatusTip(tr("Front view"));
    connect(frontViewAct, SIGNAL(triggered()), this, SLOT(frontView()));

    leftViewAct = new QAction(QIcon(":STLViewer/Images/left_view.png"),
        tr("&Left View"), this);
    leftViewAct->setStatusTip(tr("Left view"));
    connect(leftViewAct, SIGNAL(triggered()), this, SLOT(leftView()));

    rightViewAct = new QAction(QIcon(":STLViewer/Images/right_view.png"),
        tr("&Right View"), this);
    rightViewAct->setStatusTip(tr("Right view"));
    connect(rightViewAct, SIGNAL(triggered()), this, SLOT(rightView()));

    topViewAct = new QAction(QIcon(":STLViewer/Images/top_view.png"),
        tr("&Top View"), this);
    topViewAct->setStatusTip(tr("Top view"));
    connect(topViewAct, SIGNAL(triggered()), this, SLOT(topView()));

    bottomViewAct = new QAction(QIcon(":STLViewer/Images/bottom_view.png"),
        tr("&Bottom View"), this);
    bottomViewAct->setStatusTip(tr("Bottom view"));
    connect(bottomViewAct, SIGNAL(triggered()), this, SLOT(bottomView()));

    topFrontLeftViewAct = new QAction(QIcon(":STLViewer/Images/top_front_left_view.png"),
        tr("&Top Front Left View"), this);
    topFrontLeftViewAct->setStatusTip(tr("Top Front Left view"));
    connect(topFrontLeftViewAct, SIGNAL(triggered()), this, SLOT(topFrontLeftView()));

    wireframeAct = new QAction(QIcon(":STLViewer/Images/wireframe.png"),
                               tr("&Wireframe"), this);
    wireframeAct->setShortcut(tr("W"));
    wireframeAct->setStatusTip(tr("Wireframe view"));
    wireframeAct->setCheckable(true);
    connect(wireframeAct, SIGNAL(triggered()), this, SLOT(wireframe()));
    wireframeAct->setChecked(false);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("About STLViewer"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void STLViewer::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(saveImageAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(rotateAct);
    viewMenu->addAction(panningAct);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(zoomDefaultAct);
    viewMenu->addAction(wireframeAct);

    defaultViewsMenu = viewMenu->addMenu(tr("&Default Views"));
    defaultViewsMenu->addAction(backViewAct);
    defaultViewsMenu->addAction(frontViewAct);
    defaultViewsMenu->addAction(leftViewAct);
    defaultViewsMenu->addAction(rightViewAct);
    defaultViewsMenu->addAction(topViewAct);
    defaultViewsMenu->addAction(bottomViewAct);
    defaultViewsMenu->addAction(topFrontLeftViewAct);

    viewMenu->addSeparator();

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(showSettingsDialogAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void STLViewer::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    viewToolBar = addToolBar(tr("View"));
    viewToolBar->addAction(rotateAct);
    viewToolBar->addAction(panningAct);
    viewToolBar->addAction(zoomInAct);
    viewToolBar->addAction(zoomOutAct);
    viewToolBar->addAction(zoomDefaultAct);
    viewToolBar->addAction(wireframeAct);
    viewToolBar->addAction(backViewAct);
    viewToolBar->addAction(frontViewAct);
    viewToolBar->addAction(leftViewAct);
    viewToolBar->addAction(rightViewAct);
    viewToolBar->addAction(topViewAct);
    viewToolBar->addAction(bottomViewAct);
    viewToolBar->addAction(topFrontLeftViewAct);

#if 0
    toolsToolBar = addToolBar(tr("Settings"));
    toolsToolBar->addAction(showSettingsDialogAct);
#endif
}

void STLViewer::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void STLViewer::createDockWindows()
{
    // Create a DockWidget named "Informations"
    QDockWidget *dock = new QDockWidget(tr("Model Informations"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    // Create one GroupBox for each type of data
    dimensionsGroupBox = new DimensionsGroupBox(this);
    meshInformationGroupBox = new MeshInformationGroupBox(this);
    propertiesGroupBox = new PropertiesGroupBox(this);
    // Create a layout inside a widget to display all GroupBoxes in one layout
    QWidget *wi = new QWidget;
    wi->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                  QSizePolicy::Fixed));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(dimensionsGroupBox);
    layout->addWidget(meshInformationGroupBox);
    layout->addWidget(propertiesGroupBox);
    wi->setLayout(layout);
    // Embed the widget that contains all GroupBoxes into the DockWidget
    dock->setWidget(wi);
    // Add the DockWidget at the right side of the main layout
    addDockWidget(Qt::RightDockWidgetArea, dock);
    // Add a button in the view menu to show/hide the DockWidget
    viewMenu->addAction(dock->toggleViewAction());

    // Create a DockWidget named "View Informations"
    dock = new QDockWidget(tr("View Informations"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    // Create one GroupBox to display the axis
    axisGroupBox = new AxisGroupBox(this);
    // Create a layout inside a widget to display all GroupBoxes in one layout
    wi = new QWidget;
    wi->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                  QSizePolicy::Fixed));
    layout = new QVBoxLayout;
    layout->addWidget(axisGroupBox);
    wi->setLayout(layout);
    // Embed the widget that contains all GroupBoxes into the DockWidget
    dock->setWidget(wi);
    // Add the DockWidget at the right side of the main layout
    addDockWidget(Qt::RightDockWidgetArea, dock);
    // Add a button in the view menu to show/hide the DockWidget
    viewMenu->addAction(dock->toggleViewAction());
}

void STLViewer::readSettings()
{
    QSettings settings("Cravesoft", "STLViewer");
    curDir = settings.value("dir", QString()).toString();
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    GLWidget::setYAxisMode(settings.value("yAxisReversed", false).toBool());
    resize(size);
    move(pos);
}

void STLViewer::writeSettings()
{
    QSettings settings("Cravesoft", "STLViewer");
    settings.setValue("dir", curDir);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("yAxisReversed", GLWidget::isYAxisReversed());
}

GLMdiChild *STLViewer::activeGLMdiChild()
{
    if(QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
    {
        return qobject_cast<GLMdiChild *>(activeSubWindow->widget());
    }
    return 0;
}

QMdiSubWindow *STLViewer::findGLMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList())
    {
        GLMdiChild *glMdiChild = qobject_cast<GLMdiChild *>(window->widget());
        if(glMdiChild->currentFile() == canonicalFilePath)
        {
            return window;
        }
    }
    return 0;
}
