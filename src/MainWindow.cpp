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

#include "MainWindow.hpp"
#include <QBuffer>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QTimer>
#include "AxisGroupBox.hpp"
#include "DimensionsGroupBox.hpp"
#include "MeshInformationGroupBox.hpp"
#include "PropertiesGroupBox.hpp"
#include "SettingsDialog.hpp"

using namespace stlviewer;

// Load an SVG from resources and replace its fill color before rendering.
// FA4 icons use fill="#000000"; the replacement targets that literal value.
// Uses Qt's runtime SVG imageformat plugin (no Qt6::Svg compile dependency).
static QIcon coloredIcon(const QString &resourcePath, const QColor &color)
{
    QFile f(resourcePath);
    if (!f.open(QFile::ReadOnly))
        return QIcon();
    QByteArray svg = f.readAll();
    const QByteArray col = ("\"" + color.name() + "\"").toUtf8();
    svg.replace(QByteArrayLiteral("\"#000000\""), col);
    QBuffer buf(&svg);
    buf.open(QIODevice::ReadOnly);
    QImageReader reader(&buf, "svg");
    reader.setScaledSize(QSize(24, 24));
    QImage img = reader.read();
    if (img.isNull())
        return QIcon();
    return QIcon(QPixmap::fromImage(img));
}

