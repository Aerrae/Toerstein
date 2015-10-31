/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** Copyright (C) 2015 Tommi Tauriainen.
** Contact: http://www.tauriainen.info/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QMessageBox>
#include <QLineEdit>
#include <QGridLayout>

#include "codeeditor.h"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(modificationChanged(bool)),this, SLOT(setContentChanged(bool)));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    setContentChanged(false);
    fileName = "";
}

bool CodeEditor::hasContent(void)
{
    return ( !fileName.isEmpty() || contentHasChanged );
}

bool CodeEditor::hasChanged(void)
{
    return contentHasChanged;
}

void CodeEditor::setContentChanged(bool changed)
{
    contentHasChanged = changed;
}

bool CodeEditor::open(QString name)
{
    QFileInfo fileInfo(name);

    if ( fileInfo.exists() )
    {
        if ( !fileInfo.isFile() )
        {
        QMessageBox messageBox;
        messageBox.setText(QString("\"%1\" is a folder").arg(name));
        messageBox.exec();
        return false;
        }
    }
    else
    {
        QMessageBox messageBox;
        messageBox.setText(QString("File: \"%1\" does not exist").arg(name));
        messageBox.exec();
        return false;
    }

    if ( !closeFile() )
    {
        return false;
    }

    file.setFileName(name);

    if (!file.open(QFile::ReadOnly))
    {
        file.setFileName("");
        return false;
    }

    setPlainText(file.readAll());

    setFilePath(name);

    setContentChanged(false);

    return true;
}

bool CodeEditor::save(void)
{
    if ( fileName.isEmpty() )
    {
        return saveAs();
    }
    else
    {   if ( contentHasChanged )
        {
            return writeFile(fileName);
        }
        else
        {
            return true;
        }
    }
}

bool CodeEditor::saveAs(void)
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(), tr("All Files (*)"));

    if (name.isEmpty())
    {
        return false;
    }

    return writeFile(name);
}

bool CodeEditor::writeFile(QString name)
{
    file.setFileName(name);

    if (!file.open(QFile::WriteOnly))
    {
        return false;
    }

    QTextStream ts(&file);

    ts << toPlainText();

    this->document()->setModified(false);

    setFilePath(name);

    return true;
}

QMessageBox::StandardButton CodeEditor::askToSave(void)
{
    QMessageBox msgBox;
    msgBox.setText("Document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    return static_cast<QMessageBox::StandardButton>(msgBox.exec());
}

bool CodeEditor::closeFile(void)
{
    if (contentHasChanged)
    {
        QMessageBox::StandardButton reply = askToSave();

        if (reply == QMessageBox::Save)
        {
            return save();
        }
        else if ( reply == QMessageBox::Cancel)
        {
            return false;
        }
    }

    if ( file.isOpen() )
    {
        file.close();
    }

    setPlainText("");
    setFilePath("");
    setContentChanged(false);

    return true;
}

void CodeEditor::setFilePath(QString name)
{
    if (name != fileName)
    {
        fileName = name;
        emit filePathChanged(fileName);
    }
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    if ( digits < 3)
    {
        digits = 3;
    }

    int space = 5 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::gray).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
