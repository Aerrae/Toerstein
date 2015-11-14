#ifndef FILESEARCH_H
#define FILESEARCH_H

#include <QWidget>
#include <QLineEdit>
#include <QCompleter>

class FileSearch : public QLineEdit
{
    Q_OBJECT
public:
    explicit FileSearch(QWidget *parent = 0);

signals:
    void keyPressed(QString text);

public slots:
    void updateList(QStringList list);

protected:
    bool event(QEvent* e);

private:
    QCompleter *completer;
};

#endif // FILESEARCH_H
