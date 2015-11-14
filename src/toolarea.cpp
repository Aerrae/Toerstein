/************************************************************************************
**
** Toerstein - Code Editor
** Copyright (C) 2015  Tommi Tauriainen
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
*************************************************************************************/

#include "toolarea.h"
#include "filesearch.h"

#include <QGridLayout>
#include <QTabWidget>

ToolArea::ToolArea(QWidget *parent, ToersteBase *database) : QWidget(parent)
{
    toersteBase = database;

    layout = new QGridLayout(this);
    this->setLayout(layout);

    leftFileSearch = new FileSearch(this);
    leftFileSearch->setPlaceholderText("File path...");
    leftCodeEditor = new CodeEditor(this);

    rightFileSearch = new FileSearch(this);
    rightFileSearch->setPlaceholderText("File path...");
    rightCodeEditor = new CodeEditor(this);

    layout->addWidget(leftFileSearch,0,0);
    layout->addWidget(leftCodeEditor,1,0);
    layout->addWidget(rightFileSearch,0,1);
    layout->addWidget(rightCodeEditor,1,1);

    leftFileSearch->hide();
    leftCodeEditor->hide();

    connect(leftCodeEditor,SIGNAL(filePathChanged(QString)),leftFileSearch,SLOT(setText(QString)));
    connect(rightCodeEditor,SIGNAL(filePathChanged(QString)),rightFileSearch,SLOT(setText(QString)));
    connect(leftCodeEditor,SIGNAL(filePathChanged(QString)),this,SLOT(setLeftFilePath(QString)));
    connect(rightCodeEditor,SIGNAL(filePathChanged(QString)),this,SLOT(setRightFilePath(QString)));

    connect(leftFileSearch,SIGNAL(returnPressed()),this,SLOT(fileSearchPathOpen()));
    connect(rightFileSearch,SIGNAL(returnPressed()),this,SLOT(fileSearchPathOpen()));
    connect(leftFileSearch,SIGNAL(keyPressed(QString)),toersteBase,SLOT(queryFileInfo(QString)));
    connect(rightFileSearch,SIGNAL(keyPressed(QString)),toersteBase,SLOT(queryFileInfo(QString)));

    rightCodeEditor->setFocus();

    diffModeEnabled = false;
}

bool ToolArea::open(QString path)
{
    if ( leftCodeEditor->hasFocus() || leftFileSearch->hasFocus() )
    {
        if ( !leftCodeEditor->hasContent() )
        {
            return leftCodeEditor->open(path);
        }
    }
    else
    {
        if ( !rightCodeEditor->hasContent() )
        {
            return rightCodeEditor->open(path);
        }
    }

    return false;
}

bool ToolArea::open(QString path1, QString path2)
{
    if ( leftCodeEditor->hasContent() || rightCodeEditor->hasContent() )
    {
        return false;
    }

    if( !diffModeEnabled )
    {
        toggleViewMode();
    }

    leftCodeEditor->open(path1);
    rightCodeEditor->open(path2);

    return true;
}

void ToolArea::search(void)
{
    if ( leftCodeEditor->hasFocus() || leftFileSearch->hasFocus() )
    {
        leftFileSearch->setText("");
        leftFileSearch->setFocus();
    }
    else
    {
        rightFileSearch->setText("");
        rightFileSearch->setFocus();
    }
}

void ToolArea::save(void)
{
    rightCodeEditor->save();

    if ( diffModeEnabled )
    {
        leftCodeEditor->save();
    }
}

void ToolArea::saveAs(void)
{
    rightCodeEditor->saveAs();

    if ( diffModeEnabled )
    {
        leftCodeEditor->saveAs();
    }
}

bool ToolArea::closeFile(void)
{
    if ( !diffModeEnabled || rightCodeEditor->hasFocus() || rightFileSearch->hasFocus() )
    {
        rightCodeEditor->closeFile();
    }

    if ( diffModeEnabled )
    {
        if ( leftCodeEditor->closeFile() )
        {
            toggleViewMode();
        }
    }

    if ( leftCodeEditor->hasContent() || rightCodeEditor->hasContent() )
    {
        return false;
    }
    else
    {
        return true;
    }
}

void ToolArea::toggleViewMode(void)
{
    diffModeEnabled = !diffModeEnabled;

    if ( diffModeEnabled )
    {
        leftFileSearch->show();
        leftCodeEditor->show();
    }
    else
    {
        leftFileSearch->hide();
        leftCodeEditor->hide();
        rightCodeEditor->setFocus();
    }
}

void ToolArea::setFocusToLeftFileSearch(void)
{
    leftFileSearch->setFocus();
}

void ToolArea::setFocusToRightFileSearch(void)
{
    rightFileSearch->setFocus();
}

void ToolArea::setFocusToLeftCodeEditor(void)
{
    leftCodeEditor->setFocus();
}

void ToolArea::setFocusToRightCodeEditor(void)
{
    rightCodeEditor->setFocus();
}

void ToolArea::setLeftFilePath(QString path)
{
    emit leftFilePathChanged(path);
}

void ToolArea::setRightFilePath(QString path)
{
    emit rightFilePathChanged(path);
}

void ToolArea::fileSearchPathOpen(void)
{
    FileSearch *fileSearch = qobject_cast<FileSearch *>(sender());
    QString path;
    QRegExp rx;

    if ( fileSearch == leftFileSearch )
    {
        path = fileSearch->text();
    }
    else
    {
        path = fileSearch->text();
    }

    /* Filter filename.ext ( /path/../filename.ext ) */
    rx.setPattern("^(.+) \\( (.+) \\)$");

    if (rx.indexIn(path) != -1)
    {
        /* Convert to /path/../filename.ext */
        path = rx.cap(2);
    }

    if ( fileSearch == leftFileSearch )
    {
        leftCodeEditor->open(path);
    }
    else
    {
        rightCodeEditor->open(path);
    }
}
