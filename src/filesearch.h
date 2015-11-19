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

#ifndef FILESEARCH_H
#define FILESEARCH_H

#include <QWidget>
#include <QLineEdit>
#include <QCompleter>
#include <QTimer>

class FileSearch : public QLineEdit
{
    Q_OBJECT
public:
    explicit FileSearch(QWidget *parent = 0);

signals:
    void writingPaused(const QString &text);

public slots:
    void updateList(const QStringList &list);

protected:
    bool event(QEvent* e);

private:
    QCompleter *completer;
    bool storeCursor;
    bool restoreCursor;
    int cursorOldPos;
    QTimer *timer;

private slots:
    void clearCompleter(void);
    void userVatulating(void);
};

#endif // FILESEARCH_H
