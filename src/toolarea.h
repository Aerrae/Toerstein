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

#ifndef TOOLAREA_H
#define TOOLAREA_H

#include "codeeditor.h"

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>

class ToolArea : public QWidget
{
    Q_OBJECT
public:
    explicit ToolArea(QWidget *parent = 0);

public slots:
    void search(void);
    void save(void);
    void saveAs(void);
    bool closeFile(void);
    void toggleViewMode(void);
    void setFocusToLeftFileSearch(void);
    void setFocusToRightFileSearch(void);
    void setFocusToLeftCodeEditor(void);
    void setFocusToRightCodeEditor(void);

public:
    bool open(QString path);
    bool open(QString path1, QString path2);

signals:
    void leftFilePathChanged(QString path);
    void rightFilePathChanged(QString path);

private:
    QGridLayout *layout;
    QLineEdit *leftFileSearch;
    CodeEditor *leftCodeEditor;
    QLineEdit *rightFileSearch;
    CodeEditor *rightCodeEditor;
    bool diffModeEnabled;

private slots:
    void setLeftFilePath(QString path);
    void setRightFilePath(QString path);
    void searchFile(void);
};

#endif // TOOLAREA_H
