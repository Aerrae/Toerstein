/************************************************************************************
**
** Toerstein - Code Editor
** Copyright (C) 2016  Tommi Tauriainen
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

#include "toerstediff.h"
#include "codeeditor.h"

#include <QTextBlock>
#include <QScrollBar>

ToersteDiff::ToersteDiff(QObject *parent, CodeEditor *leftEditor, CodeEditor *rightEditor) : QObject(parent)
{
    leftCodeEditor = leftEditor;
    rightCodeEditor = rightEditor;

    connect(leftCodeEditor, SIGNAL(cursorPositionChanged()), this, SLOT(syncCursors()));
    connect(rightCodeEditor, SIGNAL(cursorPositionChanged()), this, SLOT(syncCursors()));

    connect(leftCodeEditor->verticalScrollBar(), SIGNAL(valueChanged(int)), rightCodeEditor->verticalScrollBar(), SLOT(setValue(int)));
    connect(leftCodeEditor->horizontalScrollBar(), SIGNAL(valueChanged(int)), rightCodeEditor->horizontalScrollBar(), SLOT(setValue(int)));
    connect(rightCodeEditor->verticalScrollBar(), SIGNAL(valueChanged(int)), leftCodeEditor->verticalScrollBar(), SLOT(setValue(int)));
    connect(rightCodeEditor->horizontalScrollBar(), SIGNAL(valueChanged(int)), leftCodeEditor->horizontalScrollBar(), SLOT(setValue(int)));
}

void ToersteDiff::syncToReferenceCursor(QTextCursor &targetCursor, const QTextCursor &referenceCursor)
{
    int blockNumberDiff;

    if ( targetCursor.blockNumber() < referenceCursor.blockNumber() )
    {
        blockNumberDiff = referenceCursor.blockNumber() - targetCursor.blockNumber();
        targetCursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, blockNumberDiff);
    }
    else if ( targetCursor.blockNumber() > referenceCursor.blockNumber() )
    {
        blockNumberDiff = targetCursor.blockNumber() - referenceCursor.blockNumber();
        targetCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor, blockNumberDiff);
    }
}

void ToersteDiff::syncCursors(void)
{
    QTextCursor leftCursor = leftCodeEditor->textCursor();
    QTextCursor rightCursor = rightCodeEditor->textCursor();

    if ( sender() == rightCodeEditor )
    {
        syncToReferenceCursor(leftCursor, rightCursor);
        leftCodeEditor->setTextCursor(leftCursor);
    }
    else
    {
        syncToReferenceCursor(rightCursor, leftCursor);
        rightCodeEditor->setTextCursor(rightCursor);
    }
}
