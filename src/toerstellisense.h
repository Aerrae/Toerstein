#ifndef TOERSTELLISENSE_H
#define TOERSTELLISENSE_H

#include "toerstebase.h"

#include <QObject>

class ToerstelliSense : public QObject
{
    Q_OBJECT
public:
    explicit ToerstelliSense(QObject *parent = 0, ToersteBase *database = 0);
    void indexFile(QString path);

signals:

public slots:

private:
    ToersteBase *toersteBase;
};

#endif // TOERSTELLISENSE_H
