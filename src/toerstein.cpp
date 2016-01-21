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

#include "toerstebase.h"
#include "codeeditor.h"
#include "tabview.h"
#include "toolarea.h"

#include <QMenuBar>
#include <QShortcut>
#include <QFileDialog>
#include <QDebug>
#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

Toerstein::Toerstein(QWidget *parent) : QMainWindow(parent)
{
    toersteBase = new ToersteBase(this);
    toerstelliSense = new ToerstelliSense(this,toersteBase);

    connect(this,SIGNAL(fileLoaded(QString)),toerstelliSense->worker(),SLOT(indexFile(QString)));

    createMenuBar();
    createShortcuts();

    /* Create tab view */
    tabView = new TabView(this);
    tabView->setTabsClosable(true);
    setCentralWidget(tabView);

    connect(tabView,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));

    QCommandLineOption diffMode(QStringList() << "d" << "diff");
    QCommandLineParser parser;

    parser.addOption(diffMode);
    parser.process(QCoreApplication::arguments());

    const QStringList args = parser.positionalArguments();

    if ( args.length() > 0 )
    {
        if ( parser.isSet(diffMode) )
        {
            createNewFile();

            if ( args.length() > 1 )
            {
                open(args.at(0), args.at(1));
            }
            else
            {
                open(args.at(0));
            }
        }
        else
        {
            for(int i=0; i < args.length(); i++)
            {
                createNewFile();
                open(args.at(i));
            }
        }
    }
    else
    {
        createNewFile();
    }
}

bool Toerstein::event(QEvent* e)
{
    if ( e->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);

        prevKey = static_cast<Qt::Key>(keyEvent->key());
        prevModifier = keyEvent->modifiers();
    }
    else if ( e->type() == QEvent::KeyRelease )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);

        /* Check if only alt-key has been pressed and released */
        if ( ( prevKey == Qt::Key_Alt ) &&
            ( ( prevModifier == Qt::NoModifier ) ||
              ( prevModifier == Qt::AltModifier ) ) &&
            ( keyEvent->key() == Qt::Key_Alt ) &&
            ( ( keyEvent->modifiers() == Qt::NoModifier ) ||
              ( keyEvent->modifiers() == Qt::AltModifier ) ) )
        {
            toggleMenuBarVisibility();
            return true;
        }
    }

    return QMainWindow::event(e);
}

void Toerstein::createMenuBar(void)
{
    QMenuBar *menuBar = new QMenuBar(this);
    menuBar->setVisible(false);

    /* Create File menu */
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));

    fileMenu->addAction( tr("New &Tab"), this, SLOT(createNewTab()) );
    fileMenu->addAction( tr("&New"), this, SLOT(createNewFile()) );
    fileMenu->addAction( tr("&Open"), this, SLOT(open()) );
    fileMenu->addAction( tr("Enter File &Path"), this, SLOT(search()) );
    fileMenu->addAction( tr("&Save"), this, SLOT(save()) );
    fileMenu->addAction( tr("Save As..."), this, SLOT(saveAs()) );
    fileMenu->addAction( tr("&Close File"), this, SLOT(closeFile()) );
    fileMenu->addAction( tr("&Quit"), this, SLOT(close()) );

    QMenu *viewMenu = menuBar->addMenu(tr("&View"));
    viewMenu->addAction( tr("Toggle &Diff View"), this, SLOT(toggleViewMode()) );

    this->setMenuBar(menuBar);
}

void Toerstein::createShortcuts(void)
{
    QShortcut *shortcut;

    /* In QMenuBar shortcuts won't work if menuBar is not visible,
     * so we'll add them separately here
     */

    /* Shortcuts to File-menu */
    shortcut = new QShortcut(QKeySequence(tr("Ctrl+T", "File|New Tab")),this,SLOT(createNewTab()));
    shortcut->setAutoRepeat(false);
    shortcut = new QShortcut(QKeySequence(tr("Ctrl+N", "File|New File")),this,SLOT(createNewFile()));
    shortcut->setAutoRepeat(false);
    shortcut = new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open File")),this,SLOT(open()));
    shortcut->setAutoRepeat(false);
    shortcut = new QShortcut(QKeySequence(tr("Ctrl+P", "File|Enter File Path") ),this,SLOT(search()));
    shortcut->setAutoRepeat(false);
    shortcut = new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save File") ),this,SLOT(save()));
    shortcut->setAutoRepeat(false);
    shortcut = new QShortcut(QKeySequence(tr("Ctrl+W", "File|Close File") ),this,SLOT(closeFile()));
    shortcut->setAutoRepeat(false);
    shortcut = new QShortcut(QKeySequence(tr("Ctrl+Q", "File|Quit")),this,SLOT(close()));
    shortcut->setAutoRepeat(false);

    /* Shortcuts to View-menu */
    shortcut = new QShortcut(QKeySequence(tr("Ctrl+D", "Toggle Diff View|Quit")),this,SLOT(toggleViewMode()));
    shortcut->setAutoRepeat(false);
}

