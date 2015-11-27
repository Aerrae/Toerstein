#include "tabview.h"

#include <QTabWidget>
#include <QTabBar>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>

TabView::TabView(QWidget *parent) : QTabWidget(parent)
{
    tabBar()->installEventFilter(this);
}

bool TabView::eventFilter(QObject *o, QEvent *e)
{
    if ( o == tabBar() && ( e->type() == QEvent::MouseButtonPress ) )
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);

        if ( mouseEvent->buttons() & Qt::MidButton )
        {
            emit tabCloseRequested( tabBar()->tabAt(mouseEvent->pos()) );
            return true;
        }
    }

    return QTabWidget::eventFilter(o, e);
}
