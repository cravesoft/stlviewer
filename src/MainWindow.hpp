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

#ifndef _MAINWINDOW_HPP
#define _MAINWINDOW_HPP

#include "qt.hpp"
#include "RenderWidget.hpp"
#include "GLWidget.hpp"

class AxisGroupBox;
class DimensionsGroupBox;
class MeshInformationGroupBox;
class PropertiesGroupBox;
class SettingsDialog;

namespace stlviewer
{

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

        public: MainWindow(QWidget *_parent = nullptr, Qt::WindowFlags _flags = Qt::WindowFlags());

        public: ~MainWindow();

        /// \brief Show a custom menubar. If NULL is used, the default menubar
        /// is shown.
        /// \param[in] _bar The menubar to show. NULL will show the default
        /// menubar.
        public: void showMenuBar(QMenuBar *_bar = NULL);

        public: void init();

        signals: void leftMouseButtonModeChanged(GLWidget::LeftMouseButtonMode mode);

        protected: void closeEvent(QCloseEvent *event);

        /// This event is called when the mouse enters the widgets area during a drag/drop operation
        protected: void dragEnterEvent(QDragEnterEvent* event);

        /// this event is called when the mouse moves inside the widgets area during a drag/drop operation
        protected: void dragMoveEvent(QDragMoveEvent* event);

        /// this event is called when the mouse leaves the widgets area during a drag/drop operation
        protected: void dragLeaveEvent(QDragLeaveEvent* event);

        /// this event is called when the drop operation is initiated at the widget
        protected: void dropEvent(QDropEvent* event);

        private slots: void initialize();
        private slots: void newFile();
        private slots: void open();
        private slots: void save();
        private slots: void saveAs();
        private slots: void saveImage();
        private slots: void showSettingsDialog();
        private slots: void rotate();
        private slots: void panning();
        private slots: void about();
        private slots: void updateMenus();
        private slots: void updateWindowMenu();
        private slots: void setMousePressed(Qt::MouseButtons button);
        private slots: void setMouseReleased(Qt::MouseButtons button);
        private slots: GLMdiChild *createRenderWidget();
        private slots: void setActiveSubWindow(QWidget *window);
        private slots: void destroyRenderWidget();
        private slots: void zoomIn();
        private slots: void zoomOut();
        private slots: void zoomDefault();
        private slots: void backView();
        private slots: void frontView();
        private slots: void leftView();
        private slots: void rightView();
        private slots: void topView();
        private slots: void bottomView();
        private slots: void topFrontLeftView();
        private slots: void wireframe();
        private slots: void setDarkTheme();
        private slots: void setLightTheme();

        private: void createActions();
        private: void applyTheme(bool dark);

        private: void openFile(const QString& path);

        private: bool openFiles(const QStringList& pathList);

        private: void createMenus();

        private: void createMenuBar();

        private: void createToolBars();

        private: void createDockWindows();

        /// \brief Reads persistent platform-independent application settings
        private: void readSettings();

        /// \brief Writes persistent platform-independent application settings
        private: void writeSettings();

        private: GLMdiChild *activeRenderWidget();

        private: QMdiSubWindow *findRenderWidget(const QString &fileName);

        private: QMdiArea *mdiArea;

        private: SettingsDialog *settingsDialog;

        //private: QMenu *fileMenu;

        private: QMenu *windowMenu;

        private: QMenu *viewMenu;

        //private: QMenu *defaultViewsMenu;

        //private: QMenu *toolsMenu;

        //private: QMenu *helpMenu;

        private: QToolBar *fileToolBar;

        private: QToolBar *viewToolBar;

        //private: QToolBar *toolsToolBar;

        private: QString curDir;

        private: GLWidget::LeftMouseButtonMode leftMouseButtonMode;

        private: bool darkTheme;

        private: QWidget *modelInfoDockContent;
        private: QWidget *viewInfoDockContent;

        private: AxisGroupBox *axisGroupBox;

        private: DimensionsGroupBox *dimensionsGroupBox;

        private: MeshInformationGroupBox *meshInformationGroupBox;

        private: PropertiesGroupBox *propertiesGroupBox;

        /// \brief Mainwindow's menubar
        private: QMenuBar *menuBar;

        /// \brief A layout for the menu bar.
        private: QHBoxLayout *menuLayout;

        private: QAction *g_newAct;
        private: QAction *g_openAct;
        private: QAction *g_saveAct;
        private: QAction *g_saveAsAct;
        private: QAction *g_saveImageAct;
        private: QAction *g_showSettingsDialogAct;
        private: QAction *g_closeAct;
        private: QAction *g_closeAllAct;
        private: QAction *g_tileAct;
        private: QAction *g_cascadeAct;
        private: QAction *g_nextAct;
        private: QAction *g_previousAct;
        private: QAction *g_separatorAct;
        private: QAction *g_zoomInAct;
        private: QAction *g_rotateAct;
        private: QAction *g_panningAct;
        private: QAction *g_zoomOutAct;
        private: QAction *g_zoomDefaultAct;
        private: QAction *g_backViewAct;
        private: QAction *g_frontViewAct;
        private: QAction *g_leftViewAct;
        private: QAction *g_rightViewAct;
        private: QAction *g_topViewAct;
        private: QAction *g_bottomViewAct;
        private: QAction *g_topFrontLeftViewAct;
        private: QAction *g_wireframeAct;
        private: QAction *g_exitAct;
        private: QAction *g_aboutAct;
        private: QAction *g_darkThemeAct;
        private: QAction *g_lightThemeAct;
    };
}

#endif
