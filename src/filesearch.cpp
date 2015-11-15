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
#include <QDebug>

FileSearch::FileSearch(QWidget *parent) : QLineEdit(parent)
{
    completer = new QCompleter(this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(true);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    this->setCompleter(completer);
}

void FileSearch::updateList(QStringList list)
{
    QStringListModel *model;
    model = (QStringListModel*)(completer->model());

    if ( list.isEmpty() )
    {
        return;
    }

    qDebug() << "FileSearch: updateList";
    qDebug() << list;

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

        if ( keyEvent->key() == Qt::Key_Tab )
        {
            QString path = completer->currentCompletion();

            qDebug() << "FileSearch: completionCount" << completer->completionCount();

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
    return QLineEdit::event(e);
}
