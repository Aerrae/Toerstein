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

#include "toerstebaseworker.h"

#include <QApplication>
#include <QDir>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

ToersteBaseWorker::ToersteBaseWorker(void)
{
    setObjectName("ToersteBase");
}

void ToersteBaseWorker::openDatabase(void)
{
    if ( fileDatabase.isOpen() )
    {
        return;
    }

    fileDatabase = QSqlDatabase::addDatabase("QSQLITE");
    fileDatabase.setDatabaseName( QApplication::applicationDirPath()
                                 + QDir::separator()
                                 + "filedatabase.sqlite");

    if (!fileDatabase.open())
    {
        qDebug() << "Cannot open database";

        return;
    }

    QSqlQuery query;

    query.exec("CREATE TABLE IF NOT EXISTS file (id INT auto_increment primary key, "
               "filePath TEXT, autoComplete TEXT)");
}

void ToersteBaseWorker::queryFileInfo(const QString &fileNameToSearch)
{
    if( !fileDatabase.isOpen() )
    {
        qDebug() << "ToersteBase: Error: Database is not open";
        return;
    }

    QString autoComplete;
    QStringList autoCompleteList;
    QSqlQuery query;
    QString queryString =
            "SELECT * FROM file WHERE autoComplete LIKE '" + fileNameToSearch + "%'";

    qDebug() << "ToersteBase: fileinfo query" << fileNameToSearch;

    query.prepare(queryString);
    query.exec();

    int autoCompleteField = query.record().indexOf("autoComplete");

    while ( query.next() )
    {
        autoComplete = query.value(autoCompleteField).toString();
        autoCompleteList.append(autoComplete);
    }

    QMetaObject::invokeMethod(
        sender(),
        "updateList",
        Qt::QueuedConnection,
        Q_ARG(QStringList, autoCompleteList));

}

bool ToersteBaseWorker::isFileIndexed(const QString &path)
{
    QSqlQuery query;
    QString sqlError;
    QString queryString = "SELECT * FROM file WHERE filePath = '" + path + "'";

    qDebug() << "ToersteBase: Checking if file" << path << "is in database.";
    qDebug() << "ToersteBase: SQL query:" << queryString;

    query.prepare(queryString);

    query.exec();
    sqlError = query.lastError().databaseText();

    if ( !sqlError.isEmpty() )
    {
        qDebug() << "ToersteBase: SQL error:" << sqlError;
    }

    if ( query.next() )
    {
        qDebug() << "ToersteBase: file" << path << "found from database";
        return true;
    }
    else
    {
        qDebug() << "ToersteBase: file" << path << "not found from database";
        return false;
    }

}

void ToersteBaseWorker::insertFileInfo(const QString &path)
{
    if( !fileDatabase.isOpen() )
    {
        qDebug() << "ToersteBase: Error: Database is not open";
        return;
    }

    QFileInfo fileInfo(path);

    if ( isFileIndexed(path) )
    {
        return;
    }

    QSqlQuery query;
    QString sqlError;

    QString autoComplete = fileInfo.fileName() + " ( " +
        QDir::toNativeSeparators(fileInfo.canonicalPath()) +
        QDir::separator() + fileInfo.fileName() + " )";

    QString queryString =
            "INSERT INTO file (filePath, autoComplete) VALUES ('" + path +
            "', '" + autoComplete + "')";

    qDebug() << "ToersteBase: inserting file info for" << fileInfo.fileName() <<
        "with path" << fileInfo.canonicalPath();

    qDebug() << "ToersteBase: SQL query:" << queryString;

    query.prepare(queryString);
    query.exec();
    sqlError = query.lastError().databaseText();

    if ( !sqlError.isEmpty() )
    {
        qDebug() << "ToersteBase: SQL error:" << sqlError;
    }
}

void ToersteBaseWorker::deleteFileInfo(const QString &path)
{
    if( !fileDatabase.isOpen() )
    {
        qDebug() << "ToersteBase: Error: Database is not open";
        return;
    }

    QFileInfo fileInfo(path);

    QSqlQuery query;
    QString sqlError;

    QString queryString = "DELETE FROM file WHERE filePath = '" + path + "'";

    qDebug() << "ToersteBase: deleting file info of" << path;
    qDebug() << "ToersteBase: SQL query:" << queryString;

    query.prepare(queryString);
    query.exec();
    sqlError = query.lastError().databaseText();

    if ( !sqlError.isEmpty() )
    {
        qDebug() << "ToersteBase: SQL error:" << sqlError;
    }
}

ToersteBaseWorker::~ToersteBaseWorker()
{
    if( fileDatabase.isOpen() )
    {
        fileDatabase.close();
    }
}
