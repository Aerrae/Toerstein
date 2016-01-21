#ifndef TABVIEW_H
#define TABVIEW_H

#include <QWidget>
#include <QTabWidget>

class TabView : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabView(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *o, QEvent *e);

signals:
    void userRequestedTabClose(int tabIndex);

public slots:
};

#endif // TABVIEW_H
