#include "toerstellisense.h"

#include <QString>
#include <QFileInfo>

ToerstelliSense::ToerstelliSense(QObject *parent, ToersteBase *database) : QObject(parent)
{
    toersteBase = database;
}

void ToerstelliSense::indexFile(QString path)
{
    QFileInfo fileInfo = QFileInfo(path);

    if ( !toersteBase->isFileIndexed(fileInfo) )
    {
        toersteBase->insertFileInfo(fileInfo);
    }
}
