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
#include <exception>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QMouseEvent>
#include <QCloseEvent>

#include "glmdichild.h"

GLMdiChild::GLMdiChild(QWidget *parent) : GLWidget(parent)
{
    stlFile = new StlFile;
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
}

GLMdiChild::~GLMdiChild()
{
    delete stlFile;
}

void GLMdiChild::newFile()
{
    static int sequenceNumber = 1;
    isUntitled = true;
    curFile = tr("untitled%1.stl").arg(sequenceNumber++);
    setWindowTitle(curFile);
}

bool GLMdiChild::loadFile(const QString &fileName)
{
    try
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        // Open the file and make an object from its content
        stlFile->open(fileName.toStdString());
        makeObjectFromStlFile(stlFile);
        updateGL();
        setCurrentFile(fileName);
        QApplication::restoreOverrideCursor();
        return true;
    }
    catch(::std::bad_alloc)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("Problem allocating memory.");
        msgBox.exec();
        return false;
    }
    catch(StlFile::wrong_header_size)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("The file " + fileName + " has a wrong size.");
        msgBox.exec();
        return false;
    }
    catch(StlFile::error_opening_file)
    { // ::std::ios_base::failure
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("The file " + fileName + " could not be opened.");
        msgBox.exec();
        return false;
    }
    catch(...)
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
    if(isUntitled)
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFile);
    }
}

bool GLMdiChild::saveAs()
{
    if(!isUntitled)
    {
        QString filterBin = tr("STL Files, binary (*.stl)");
        QString filterAscii = tr("STL Files, ASCII (*.stl)");
        QString filterAll = tr("All files (*.*)");
        QString filterSel;
        // Set the current file type as default
        if(stlFile->getStats().type == StlFile::ASCII)
        {
            filterSel = filterAscii;
        }
        else
        {
            filterSel = filterBin;
        }
        QString fileName = QFileDialog::getSaveFileName(
            this, tr("Save As"), curFile,
            filterBin + ";;" + filterAscii + ";;" + filterAll, &filterSel);
        if(fileName.isEmpty())
        {
            return false;
        }
        // Change the current file type to the one chosen by the user
        if(filterSel == filterBin)
        {
            stlFile->setFormat(StlFile::BINARY);
        }
        else if(filterSel == filterAscii)
        {
            stlFile->setFormat(StlFile::ASCII);
        }
        // Save the file
        return saveFile(fileName);
    }
    return false;
}

bool GLMdiChild::saveFile(const QString &fileName)
{
    try
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        // Write the current object into a file
        stlFile->write(fileName.toStdString());
        QApplication::restoreOverrideCursor();
        setCurrentFile(fileName);
        return true;
    }
    catch(StlFile::error_opening_file)
    { // ::std::ios_base::failure
        QApplication::restoreOverrideCursor();
        QMessageBox msgBox;
        msgBox.setText("Unable to write in " + fileName + ".");
        msgBox.exec();
        return false;
    }
    catch(...)
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
    QFileInfo fi(curFile);
    QString imFile = fi.path() + "/" + fi.completeBaseName() + ".png";
    QString filterPng = tr("PNG Files (*.png)");
    QString filterBmp = tr("BMP Files (*.bmp)");
    QString filterAll = tr("All files (*.*)");
    QString filterSel = filterPng;  // Default image type is PNG
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save Image"), imFile,
        filterPng + ";;" + filterBmp + ";;" + filterAll, &filterSel);
    if(fileName.isEmpty())
    {
        return false;
    }
    QString format = "png";  // Default image type is PNG if none was specified
    if(filterSel == filterBmp || QFileInfo(fileName).suffix() == ".bmp")
    {
        format = "bmp";  
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QImage screenshot = this->grabFrameBuffer();
    screenshot.save(fileName, format.toAscii());
    QApplication::restoreOverrideCursor();
    return true;
}

QString GLMdiChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void GLMdiChild::closeEvent(QCloseEvent *event)
{
    stlFile->close();
    if(maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void GLMdiChild::mousePressEvent(QMouseEvent *event)
{
    // Emit a signal if a mouse button is pressed on this widget
    emit mouseButtonPressed(event->buttons());
    GLWidget::mousePressEvent(event);
}

void GLMdiChild::mouseReleaseEvent(QMouseEvent *event)
{
    // Emit a signal if a mouse button is released on this widget
    emit mouseButtonReleased(event->button());
    GLWidget::mouseReleaseEvent(event);
}

bool GLMdiChild::maybeSave()
{
    return true;
}

void GLMdiChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString GLMdiChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
