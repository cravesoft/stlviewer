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

#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QCloseEvent>
#include <QMouseEvent>

#include "RenderWidget.hpp"

using stlviewer::GLWidget;

GLMdiChild::GLMdiChild(QWidget *parent)
    : GLWidget(parent)
    , stlFile(new StlFile)
    , isUntitled(true)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

GLMdiChild::~GLMdiChild()
{
    delete this->stlFile;
}

void GLMdiChild::newFile()
{
    static int sequenceNumber = 1;
    isUntitled = true;
    this->curFile = tr("untitled%1.stl").arg(sequenceNumber++);
    setWindowTitle(this->curFile);
}

bool GLMdiChild::loadFile(const QString &fileName)
{
    try
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        this->stlFile->open(fileName.toUtf8().constData());
        this->makeObjectFromSTLFile(*this->stlFile);
        this->setCurrentFile(fileName);
        QApplication::restoreOverrideCursor();
        return true;
    }
    catch (const StlFile::wrong_header_size&)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("The file " + fileName + " has a wrong size.");
        msgBox.exec();
        return false;
    }
    catch (const StlFile::error_opening_file&)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("The file " + fileName + " could not be opened.");
        msgBox.exec();
        return false;
    }
    catch (const ::std::bad_alloc&)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("Problem allocating memory.");
        msgBox.exec();
        return false;
    }
    catch (...)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("Error unknown.");
        msgBox.exec();
        return false;
    }
}

bool GLMdiChild::save()
{
    if (isUntitled)
        return saveAs();
    return saveFile(this->curFile);
}

bool GLMdiChild::saveAs()
{
    if (!isUntitled)
    {
        QString filterBin   = tr("STL Files, binary (*.stl)");
        QString filterAscii = tr("STL Files, ASCII (*.stl)");
        QString filterAll   = tr("All files (*.*)");
        QString filterSel;
        if (this->stlFile->getStats().type == StlFile::ASCII)
            filterSel = filterAscii;
        else
            filterSel = filterBin;
        QString fileName = QFileDialog::getSaveFileName(
            this, tr("Save As"), this->curFile,
            filterBin + ";;" + filterAscii + ";;" + filterAll, &filterSel);
        if (fileName.isEmpty())
            return false;
        if (filterSel == filterBin)
            this->stlFile->setFormat(StlFile::BINARY);
        else if (filterSel == filterAscii)
            this->stlFile->setFormat(StlFile::ASCII);
        return saveFile(fileName);
    }
    return false;
}

bool GLMdiChild::saveFile(const QString &fileName)
{
    try
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        this->stlFile->write(fileName.toUtf8().constData());
        QApplication::restoreOverrideCursor();
        this->setCurrentFile(fileName);
        return true;
    }
    catch (const StlFile::error_opening_file&)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("Unable to write in " + fileName + ".");
        msgBox.exec();
        return false;
    }
    catch (...)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("Error unknown.");
        msgBox.exec();
        return false;
    }
}

bool GLMdiChild::saveImage()
{
    QFileInfo fi(this->curFile);
    QString imFile = fi.path() + "/" + fi.completeBaseName() + ".png";
    QString filterPng = tr("PNG Files (*.png)");
    QString filterBmp = tr("BMP Files (*.bmp)");
    QString filterAll = tr("All files (*.*)");
    QString filterSel = filterPng;
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save Image"), imFile,
        filterPng + ";;" + filterBmp + ";;" + filterAll, &filterSel);
    if (fileName.isEmpty())
        return false;
    QString format = "png";
    if (filterSel == filterBmp || QFileInfo(fileName).suffix() == ".bmp")
        format = "bmp";
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QImage screenshot = this->grabFramebuffer();
    screenshot.save(fileName, format.toLatin1());
    QApplication::restoreOverrideCursor();
    return true;
}

QString GLMdiChild::userFriendlyCurrentFile()
{
    return QFileInfo(this->curFile).fileName();
}

void GLMdiChild::closeEvent(QCloseEvent *event)
{
    this->stlFile->close();
    event->accept();
}

void GLMdiChild::mousePressEvent(QMouseEvent *event)
{
    emit mouseButtonPressed(event->buttons());
    GLWidget::mousePressEvent(event);
}

void GLMdiChild::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseButtonReleased(event->button());
    GLWidget::mouseReleaseEvent(event);
}

bool GLMdiChild::maybeSave()
{
    return true;
}

void GLMdiChild::setCurrentFile(const QString &fileName)
{
    this->curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString GLMdiChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
