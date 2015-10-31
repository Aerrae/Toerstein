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

#include "toerstein.h"

#include "codeeditor.h"
#include "toolarea.h"

#include <QMenuBar>
#include <QTabWidget>
#include <QFileDialog>
#include <QDebug>

Toerstein::Toerstein(QWidget *parent) : QMainWindow(parent)
{
    QMenuBar *menuBar = new QMenuBar;

    /* Create File menu */
    QMenu *fileMenu = menuBar->addMenu("File");

    fileMenu->addAction( tr("New &Tab"), this, SLOT(createNewTab()), QKeySequence(tr("Ctrl+T", "File|New Tab") ) );
    fileMenu->addAction( tr("&New"), this, SLOT(createNewFile()), QKeySequence(tr("Ctrl+N", "File|New File") ) );
    fileMenu->addAction( tr("&Open"), this, SLOT(open()), QKeySequence(tr("Ctrl+O", "File|Open File") ) );
    fileMenu->addAction( tr("Enter File&Path"), this, SLOT(search()), QKeySequence(tr("Ctrl+P", "File|Enter FilePath") ) );
    fileMenu->addAction( tr("&Save"), this, SLOT(save()), QKeySequence(tr("Ctrl+S", "File|Save File") ) );
    fileMenu->addAction( tr("Save As..."), this, SLOT(saveAs()) );
    fileMenu->addAction( tr("&Close File"), this, SLOT(closeFile()), QKeySequence(tr("Ctrl+W", "File|Close File") ) );
    fileMenu->addAction( tr("&Quit"), this, SLOT(close()), QKeySequence(tr("Ctrl+Q", "File|Quit") ) );

    QMenu *viewMenu = menuBar->addMenu("View");
    viewMenu->addAction( tr("&Diff view"), this, SLOT(toggleViewMode()), QKeySequence(tr("Ctrl+D", "Diff view|Quit") ) );

    this->setMenuBar(menuBar);

    /* Create tab view */
    tabWidget = new QTabWidget;
    setCentralWidget(tabWidget);

    /* Add one code editor widget to first tab */
    createNewFile();
}

void Toerstein::createNewFile(void)
{
    ToolArea *editorBlock = new ToolArea;
    tabWidget->setCurrentIndex(tabWidget->addTab(editorBlock,"New tab"));
    connect(editorBlock,SIGNAL(rightFilePathChanged(QString)),this,SLOT(setRightFilePath(QString)));
    editorBlock->setFocusToRightCodeEditor();
}

void Toerstein::createNewTab(void)
{
    ToolArea *editorBlock = new ToolArea;
    tabWidget->setCurrentIndex(tabWidget->addTab(editorBlock,"New tab"));
    connect(editorBlock,SIGNAL(rightFilePathChanged(QString)),this,SLOT(setRightFilePath(QString)));
    editorBlock->setFocusToRightFileSearch();
}

void Toerstein::open(void)
{
    QString name = QFileDialog::getOpenFileName(this,
        tr("Open File"), tr("All files (*.*)"));

    QFileInfo fileInfo(name);

    if ( !fileInfo.exists() || !fileInfo.isFile() )
    {
        return;
    }

    ToolArea* toolArea = qobject_cast<ToolArea *>(tabWidget->currentWidget());

    if ( !toolArea->open(name) )
    {
        createNewTab();
    }

    toolArea = qobject_cast<ToolArea *>(tabWidget->currentWidget());
    toolArea->open(name);
}

void Toerstein::search(void)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(tabWidget->currentWidget());
    toolArea->search();
}

void Toerstein::save(void)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(tabWidget->currentWidget());
    toolArea->save();
}
void Toerstein::saveAs(void)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(tabWidget->currentWidget());
    toolArea->saveAs();
}

void Toerstein::closeFile(void)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(tabWidget->currentWidget());

    if ( toolArea->closeFile() )
    {
        tabWidget->removeTab(tabWidget->currentIndex());
    }

    if ( tabWidget->currentWidget() )
    {
        tabWidget->currentWidget()->setFocus();
    }
    else
    {
        close();
    }
}

void Toerstein::toggleViewMode(void)
{
    qobject_cast<ToolArea *>(tabWidget->currentWidget())->toggleViewMode();
}

void Toerstein::setLeftFilePath(QString name)
{
    qDebug() << "Left filepath set to: " << name;
}

void Toerstein::setRightFilePath(QString name)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(sender());

    int tabIndex = tabWidget->indexOf(toolArea);

    if ( tabIndex >= 0 )
    {
        if ( name.isEmpty() )
        {
            tabWidget->setTabText(tabIndex,"New file");
        }
            else
        {
            tabWidget->setTabText(tabIndex,QFileInfo(name).fileName());
        }
    }
    qDebug() << "Right filepath set to: " << name;
}

Toerstein::~Toerstein()
{
}
