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

#include "toerstebaseworker.h"

#include <QObject>
#include <QThread>

class ToersteBase : public QObject
{
    Q_OBJECT
public:
    explicit ToersteBase(QObject *parent = 0);
     ~ToersteBase();
    ToersteBaseWorker *worker(void);
    bool isFileIndexed(const QFileInfo &fileInfo);

signals:
    void openDatabase(void);
    void closeDatabase(void);

public slots:

private slots:

private:
    QThread toersteBaseThread;
    ToersteBaseWorker *toersteBaseWorker;
};

#endif // DATABASE_H
