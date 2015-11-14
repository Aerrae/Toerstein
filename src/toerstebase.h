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
    bool isFileIndexed(QFileInfo fileInfo);

signals:

public slots:
    void queryFileInfo(QString filename);
    void insertFileInfo(QFileInfo fileInfo);

private slots:

private:
    bool openDatabase(void);
    QSqlDatabase fileDatabase;
};

#endif // DATABASE_H