ToolArea* Toerstein::createToolArea(void)
{
    ToolArea *toolArea = new ToolArea(this,toersteBase);
    connect(toolArea,SIGNAL(leftFilePathChanged(QString)),this,SLOT(setLeftFilePath(QString)));
    connect(toolArea,SIGNAL(rightFilePathChanged(QString)),this,SLOT(setRightFilePath(QString)));
    return toolArea;
}

void Toerstein::createNewFile(void)
{
    ToolArea *toolArea = createToolArea();
    tabView->setCurrentIndex(tabView->addTab(toolArea,"New file"));
    toolArea->setFocusToCodeEditor(ToolAreaRightSide);
}

void Toerstein::createNewTab(void)
{
    ToolArea *toolArea = createToolArea();
    tabView->setCurrentIndex(tabView->addTab(toolArea,"New tab"));
    toolArea->setFocusToFileSearch(ToolAreaRightSide);
}

bool Toerstein::isFileValid(const QString &path)
{
    QFileInfo fileInfo(path);

    if ( !fileInfo.exists() )
    {
        qDebug() << "File not found:" << path;
        return false;
    }

    if ( !fileInfo.isFile() )
    {
        qDebug() << path << "is not a file";
        return false;
    }

    return true;
}

void Toerstein::open(void)
{
    QString path = QFileDialog::getOpenFileName(this,
        tr("Open File"), tr("All files (*.*)"));

    open(path);
}

void Toerstein::open(const QString &path)
{
    ToolArea* toolArea;
    ToolAreaSide side;

    if (!isFileValid(path))
    {
        return;
    }

    for ( int i = 0; i < tabView->count(); i++ )
    {
        toolArea = qobject_cast<ToolArea *>(tabView->widget(i));

        side = toolArea->isFileOpen(path);

        if ( side != ToolAreaNone )
        {
            tabView->setCurrentIndex(i);
            toolArea->setFocusToCodeEditor(side);
            return;
        }
    }

    toolArea = qobject_cast<ToolArea *>(tabView->currentWidget());

    if ( !toolArea->open(path) )
    {
        createNewTab();
    }

    toolArea = qobject_cast<ToolArea *>(tabView->currentWidget());
    toolArea->open(path);
}

void Toerstein::open(const QString &path1, const QString &path2)
{
    if (!isFileValid(path1) || !isFileValid(path2))
    {
        return;
    }

    ToolArea* toolArea = qobject_cast<ToolArea *>(tabView->currentWidget());

    if ( !toolArea->open(path1, path2) )
    {
        createNewTab();
    }

    toolArea = qobject_cast<ToolArea *>(tabView->currentWidget());
    toolArea->open(path1, path2);
}

void Toerstein::search(void)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(tabView->currentWidget());
    toolArea->search();
}

void Toerstein::save(void)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(tabView->currentWidget());
    toolArea->save();
}
void Toerstein::saveAs(void)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(tabView->currentWidget());
    toolArea->saveAs();
}

void Toerstein::closeFile(void)
{
    closeTab(tabView->currentIndex());
}

void Toerstein::closeTab(int tabIndex)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(tabView->widget(tabIndex));
    ToolArea* previousToolArea = NULL;

    if ( tabView->widget(tabIndex) != tabView->currentWidget() )
    {
        previousToolArea = qobject_cast<ToolArea *>(tabView->currentWidget());
    }

    if ( toolArea->hasUnsavedContent() )
    {
        tabView->setCurrentIndex(tabIndex);
        tabView->currentWidget()->setFocus();
    }

    if ( toolArea->closeFile() )
    {
        tabView->removeTab(tabIndex);
    }

    if ( previousToolArea )
    {
        tabView->setCurrentIndex(tabView->indexOf(previousToolArea));
        tabView->currentWidget()->setFocus();
    }
    else if ( tabView->currentWidget() )
    {
        tabView->currentWidget()->setFocus();
    }
    else
    {
        close();
    }
}

void Toerstein::toggleViewMode(void)
{
    qobject_cast<ToolArea *>(tabView->currentWidget())->toggleViewMode();
}

void Toerstein::toggleMenuBarVisibility(void)
{
    menuBar()->setVisible( !menuBar()->isVisible() );
}

void Toerstein::setLeftFilePath(const QString &path)
{
    qDebug() << "Toerstein: Left filepath set to" << path;
}

void Toerstein::setRightFilePath(const QString &path)
{
    ToolArea* toolArea = qobject_cast<ToolArea *>(sender());

    qDebug() << "Toerstein: Right filepath set to" << path;

    int tabIndex = tabView->indexOf(toolArea);

    if ( tabIndex >= 0 )
    {
        if ( path.isEmpty() )
        {
            tabView->setTabText(tabIndex,"New tab");
        }
        else
        {
            tabView->setTabText(tabIndex,QFileInfo(path).fileName());
            emit fileLoaded(path);
        }
    }
}

Toerstein::~Toerstein()
{
}
