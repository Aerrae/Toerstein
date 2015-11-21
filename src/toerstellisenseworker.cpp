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

#include "toerstebase.h"
#include "toerstellisenseworker.h"

#include <QDir>

ToerstelliSenseWorker::ToerstelliSenseWorker(ToersteBase *database)
{
    setObjectName("ToerstelliSense");
    toersteBase = database;

    connect(this,SIGNAL(indexingFile(QString)),toersteBase->worker(),SLOT(insertFileInfo(QString)));
}

void ToerstelliSenseWorker::indexFile(const QString &path)
{
    QFileInfo fileInfo = QFileInfo(path);
    QStringList fileNameList;
    QString canonicalPath = fileInfo.canonicalPath();
    QDir directory(canonicalPath);

    fileNameList = directory.entryList(QDir::Files, QDir::Name);

    foreach ( QString fileName, fileNameList )
    {
        QString filePath = canonicalPath + QDir::separator() + fileName;
        emit indexingFile(filePath);
    }
}
