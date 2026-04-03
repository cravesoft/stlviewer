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

#include <signal.h>

#include <QDebug>
#include "qt.hpp"
#include "MainWindow.hpp"
#include "GuiIface.hpp"
#include "version.h"

// These are needed by QT. They need to stay valid during the entire
// lifetime of the application, and argc > 0 and argv must contain one valid
// character string
int g_argc = 1;
char **g_argv;

using namespace stlviewer;

QApplication *g_app;
MainWindow *g_main_win = NULL;

//////////////////////////////////////////////////
void signal_handler(int)
{
    stlviewer::stop();
}

namespace stlviewer
{
    /////////////////////////////////////////////////
    void fini()
    {
        fflush(stdout);
    }
}

/////////////////////////////////////////////////
void stlviewer::init()
{
    g_main_win->init();
}

/////////////////////////////////////////////////
bool stlviewer::load()
{
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    g_app = new QApplication(g_argc, g_argv);
    g_app->setApplicationVersion(STLVIEWER_VERSION);

    g_main_win = new MainWindow();

    return true;
}

/////////////////////////////////////////////////
bool stlviewer::run(int _argc, char **_argv)
{
    g_argc = _argc;
    g_argv = _argv;

    if (!stlviewer::load())
    {
        return false;
    }

    stlviewer::init();

#ifndef _WIN32
    // Now that we're about to run, install a signal handler to allow for
    // graceful shutdown on Ctrl-C.
    struct sigaction sigact;
    sigact.sa_handler = signal_handler;
    if (sigaction(SIGINT, &sigact, NULL))
    {
        qCritical() << "signal(2) failed while setting up for SIGINT";
        return false;
    }
#endif

    g_app->exec();

    stlviewer::fini();

    delete g_main_win;
    return true;
}

/////////////////////////////////////////////////
void stlviewer::stop()
{
    g_app->quit();
}

/////////////////////////////////////////////////
MainWindow *stlviewer::get_main_window()
{
    return g_main_win;
}
