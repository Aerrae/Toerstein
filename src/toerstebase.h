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

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QFileInfo>
#include <QSqlDatabase>

class ToersteBase : public QObject
{
    Q_OBJECT
public:
    explicit ToersteBase(QObject *parent = 0);
     ~ToersteBase();
    bool isFileIndexed(const QFileInfo &fileInfo);

signals:

public slots:
    void queryFileInfo(const QString &filename);
    void insertFileInfo(const QFileInfo &fileInfo);

private slots:

private:
    bool openDatabase(void);
    QSqlDatabase fileDatabase;
};

#endif // DATABASE_H
