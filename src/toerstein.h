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

#ifndef TOERSTEIN_H
#define TOERSTEIN_H

#include "toerstebase.h"
#include "toerstellisense.h"
#include "toolarea.h"
#include "tabview.h"

#include <QMainWindow>

class Toerstein : public QMainWindow
{
    Q_OBJECT

public:
    explicit Toerstein(QWidget *parent = 0);
    ~Toerstein();

private slots:
    void createNewFile(void);
    void createNewTab(void);
    void open(void);
    void search(void);
    void save(void);
    void saveAs(void);
    void closeFile(void);
    void closeTab(int tabIndex);
    void toggleViewMode(void);
    void setLeftFilePath(const QString &path);
    void setRightFilePath(const QString &path);

signals:
    void fileLoaded(const QString &path);

private:
    void createMenuBar(void);
    ToolArea* createToolArea(void);
    bool isFileValid(const QString &path);
    void open(const QString &path);
    void open(const QString &path1, const QString &path2);
    TabView *tabView;
    ToersteBase *toersteBase;
    ToerstelliSense *toerstelliSense;
};

#endif // TOERSTEIN_H
