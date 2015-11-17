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

FileSearch::FileSearch(QWidget *parent) : QLineEdit(parent)
{
    connect(this,SIGNAL(returnPressed()),this,SLOT(clearCompleter()));
    completer = new QCompleter(this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(true);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    this->setCompleter(completer);

    storeCursor = false;
    restoreCursor = false;
}

void FileSearch::clearCompleter(void)
{
    completer->setModel(NULL);
    storeCursor = false;
    restoreCursor = false;
}

void FileSearch::updateList(QStringList list)
{
    QStringListModel *model;
    model = (QStringListModel*)(completer->model());

    if ( list.isEmpty() )
    {
        return;
    }

    if( !model )
    {
        model = new QStringListModel(this);
    }

    model->setStringList(list);
    completer->setModel(model);
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
        else
        {
            emit keyPressed(this->text());
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
