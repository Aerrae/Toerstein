#include "toerstebase.h"

#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>

ToersteBase::ToersteBase(QObject *parent) : QObject(parent)
{
    openDatabase();
}

bool ToersteBase::openDatabase(void)
{
    if ( fileDatabase.isOpen() )
    {
        return true;
    }

    fileDatabase = QSqlDatabase::addDatabase("QSQLITE");
    fileDatabase.setDatabaseName( QApplication::applicationDirPath()
                                 + QDir::separator()
                                 + "filedatabase.sqlite");

    if (!fileDatabase.open())
    {
        qDebug() << "Cannot open database";

        return false;
    }

    QSqlQuery query;

    query.exec("CREATE TABLE IF NOT EXISTS file (id INT auto_increment primary key, "
               "fileName TEXT, canonicalPath TEXT)");

    return true;
}

void ToersteBase::queryFileInfo(QString fileNameToSearch)
{
    QString fileName;
    QStringList fileNameList;
    QSqlQuery query;
    QString queryString =
            "SELECT * FROM file WHERE fileName LIKE '" + fileNameToSearch + "%'";

    qDebug() << "ToersteBase: fileinfo query" << fileNameToSearch;

    query.prepare(queryString);
    query.exec();

    int fileNameField = query.record().indexOf("fileName");
    int pathField = query.record().indexOf("canonicalPath");

    qDebug() << "Query:" << fileNameToSearch;

    while ( query.next() )
    {
        fileName = query.value(fileNameField).toString() + " ( " + query.value(pathField).toString() +
                QDir::separator() + query.value(fileNameField).toString() + " )";
        fileNameList.append(fileName);
    }

    QMetaObject::invokeMethod(
        sender(),
        "updateList",
        Qt::QueuedConnection,
        Q_ARG(QStringList, fileNameList));

}

bool ToersteBase::isFileIndexed(QFileInfo fileInfo)
{
    QSqlQuery query;
    QString sqlError;
    QString queryString =
            "SELECT * FROM file WHERE fileName = '" + fileInfo.fileName() +
            "' AND canonicalPath = '" + fileInfo.canonicalPath() + "'";

    qDebug() << "ToersteBase: Checking if file" << fileInfo.fileName() << "is in database.";
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
        qDebug() << "ToersteBase: file" << fileInfo.fileName() << "found from database";
        return true;
    }
    else
    {
        qDebug() << "ToersteBase: file" << fileInfo.fileName() << "not found from database";
        return false;
    }

}

void ToersteBase::insertFileInfo(QFileInfo fileInfo)
{
    QSqlQuery query;
    QString sqlError;

    QString queryString =
            "INSERT INTO file (fileName, canonicalPath) VALUES ('" + fileInfo.fileName() +
            "', '" + fileInfo.canonicalPath() + "')";

    qDebug() << "ToersteBase: inserting file info for" << fileInfo.fileName() << "with path" << fileInfo.canonicalPath();
    qDebug() << "ToersteBase: SQL query:" << queryString;

    query.prepare(queryString);
    query.exec();
    sqlError = query.lastError().databaseText();

    if ( !sqlError.isEmpty() )
    {
        qDebug() << "ToersteBase: SQL error:" << sqlError;
    }
}

ToersteBase::~ToersteBase()
{
    if( fileDatabase.isOpen() )
    {
        fileDatabase.close();
    }
}
