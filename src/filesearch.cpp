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

#include "filesearch.h"
#include "toerstebase.h"

#include <QStringListModel>
#include <QStringList>
#include <QLineEdit>
#include <QCompleter>
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>

FileSearch::FileSearch(QWidget *parent) : QLineEdit(parent)
{
    completer = new QCompleter(this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(true);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    this->setCompleter(completer);

    dirModel = new QDirModel(this);
    stringListModel = new QStringListModel(this);

    storeCursor = false;
    restoreCursor = false;
    timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(this,SIGNAL(returnPressed()),this,SLOT(clearCompleter()));
    connect(timer,SIGNAL(timeout()),this,SLOT(userVatulating()));
}

void FileSearch::clearCompleter(void)
{
    completer->setModel(NULL);
    storeCursor = false;
    restoreCursor = false;
}

void FileSearch::updateList(const QStringList &list)
{
    if ( list.isEmpty() )
    {
        return;
    }

    stringListModel->setStringList(list);

    if ( completer->model() != stringListModel )
    {
        completer->setModel(stringListModel);
    }

    completer->complete();
}

bool FileSearch::event(QEvent* e)
{
    if ( e->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);

        storeCursor = true;

        if ( keyEvent->key() == Qt::Key_Tab )
        {
            if ( restoreCursor )
            {
                setCursorPosition(0);
                restoreCursor = false;
            }

            QString path = completer->currentCompletion();

            if ( completer->completionCount() == 1 )
            {
                this->setText(path);
            }
            else
            {
                if(!completer->setCurrentRow(completer->currentRow() + 1 ))
                {
                completer->setCurrentRow(0);
                }

                completer->complete();
            }
            return true;
        }
        else if ( keyEvent->key() == Qt::Key_Backspace )
        {
            timer->start(600);
        }
        else
        {
            timer->start(200);
        }
    }
    else if ( storeCursor && ( e->type() == QEvent::MouseButtonPress ) )
    {
        cursorOldPos = cursorPosition();
    }

    bool eventReturn = QLineEdit::event(e);

    if ( storeCursor && ( e->type() == QEvent::MouseButtonPress ) &&
        ( cursorOldPos != cursorPosition() ) )
    {
        storeCursor = false;
        restoreCursor = true;
    }

    return eventReturn;
}

void FileSearch::userVatulating()
{
    QRegExp path_unix_rx,path_win_rx;

    /* Check if search fields starts with "/" or "x:" */
    path_unix_rx.setPattern("^\\/");
    path_win_rx.setPattern("^[a-zA-Z]:");

    if ( ( path_unix_rx.indexIn(this->text()) != -1 ) ||
        ( path_win_rx.indexIn(this->text()) != -1 ) )
    {
        /* Switch to directory autocompleter model */
        if (completer->model() != dirModel )
        {
            completer->setModel(dirModel);
        }
    }
    else
    {
        /* updateList will change autocompleter model to stringListModel
         * after database indicates results for the search
         */
        emit writingPaused(this->text());
    }
}