MainWindow::MainWindow(QWidget *_parent, Qt::WindowFlags _flags)
    : QMainWindow(_parent, _flags)
{
    this->menuLayout = NULL;
    this->menuBar = NULL;
    this->setObjectName("mainWindow");

    setAcceptDrops(true);

    this->mdiArea = new QMdiArea;
    this->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(this->mdiArea);
    connect(this->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)),
        this, SLOT(updateMenus()));

    this->settingsDialog = new SettingsDialog(this);

    // Do these things first.
    {
        this->createActions();
    }

    // Read persistent application settings
    this->readSettings();

    // Deactivate the left mouse button when manipulating objects
    this->leftMouseButtonMode = GLWidget::INACTIVE;

    setWindowTitle(tr("STLViewer"));
    setUnifiedTitleAndToolBarOnMac(true);
    setWindowIcon(QIcon(":/images/stl.png"));

    // Do these things last
    {
        this->createMenus();
        this->createToolBars();
        this->createDockWindows();
        this->applyTheme(this->darkTheme);
        this->updateMenus();
        statusBar()->showMessage(tr("Ready"));
    }

    this->show();

    QTimer::singleShot(0, this, SLOT(initialize()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    g_newAct = new QAction(coloredIcon(":/images/fa/new.svg", QColor(0xd0, 0xd0, 0xd0)), tr("&New"), this);
    g_newAct->setShortcuts(QKeySequence::New);
    g_newAct->setStatusTip(tr("Create a new file"));
    connect(g_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    g_openAct = new QAction(coloredIcon(":/images/fa/open.svg", QColor(0xd0, 0xd0, 0xd0)), tr("&Open..."), this);
    g_openAct->setShortcut(QKeySequence::Open);
    g_openAct->setStatusTip(tr("Open an existing file"));
    connect(g_openAct, SIGNAL(triggered()), this, SLOT(open()));

    g_saveAct = new QAction(coloredIcon(":/images/fa/save.svg", QColor(0xd0, 0xd0, 0xd0)), tr("&Save"), this);
    g_saveAct->setShortcut(QKeySequence::Save);
    g_saveAct->setStatusTip(tr("Save the document to disk"));
    connect(g_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    g_saveAsAct = new QAction(tr("Save &As..."), this);
    g_saveAsAct->setShortcut(QKeySequence::SaveAs);
    g_saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(g_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    g_saveImageAct = new QAction(tr("Save Image..."), this);
    g_saveImageAct->setShortcut(tr("Ctrl+I"));
    g_saveImageAct->setStatusTip(tr("Save the current view to disk"));
    connect(g_saveImageAct, SIGNAL(triggered()), this, SLOT(saveImage()));

    g_showSettingsDialogAct = new QAction(tr("&Settings"), this);
    g_showSettingsDialogAct->setShortcut(tr("Ctrl+P"));
    g_showSettingsDialogAct->setStatusTip(tr("Show settings"));
    connect(g_showSettingsDialogAct, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));

    g_closeAct = new QAction(tr("Cl&ose"), this);
    //g_closeAct->setShortcut(tr("Ctrl+W"));
    g_closeAct->setStatusTip(tr("Close the active window"));
    connect(g_closeAct, SIGNAL(triggered()), this->mdiArea, SLOT(closeActiveSubWindow()));

    g_closeAllAct = new QAction(tr("Close &All"), this);
    g_closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(g_closeAllAct, SIGNAL(triggered()), this->mdiArea, SLOT(closeAllSubWindows()));

    g_tileAct = new QAction(tr("&Tile"), this);
    g_tileAct->setStatusTip(tr("Tile the windows"));
    connect(g_tileAct, SIGNAL(triggered()), this->mdiArea, SLOT(tileSubWindows()));

    g_cascadeAct = new QAction(tr("&Cascade"), this);
    g_cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(g_cascadeAct, SIGNAL(triggered()), this->mdiArea, SLOT(cascadeSubWindows()));

    g_nextAct = new QAction(tr("Ne&xt"), this);
    g_nextAct->setShortcut(QKeySequence::NextChild);
    g_nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(g_nextAct, SIGNAL(triggered()), this->mdiArea, SLOT(activateNextSubWindow()));

    g_previousAct = new QAction(tr("Pre&vious"), this);
    g_previousAct->setShortcut(QKeySequence::PreviousChild);
    g_previousAct->setStatusTip(tr("Move the focus to the previous window"));
    connect(g_previousAct, SIGNAL(triggered()), this->mdiArea, SLOT(activatePreviousSubWindow()));

    g_separatorAct = new QAction(this);
    g_separatorAct->setSeparator(true);

    g_rotateAct = new QAction(coloredIcon(":/images/fa/rotate.svg", QColor(0xd0, 0xd0, 0xd0)), tr("&Rotate"), this);
    g_rotateAct->setShortcut(tr("R"));
    g_rotateAct->setStatusTip(tr("Rotate the object"));
    g_rotateAct->setCheckable(true);
    connect(g_rotateAct, SIGNAL(triggered()), this, SLOT(rotate()));
    g_rotateAct->setChecked(false);

    g_panningAct = new QAction(coloredIcon(":/images/fa/pan.svg", QColor(0xd0, 0xd0, 0xd0)), tr("&Pan"), this);
    g_panningAct->setShortcut(tr("P"));
    g_panningAct->setStatusTip(tr("Drag the object around"));
    g_panningAct->setCheckable(true);
    connect(g_panningAct, SIGNAL(triggered()), this, SLOT(panning()));
    g_panningAct->setChecked(false);

    g_zoomInAct = new QAction(coloredIcon(":/images/fa/zoom_in.svg", QColor(0xd0, 0xd0, 0xd0)), tr("&Zoom In"), this);
    g_zoomInAct->setShortcut(tr("+"));
    g_zoomInAct->setStatusTip(tr("Zoom in"));
    connect(g_zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    g_zoomOutAct = new QAction(coloredIcon(":/images/fa/zoom_out.svg", QColor(0xd0, 0xd0, 0xd0)),
        tr("&Zoom Out"), this);
    g_zoomOutAct->setShortcut(tr("-"));
    g_zoomOutAct->setStatusTip(tr("Zoom out"));
    connect(g_zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    g_zoomDefaultAct = new QAction(coloredIcon(":/images/fa/zoom_default.svg", QColor(0xd0, 0xd0, 0xd0)),
        tr("&Default Zoom"), this);
    g_zoomDefaultAct->setShortcut(tr("1"));
    g_zoomDefaultAct->setStatusTip(tr("Set default zoom"));
    connect(g_zoomDefaultAct, SIGNAL(triggered()), this, SLOT(zoomDefault()));

    g_backViewAct = new QAction(coloredIcon(":/images/back_view.svg", QColor(0xd0, 0xd0, 0xd0)),
        tr("&Back View"), this);
    g_backViewAct->setStatusTip(tr("Back view"));
    connect(g_backViewAct, SIGNAL(triggered()), this, SLOT(backView()));

    g_frontViewAct = new QAction(coloredIcon(":/images/front_view.svg", QColor(0xd0, 0xd0, 0xd0)),
        tr("&Front View"), this);
    g_frontViewAct->setStatusTip(tr("Front view"));
    connect(g_frontViewAct, SIGNAL(triggered()), this, SLOT(frontView()));

    g_leftViewAct = new QAction(coloredIcon(":/images/left_view.svg", QColor(0xd0, 0xd0, 0xd0)),
        tr("&Left View"), this);
    g_leftViewAct->setStatusTip(tr("Left view"));
    connect(g_leftViewAct, SIGNAL(triggered()), this, SLOT(leftView()));

    g_rightViewAct = new QAction(coloredIcon(":/images/right_view.svg", QColor(0xd0, 0xd0, 0xd0)),
        tr("&Right View"), this);
    g_rightViewAct->setStatusTip(tr("Right view"));
    connect(g_rightViewAct, SIGNAL(triggered()), this, SLOT(rightView()));

    g_topViewAct = new QAction(coloredIcon(":/images/top_view.svg", QColor(0xd0, 0xd0, 0xd0)),
        tr("&Top View"), this);
    g_topViewAct->setStatusTip(tr("Top view"));
    connect(g_topViewAct, SIGNAL(triggered()), this, SLOT(topView()));

    g_bottomViewAct = new QAction(coloredIcon(":/images/bottom_view.svg", QColor(0xd0, 0xd0, 0xd0)),
        tr("&Bottom View"), this);
    g_bottomViewAct->setStatusTip(tr("Bottom view"));
    connect(g_bottomViewAct, SIGNAL(triggered()), this, SLOT(bottomView()));

    g_topFrontLeftViewAct = new QAction(coloredIcon(":/images/isometric_view.svg", QColor(0xd0, 0xd0, 0xd0)),
        tr("&Top Front Left View"), this);
    g_topFrontLeftViewAct->setStatusTip(tr("Top Front Left view"));
    connect(g_topFrontLeftViewAct, SIGNAL(triggered()), this, SLOT(topFrontLeftView()));

    g_exitAct = new QAction(tr("E&xit"), this);
    g_exitAct->setShortcut(tr("Ctrl+Q"));
    g_exitAct->setStatusTip(tr("Exit the application"));
    connect(g_exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    g_aboutAct = new QAction(tr("&About"), this);
    g_aboutAct->setStatusTip(tr("About STLViewer"));
    connect(g_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    g_wireframeAct = new QAction(coloredIcon(":/images/wireframe.svg", QColor(0xd0, 0xd0, 0xd0)),
                                 tr("&Wireframe"), this);
    g_wireframeAct->setShortcut(tr("W"));
    g_wireframeAct->setStatusTip(tr("Wireframe view"));
    g_wireframeAct->setCheckable(true);
    connect(g_wireframeAct, SIGNAL(triggered()), this, SLOT(wireframe()));
    g_wireframeAct->setChecked(false);

    g_lightThemeAct = new QAction(tr("&Light Theme"), this);
    g_lightThemeAct->setStatusTip(tr("Switch to light theme"));
    connect(g_lightThemeAct, &QAction::triggered, this, &MainWindow::setLightTheme);

    g_darkThemeAct = new QAction(tr("&Dark Theme"), this);
    g_darkThemeAct->setStatusTip(tr("Switch to dark theme"));
    connect(g_darkThemeAct, &QAction::triggered, this, &MainWindow::setDarkTheme);
}

void MainWindow::createMenus()
{
    this->showMenuBar();

    QFrame *frame = new QFrame;
    frame->setLayout(this->menuLayout);
    frame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    this->setMenuWidget(frame);
}

void MainWindow::showMenuBar(QMenuBar *_bar)
{
    if (!this->menuLayout)
    {
        this->menuLayout = new QHBoxLayout;
    }

    // Remove all widgets from the menuLayout
    while(this->menuLayout->takeAt(0) != 0)
    {
    }

    if (!this->menuBar)
    {
        // create the native menu bar
        this->menuBar = new QMenuBar;
        this->menuBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        this->setMenuBar(this->menuBar);

        this->createMenuBar();
    }

    this->menuBar->clear();

    QMenuBar *newMenuBar = NULL;
    if (!_bar)
    {
        // Get the main window's menubar
        // Note: for some reason we can not call menuBar() again,
        // so manually retrieving the menubar from the mainwindow.
        QList<QMenuBar *> menuBars  = this->findChildren<QMenuBar *>();
        if (menuBars.isEmpty())
            return;
        newMenuBar = menuBars[0];
    }
    else
    {
        newMenuBar = _bar;
    }
    QList<QMenu *> menus = newMenuBar->findChildren<QMenu *>(QString(), Qt::FindDirectChildrenOnly);
    for(int i = 0; i < menus.size(); ++i)
    {
        this->menuBar->addMenu(menus[i]);
    }

    this->menuLayout->addWidget(this->menuBar);

    this->menuLayout->addStretch(5);
    this->menuLayout->setContentsMargins(0, 0, 0, 0);

    this->showNormal();
    this->menuBar->show();
}

void MainWindow::createMenuBar()
{
    // main window's menu bar
    QMenuBar *bar = QMainWindow::menuBar();

    QMenu *fileMenu = bar->addMenu(tr("&File"));
    fileMenu->addAction(g_newAct);
    fileMenu->addAction(g_openAct);
    fileMenu->addAction(g_saveAct);
    fileMenu->addAction(g_saveAsAct);
    fileMenu->addAction(g_saveImageAct);
    fileMenu->addSeparator();
    fileMenu->addAction(g_exitAct);

    this->viewMenu = bar->addMenu(tr("&View"));
    this->viewMenu->addAction(g_rotateAct);
    this->viewMenu->addAction(g_panningAct);
    this->viewMenu->addAction(g_zoomInAct);
    this->viewMenu->addAction(g_zoomOutAct);
    this->viewMenu->addAction(g_zoomDefaultAct);
    this->viewMenu->addAction(g_wireframeAct);

    QMenu *defaultViewsMenu = this->viewMenu->addMenu(tr("&Default Views"));
    defaultViewsMenu->addAction(g_backViewAct);
    defaultViewsMenu->addAction(g_frontViewAct);
    defaultViewsMenu->addAction(g_leftViewAct);
    defaultViewsMenu->addAction(g_rightViewAct);
    defaultViewsMenu->addAction(g_topViewAct);
    defaultViewsMenu->addAction(g_bottomViewAct);
    defaultViewsMenu->addAction(g_topFrontLeftViewAct);

    this->viewMenu->addSeparator();

    QMenu *toolsMenu = bar->addMenu(tr("&Tools"));
    toolsMenu->addAction(g_showSettingsDialogAct);
    toolsMenu->addSeparator();
    QMenu *themeMenu = toolsMenu->addMenu(tr("&Theme"));
    themeMenu->addAction(g_lightThemeAct);
    themeMenu->addAction(g_darkThemeAct);

    this->windowMenu = bar->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(this->windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    bar->addSeparator();

    QMenu *helpMenu = bar->addMenu(tr("&Help"));
    helpMenu->addAction(g_aboutAct);
}

void MainWindow::createToolBars()
{
    this->fileToolBar = addToolBar(tr("File"));
    this->fileToolBar->addAction(g_newAct);
    this->fileToolBar->addAction(g_openAct);
    this->fileToolBar->addAction(g_saveAct);

    this->viewToolBar = addToolBar(tr("View"));
    this->viewToolBar->addAction(g_rotateAct);
    this->viewToolBar->addAction(g_panningAct);
    this->viewToolBar->addAction(g_zoomInAct);
    this->viewToolBar->addAction(g_zoomOutAct);
    this->viewToolBar->addAction(g_zoomDefaultAct);
    this->viewToolBar->addAction(g_wireframeAct);
    this->viewToolBar->addAction(g_backViewAct);
    this->viewToolBar->addAction(g_frontViewAct);
    this->viewToolBar->addAction(g_leftViewAct);
    this->viewToolBar->addAction(g_rightViewAct);
    this->viewToolBar->addAction(g_topViewAct);
    this->viewToolBar->addAction(g_bottomViewAct);
    this->viewToolBar->addAction(g_topFrontLeftViewAct);

#if 0
    toolsToolBar = addToolBar(tr("Settings"));
    toolsToolBar->addAction(g_showSettingsDialogAct);
#endif
}

/////////////////////////////////////////////////
void MainWindow::init()
{
    //Events::mainWindowReady();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}


void MainWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        for (int i = 0; i < urlList.size() && i < 32; ++i)
        {
            pathList.append(urlList.at(i).toLocalFile());
        }

        if (this->openFiles(pathList))
            event->acceptProposedAction();
    }
}

void MainWindow::openFile(const QString& path)
{
    QMdiSubWindow *existing = this->findRenderWidget(path);
    if (existing)
    {
        this->mdiArea->setActiveSubWindow(existing);
    }
    else
    {
        GLMdiChild *child = this->createRenderWidget();
        if (child->loadFile(path))
        {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        }
        else
        {
            setActiveSubWindow(child);
            this->mdiArea->closeActiveSubWindow();
            //child->close();
        }
    }
}

void MainWindow::initialize()
{
    QStringList pathList;
    for(int i = 1; i < QCoreApplication::arguments().size(); i++)
    {
        pathList.append(QCoreApplication::arguments().at(i));
    }
    this->openFiles(pathList);
}

bool MainWindow::openFiles(const QStringList& pathList)
{
    for(int i = 0; i < pathList.size() && i < 32; ++i)
    {
        this->openFile(pathList.at(i));
    }
    return !pathList.isEmpty();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->mdiArea->closeAllSubWindows();
    if (this->activeRenderWidget())
    {
        event->ignore();
    }
    else
    {
        this->writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    GLMdiChild *child = this->createRenderWidget();
    child->newFile();
    child->show();

    // Reset all informations
    this->axisGroupBox->reset();
    this->dimensionsGroupBox->reset();
    this->meshInformationGroupBox->reset();
    this->propertiesGroupBox->reset();
}

void MainWindow::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open file(s)"),
        this->curDir, tr("STL Files (*.stl);;All Files (*.*)"));
    if (!fileNames.isEmpty())
    {
        this->curDir = QFileInfo(fileNames.first()).filePath();
        this->openFiles(fileNames);
    }
}

void MainWindow::save()
{
    if (this->activeRenderWidget() && this->activeRenderWidget()->save())
    {
        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::saveAs()
{
    if (this->activeRenderWidget() && this->activeRenderWidget()->saveAs())
    {
        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::saveImage()
{
    if (this->activeRenderWidget() && this->activeRenderWidget()->saveImage())
    {
        statusBar()->showMessage(tr("Image saved"), 2000);
    }
}

void MainWindow::showSettingsDialog()
{
    this->settingsDialog->exec(GLWidget::isYAxisReversed());
    if (this->settingsDialog->result() == QDialog::Accepted)
    {
        GLWidget::setYAxisMode(this->settingsDialog->isYAxisReversed());
    }
}

void MainWindow::rotate()
{
    if (g_rotateAct->isChecked())
    {
        g_panningAct->setChecked(false);
        this->leftMouseButtonMode = GLWidget::ROTATE;
    }
    else
    {
        this->leftMouseButtonMode = GLWidget::INACTIVE;
    }
    emit leftMouseButtonModeChanged(this->leftMouseButtonMode);
}

void MainWindow::panning()
{
    if (g_panningAct->isChecked())
    {
        g_rotateAct->setChecked(false);
        this->leftMouseButtonMode = GLWidget::PANNING;
    }
    else
    {
        this->leftMouseButtonMode = GLWidget::INACTIVE;
    }
    emit leftMouseButtonModeChanged(this->leftMouseButtonMode);
}

void MainWindow::about()
{
    const QString linkColor = this->darkTheme ? "white" : "#0000cc";
    QMessageBox::about(this, tr("About STLViewer"),
        QString(
        "<p align=\"center\">STLViewer</p>"
        "<p align=\"center\">v%1</p>"
        "<p align=\"center\">Copyright (c) 2015 Olivier Crave</p>"
        "<p align=\"center\">Web: <a style=\"color: %2;\" href=\"http://www.cravesoft.com\">"
        "www.cravesoft.com</a></p>"
        "<p align=\"center\">Mail: <a style=\"color: %2;\" href=\"mailto:cravesoft@gmail.com\">"
        "cravesoft@gmail.com</a></p>"
        ).arg(QCoreApplication::applicationVersion(), linkColor));
}

void MainWindow::updateMenus()
{
    bool hasRenderWidget = (this->activeRenderWidget() != 0);
    if (hasRenderWidget && !this->activeRenderWidget()->isUntitled)
    {
        g_saveAct->setEnabled(true);
        g_saveAsAct->setEnabled(true);
    }
    else
    {
        g_saveAct->setEnabled(false);
        g_saveAsAct->setEnabled(false);
    }
    g_saveImageAct->setEnabled(hasRenderWidget);
    g_closeAct->setEnabled(hasRenderWidget);
    g_closeAllAct->setEnabled(hasRenderWidget);
    g_zoomInAct->setEnabled(hasRenderWidget);
    g_rotateAct->setEnabled(hasRenderWidget);
    g_panningAct->setEnabled(hasRenderWidget);
    g_zoomOutAct->setEnabled(hasRenderWidget);
    g_zoomDefaultAct->setEnabled(hasRenderWidget);
    g_wireframeAct->setEnabled(hasRenderWidget);
    if (hasRenderWidget)
    {
        //g_wireframeAct->setChecked(this->activeRenderWidget()->isWireframeModeActivated());
    }
    else
    {
        g_wireframeAct->setChecked(false);
    }
    g_backViewAct->setEnabled(hasRenderWidget);
    g_frontViewAct->setEnabled(hasRenderWidget);
    g_leftViewAct->setEnabled(hasRenderWidget);
    g_rightViewAct->setEnabled(hasRenderWidget);
    g_topViewAct->setEnabled(hasRenderWidget);
    g_bottomViewAct->setEnabled(hasRenderWidget);
    g_topFrontLeftViewAct->setEnabled(hasRenderWidget);
    g_tileAct->setEnabled(hasRenderWidget);
    g_cascadeAct->setEnabled(hasRenderWidget);
    g_nextAct->setEnabled(hasRenderWidget);
    g_previousAct->setEnabled(hasRenderWidget);
    g_separatorAct->setVisible(hasRenderWidget);
    if (hasRenderWidget && !this->activeRenderWidget()->isUntitled)
    {
        this->axisGroupBox->setRotation(this->activeRenderWidget()->getRotation());
        this->dimensionsGroupBox->setValues(this->activeRenderWidget()->getStats());
        this->meshInformationGroupBox->setValues(this->activeRenderWidget()->getStats());
        this->propertiesGroupBox->setValues(this->activeRenderWidget()->getStats());
    }
    else
    {
        this->axisGroupBox->reset();
        this->dimensionsGroupBox->reset();
        this->meshInformationGroupBox->reset();
        this->propertiesGroupBox->reset();
    }
}

void MainWindow::updateWindowMenu()
{
    this->windowMenu->clear();
    this->windowMenu->addAction(g_closeAct);
    this->windowMenu->addAction(g_closeAllAct);
    this->windowMenu->addSeparator();
    this->windowMenu->addAction(g_tileAct);
    this->windowMenu->addAction(g_cascadeAct);
    this->windowMenu->addSeparator();
    this->windowMenu->addAction(g_nextAct);
    this->windowMenu->addAction(g_previousAct);
    this->windowMenu->addAction(g_separatorAct);
    QList<QMdiSubWindow *> windows = this->mdiArea->subWindowList();
    g_separatorAct->setVisible(!windows.isEmpty());
    for(int i = 0; i < windows.size(); ++i)
    {
        GLMdiChild *child = qobject_cast<GLMdiChild *>(windows.at(i)->widget());
        QString text;
        if (i < 9)
        {
            text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        }
        else
        {
            text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = this->windowMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(child == this->activeRenderWidget());
        QMdiSubWindow *subWin = windows.at(i);
        connect(action, &QAction::triggered, this, [this, subWin]() {
            this->setActiveSubWindow(subWin);
        });
    }
}

void MainWindow::setMousePressed(Qt::MouseButtons button)
{
    if (button & Qt::RightButton)
    {
        g_rotateAct->setChecked(true);
    }
    else if (button & Qt::MiddleButton)
    {
        g_panningAct->setChecked(true);
    }
}

void MainWindow::setMouseReleased(Qt::MouseButtons button)
{
    if (button & Qt::RightButton)
    {
        if (this->leftMouseButtonMode != GLWidget::ROTATE)
        {
            g_rotateAct->setChecked(false);
        }
    }
    else if (button & Qt::MiddleButton)
    {
        if (this->leftMouseButtonMode != GLWidget::PANNING)
        {
            g_panningAct->setChecked(false);
        }
    }
}

GLMdiChild *MainWindow::createRenderWidget()
{
    GLMdiChild *child = new GLMdiChild;
    QMdiSubWindow *subWin = this->mdiArea->addSubWindow(child);
    child->setLeftMouseButtonMode(this->leftMouseButtonMode);

    connect(child, SIGNAL(mouseButtonPressed(Qt::MouseButtons)), this,
        SLOT(setMousePressed(Qt::MouseButtons)));

    connect(child, SIGNAL(mouseButtonReleased(Qt::MouseButtons)), this,
        SLOT(setMouseReleased(Qt::MouseButtons)));

    connect(this, &MainWindow::leftMouseButtonModeChanged,
        child, &stlviewer::GLWidget::setLeftMouseButtonMode);

    connect(child, &stlviewer::GLWidget::rotationChanged,
        this->axisGroupBox, &AxisGroupBox::setRotation);

    connect(child, SIGNAL(destroyed()), this, SLOT(destroyRenderWidget()));

    return child;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
    {
        return;
    }
    this->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::destroyRenderWidget()
{
    // Qt emits destroyed() before removing connections, so the typed
    // leftMouseButtonModeChanged -> setLeftMouseButtonMode connection to the
    // dying widget is still live.  Disconnect it now (using QObject* to avoid
    // the vtable type-check assertion that fires on a partially-destroyed obj).
    disconnect(this, &MainWindow::leftMouseButtonModeChanged, sender(), nullptr);

    if (this->activeRenderWidget() == 0)
    {
        g_panningAct->setChecked(false);
        g_rotateAct->setChecked(false);
        this->leftMouseButtonMode = GLWidget::INACTIVE;
        emit leftMouseButtonModeChanged(this->leftMouseButtonMode);
    }
}

void MainWindow::zoomIn()
{
    this->activeRenderWidget()->zoomIn();
}

void MainWindow::zoomOut()
{
    this->activeRenderWidget()->zoomOut();
}

void MainWindow::zoomDefault()
{
    this->activeRenderWidget()->setDefaultZoom();
}

void MainWindow::backView()
{
    this->activeRenderWidget()->setBackView();
}

void MainWindow::frontView()
{
    this->activeRenderWidget()->setFrontView();
}

void MainWindow::leftView()
{
    this->activeRenderWidget()->setLeftView();
}

void MainWindow::rightView()
{
    this->activeRenderWidget()->setRightView();
}

void MainWindow::topView()
{
    this->activeRenderWidget()->setTopView();
}

void MainWindow::bottomView()
{
    this->activeRenderWidget()->setBottomView();
}

void MainWindow::topFrontLeftView()
{
    this->activeRenderWidget()->setTopFrontLeftView();
}

void MainWindow::wireframe()
{
    this->activeRenderWidget()->setWireframeMode(g_wireframeAct->isChecked());
}

void MainWindow::createDockWindows()
{
    // Create a DockWidget named "Informations"
    QDockWidget *dock = new QDockWidget(tr("Model Informations"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // Create one GroupBox for each type of data
    this->dimensionsGroupBox = new DimensionsGroupBox(this);
    this->meshInformationGroupBox = new MeshInformationGroupBox(this);
    this->propertiesGroupBox = new PropertiesGroupBox(this);

    // Create a layout inside a widget to display all GroupBoxes in one layout
    this->modelInfoDockContent = new QWidget;
    this->modelInfoDockContent->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                  QSizePolicy::Fixed));
    this->modelInfoDockContent->setAutoFillBackground(true);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(this->dimensionsGroupBox);
    layout->addWidget(this->meshInformationGroupBox);
    layout->addWidget(this->propertiesGroupBox);
    this->modelInfoDockContent->setLayout(layout);

    // Embed the widget that contains all GroupBoxes into the DockWidget
    dock->setWidget(this->modelInfoDockContent);

    // Add the DockWidget at the right side of the main layout
    addDockWidget(Qt::RightDockWidgetArea, dock);

    // Add a button in the view menu to show/hide the DockWidget
    this->viewMenu->addAction(dock->toggleViewAction());

    // Create a DockWidget named "View Informations"
    dock = new QDockWidget(tr("View Informations"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // Create one GroupBox to display the axis
    this->axisGroupBox = new AxisGroupBox(this);

    // Create a layout inside a widget to display all GroupBoxes in one layout
    this->viewInfoDockContent = new QWidget;
    this->viewInfoDockContent->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                  QSizePolicy::Fixed));
    this->viewInfoDockContent->setAutoFillBackground(true);
    layout = new QVBoxLayout;
    layout->addWidget(this->axisGroupBox);
    this->viewInfoDockContent->setLayout(layout);

    // Embed the widget that contains all GroupBoxes into the DockWidget
    dock->setWidget(this->viewInfoDockContent);

    // Add the DockWidget at the right side of the main layout
    addDockWidget(Qt::RightDockWidgetArea, dock);

    // Add a button in the view menu to show/hide the DockWidget
    this->viewMenu->addAction(dock->toggleViewAction());
}

void MainWindow::applyTheme(bool dark)
{
    this->darkTheme = dark;

    QFile file(dark ? ":/dark.qss" : ":/light.qss");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(QLatin1String(file.readAll()));

    // Recolor all icons for the current theme (single SVG set for all)
    const QColor iconColor = dark ? QColor(0xd0, 0xd0, 0xd0) : QColor(0x24, 0x29, 0x2e);
    const QList<QPair<QAction*, QString>> allIconActions = {
        {g_newAct,              ":/images/fa/new.svg"},
        {g_openAct,             ":/images/fa/open.svg"},
        {g_saveAct,             ":/images/fa/save.svg"},
        {g_rotateAct,           ":/images/fa/rotate.svg"},
        {g_panningAct,          ":/images/fa/pan.svg"},
        {g_zoomInAct,           ":/images/fa/zoom_in.svg"},
        {g_zoomOutAct,          ":/images/fa/zoom_out.svg"},
        {g_zoomDefaultAct,      ":/images/fa/zoom_default.svg"},
        {g_wireframeAct,        ":/images/wireframe.svg"},
        {g_backViewAct,         ":/images/back_view.svg"},
        {g_frontViewAct,        ":/images/front_view.svg"},
        {g_leftViewAct,         ":/images/left_view.svg"},
        {g_rightViewAct,        ":/images/right_view.svg"},
        {g_topViewAct,          ":/images/top_view.svg"},
        {g_bottomViewAct,       ":/images/bottom_view.svg"},
        {g_topFrontLeftViewAct, ":/images/isometric_view.svg"},
    };
    for (const auto &p : allIconActions)
        p.first->setIcon(coloredIcon(p.second, iconColor));

    QColor dockBg = dark ? QColor(0x30, 0x30, 0x30) : QColor(0xd8, 0xd8, 0xd8);
    for (QWidget *w : {this->modelInfoDockContent, this->viewInfoDockContent})
    {
        QPalette pal = w->palette();
        pal.setColor(QPalette::Window, dockBg);
        w->setPalette(pal);
    }

}

void MainWindow::setDarkTheme()
{
    this->applyTheme(true);
}

void MainWindow::setLightTheme()
{
    this->applyTheme(false);
}

void MainWindow::readSettings()
{
    QSettings settings("Cravesoft", "STLViewer");
    this->curDir = settings.value("dir", QString()).toString();
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    GLWidget::setYAxisMode(settings.value("yAxisReversed", false).toBool());
    this->darkTheme = settings.value("darkTheme", false).toBool();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Cravesoft", "STLViewer");
    settings.setValue("dir", this->curDir);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("yAxisReversed", GLWidget::isYAxisReversed());
    settings.setValue("darkTheme", this->darkTheme);
}

GLMdiChild *MainWindow::activeRenderWidget()
{
    if (QMdiSubWindow *activeSubWindow = this->mdiArea->activeSubWindow())
    {
        return qobject_cast<GLMdiChild *>(activeSubWindow->widget());
    }
    return 0;
}

QMdiSubWindow *MainWindow::findRenderWidget(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    for (QMdiSubWindow *window : this->mdiArea->subWindowList())
    {
        GLMdiChild *renderWidget = qobject_cast<GLMdiChild *>(window->widget());
        if (renderWidget->currentFile() == canonicalFilePath)
        {
            return window;
        }
    }
    return 0;
}
